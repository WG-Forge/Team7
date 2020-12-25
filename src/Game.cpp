#include "Game.h"
#include "Enums/ActionEnum.h"
#include <QThread>
#include <QApplication>

Game::Game(QObject *parent) : QObject(parent)
{

}

void Game::start() {

}

void Game::end() {

}

void Game::connectToServer() {
    qDebug() << "Connecting";

    socket_ = new Socket();
    this->socket_->connect();
}

void Game::login(QString userName) {
    qDebug() << "LogIn";

    QJsonObject data;
    data["name"] = userName;
    this->socket_->sendData(Request(Action::LOGIN, data));
    QJsonObject userData = socket_->getData();

    player_ = new Player(userData);
}

void Game::logout() {
    qDebug() << "LogOut";

    connected_ = false;
    this->socket_->close();
}

void Game::init() {
    connectToServer();
    getGamesList();
    connected_ = true;
}

void Game::hostGame(QString name, int players, int ticks) {
    QJsonObject request;
    request["name"] = name;
    request["password"];
    request["num_players"] = players;
    request["num_turns"] = ticks;
    qDebug() << request;

    socket_->sendData(Request(Action::LOGIN, request));
    QJsonObject response = socket_->getData();
//    qDebug() << response;
}

void Game::disconnect() {
    qDebug() << "Connection closed";
    this->player().setInGame(false);
    connected_ = false;

    this->socket_->close();
}

void Game::getMap() {
    socket_->sendData(Request(Action::MAP, QJsonObject({{"layer", 0}})));
     layer_0 = socket_->getData();

    socket_->sendData(Request(Action::MAP, QJsonObject({{"layer", 1}})));
     layer_1 = socket_->getData();

    socket_->sendData(Request(Action::MAP, QJsonObject({{"layer", 10}})));
     layer_2 = socket_->getData();
}

void Game::makeMap() {
    qDebug() << "Draw map";
    map_ = std::make_shared<Map>(layer_0, layer_1, layer_2, *player_);
    map_->makeWays(this->player().town());
}

void Game::getGamesList() {
    socket_->sendData(Request(Action::GAMES, QJsonObject()));
    QJsonObject response = socket_->getData();
    qDebug() << response;

    emit getGames(response);
}

void Game::updateGames() {
    qDebug() << "UPDATE";
    this->getGamesList();
}

bool Game::isGameStarted(const QString &name, const int &players) {
    socket_->sendData(Request(Action::GAMES, QJsonObject()));
    QJsonObject response = socket_->getData();
    qDebug() << "IS START?";

    for (auto game : response["games"].toArray()) {
        if (game.toObject()["name"].toString() == name) {
            if (game.toObject()["state"].toInt() == 2) {
                qDebug() << game.toObject()["name"].toString()
                         << game.toObject()["num_players"].toInt()
                         << game.toObject()["state"].toInt();
                return true;
            }
        }
//        qDebug() << game.toObject()["name"].toString()
//                 << game.toObject()["num_players"].toInt()
//                 << game.toObject()["state"].toInt();
    }

    return false;
}

void Game::connectToGame(const QString &userName, const QString &password, const QString &gameName, const int &players, const int &ticks) {
    QJsonObject request;
    request["name"] = userName;
    request["game"] = gameName;
    request["password"] = password;
    request["num_players"] = players;
    request["num_turns"] = ticks;
    qDebug() << request;

    socket_->sendData(Request(Action::LOGIN, request));
    QJsonObject response = socket_->getData();
    if (response.contains("error")) {
        qDebug() << "ERROR" << response;
        return;
    }

    player_ = new Player(response);
    player_->setPassword(password);

    this->setEnemiesCount(players - 1);
    this->setGameName(gameName);
    this->setTotalTicks(ticks);
    this->setCurrentTick(0);

    this->getMap();
    this->makeMap();
    this->player().setInGame(true);
    emit playerChanged(*player_);

    emit mapChanged(std::make_shared<Map>(*map_), *player_, true);
    emit showMap();

    qDebug() << this->isGameStarted(gameName, players);
    while (!this->isGameStarted(gameName, players)) {
        continue;
    }

    this->gameCycle();
}

void Game::gameCycle() {
    qDebug() << "cycle";

    int currentIdx = 0,
        currentPos = 0;
    QJsonObject response;

    const int USER_POST_IDX = this->player().town().pointIdx();
    const int USER_POST_POS = this->map()->graph().idx().at(USER_POST_IDX);
    currentIdx = USER_POST_IDX;
    currentPos = USER_POST_POS;
    int i = 0;
    for (auto &train : this->player().trains()) {
        train->setCurrentVertex(&this->player().town().vertex());
        train->setWaitingTime(i);
        if(i == 2){
            i += 5;
        }
        else{++i;}
    }

//    emit mapChanged(std::make_shared<Map>(*map_), *player_, true);
    int tickCount = 0;
    this->shortestWay(this->player().trains()[0], this->player().town().vertex(), this->map()->markets()[0].vertex());

    std::vector<Town*> upgradeTowns;
    std::vector<Train*> upgradeTrains;

    while(connected_) {
//        emit infoChange(*player_); // замутить обнову ui
        emit mapChanged(std::make_shared<Map>(*map_), *player_, false);
        for(auto &train : this->player().trains()){
                if (train->edge() != nullptr) {
                    qDebug() << "Not noll:";
                    qDebug() << "Current:" << train->currentVertex()->idx()
                             << "Next: " << train->nextVertex()->idx();
                } else qDebug() << "Null:" << tickCount;
                if(train->waitingTime() <= 0 && train->events().size() == 0){
                    this->wayStrategy(train);
                    this->sendTrain(train);
                    qDebug() << train->cooldown();
                }
                train->setWaitingTime(train->waitingTime() - 1);
                this->upgradeStrategy(train, upgradeTowns, upgradeTrains);
           // }
        }

        tickCount++;
        this->tick();
        this->updateUser();
        this->updatePosts();

        QApplication::processEvents();
    }
}
// устанавливает current / next / final
void Game::sendTrain(Train *train) {
    if (train->edge() == nullptr) {
        qDebug() << "АТДИХАЕМ ПАЦАНВА"
                 << "Current:" << train->currentVertex()->idx();
        return;
    }

//   if (train->nextVertex() == nullptr) return;

    Edge *currentLine = train->edge();
    Vertex start;
    Vertex end;
    int speed = train->speed();

    qDebug() << "Start pos:" << train->currentVertex()->idx()
             << "Final pos:" << train->finalVertex()->idx();
    qDebug() << "Line:" << currentLine->idx()
             << "V1:" << currentLine->vertex1().idx()
             << "V2:" << currentLine->vertex2().idx();

    if (train->speed() != 0) {
        qDebug() << "Edem FROM:" << train->currentVertex()->idx()
                 << "TO:" << train->nextVertex()->idx()
                 << "Position:" << train->position();
    } else {
        qDebug() << "Stoim V:" << train->edge()->idx()
                 << "Current:" << train->currentVertex()->idx()
                 << "Position:" << train->position();
    }
    if(train->speed() == 0){
    if (currentLine->vertex1().idx() == train->currentVertex()->idx()) {
        train->setNextVertex(&currentLine->vertex2());
        train->setFinalLinePosition(currentLine->length());
    } else {
        train->setNextVertex(&currentLine->vertex1());
        train->setFinalLinePosition(0);
    }
    if (train->currentVertex()->idx() < train->nextVertex()->idx()) {
        speed = 1;
    } else speed = -1;
    this->moveAction(train, currentLine, speed);
    }
}

void Game::moveAction(Train *train, Edge *moveLine, int speed) {
    QJsonObject request;
    request["line_idx"] = moveLine->idx();
    request["speed"] = speed;
    request["train_idx"] = train->idx();
    qDebug() << request;

    socket_->sendData(Request(Action::MOVE, request));
    QJsonObject response = socket_->getData();

    if (response != QJsonObject()) {
        qDebug() << "MOVEMENT ERROR" << response;
        throw(response);
    }
}

void Game::tick() {
    this->socket_->sendData(Request(Action::TURN, QJsonObject()));
    QJsonObject response = socket_->getData();
}

double Game::heuristic(Vertex *v1, Vertex *v2) {
    int mapSize = 10;
    int min = this->map()->graph().minVertexIdx();
    double x1, x2, y1, y2;
    x1 = (v1->idx() - min) % mapSize;
    x2 = (v2->idx() - min) % mapSize;
    y1 = floor((v1->idx() - min) / mapSize);
    y2 = floor((v2->idx() - min) / mapSize);


    return abs(x1 - x2) + abs(y1 - y2);
};

void Game::shortestWay(Train *train, Vertex &start, Vertex &goal) {
    int minVertex = this->map()->graph().minVertexIdx();
    int newCost = 0;
    double priority = 0;

    PriorityQueue<Vertex *, double> frontier2;
    frontier2.put(&start, 0);

    std::unordered_map<Vertex *, double> cost_so_far;
    std::unordered_map<Vertex *, Vertex *> came_from;
    std::vector<Vertex *> neighbors;
    Vertex *current;

    cost_so_far.insert(std::pair<Vertex*, double>(&start, 0));
    came_from.insert(std::pair<Vertex*, Vertex*>(&start, &start));

    this->map()->graph().setNewLength(70, 80, 1000);

    while (!frontier2.empty()) {
        current = frontier2.get();
        if (current->idx() == goal.idx()) break;

        for (auto next : current->edges()) {
            if (current->idx() == next.get().vertex1().idx()) neighbors.emplace_back(&next.get().vertex2());
            else neighbors.emplace_back(&next.get().vertex1());
        }

        for (auto &next : neighbors) {
            double new_cost = cost_so_far[current] + this->map()->graph().lengthBetween(current->idx(), next->idx());

            if (!cost_so_far.count(next) || new_cost < cost_so_far[next]) {
                cost_so_far[next] = new_cost;
                double priority = new_cost + heuristic(next, &goal);
                frontier2.put(next, priority);
                came_from[next] = current;
            }
        }
        neighbors.clear();
    }


    std::vector<Vertex *> path; // ВОТ ТУТ ПУТЬ, МОЖНО ЕГО ВЕРНУТЬ ТАМ ИЛИ ЧЁ-ТО ЕЩЁ
    current = &goal;
    path.emplace_back(current);

    while(current != &start) {
        current = came_from[current];
        path.emplace_back(current);
    }
    path.emplace_back(&start);

    std::reverse(path.begin(), path.end());
    std::cout << "PATH: ";
    for (auto &c : path) {
        std::cout << c->idx() << " -> ";
    }
    std::cout << "\n";
    std::cout << std::endl;

    this->map()->graph().restoreMatrix(); // ТУТ ВОССТАНАВЛИВАЕТСЯ МАТРИЦА, НО МОЖНО ЭТОГО И НЕ ДЕЛАТЬ
}

Vertex& Game::findPostVertex(PostType type, Vertex currentVertex, Train *train) {
    bool isGoods = false;
    int shortestPos = 0;
    int wayLength = 1000000;
    int marketPos;
    int currentLength;
    Vertex *postVertex = new Vertex();
    int currentPos = this->map()->graph().idx().at(currentVertex.idx());

    switch(type) {
    case PostType::TOWN:
        for (auto &town : this->map()->towns()) {
            marketPos = this->map()->graph().idx().at(town.pointIdx());
            currentLength = train->waysLengthStorage()[currentPos][marketPos];

            if (currentLength < wayLength) {
                wayLength = currentLength;
                shortestPos = marketPos;
                *postVertex = town.vertex();
            }
        }

        break;

    case PostType::MARKET:
        for (auto &market : this->map()->markets()) {
            if (market.product() == 0) continue;
            isGoods = true;
            marketPos = this->map()->graph().idx().at(market.pointIdx());
            currentLength = train->waysLengthMarket()[currentPos][marketPos];

            if (currentLength < wayLength) {
                wayLength = currentLength;
                shortestPos = marketPos;
                *postVertex = market.vertex();
            }
        }

        break;

    case PostType::STORAGE:
        for (auto &storage : this->map()->storages()) {
            if (storage.armor() == 0) continue;
            isGoods = true;
            marketPos = this->map()->graph().idx().at(storage.pointIdx());
            currentLength = train->waysLengthStorage()[currentPos][marketPos];

            if (currentLength < wayLength) {
                wayLength = currentLength;
                shortestPos = marketPos;
                *postVertex = storage.vertex();
            }
        }

        break;
    }

    return *postVertex;
}

void Game::updateUser() {
    socket_->sendData(Request(Action::PLAYER, QJsonObject()));
    QJsonObject response = socket_->getData();

    this->player().town().update(response["town"].toObject());
    this->player().update(response);

    int trainIndex = 0;
    for (auto &train : this->player().trains()) {
        train->update(response["trains"].toArray()[trainIndex].toObject());
        trainIndex++;
    }

    qDebug() << "SERVER: Product:" << this->player().town().product()
             << "Population:" << this->player().town().population()
             << "Armor:" << this->player().town().armor()
             << "Train:" << this->player().trains()[0]->lineIdx()
             << this->player().trains()[0]->position()
             << this->player().trains()[0]->speed()
             << this->player().trains()[0]->goods()
             << (int)this->player().trains()[0]->goodsType();
}

void Game::updatePosts() {
    socket_->sendData(Request(Action::MAP, QJsonObject({{"layer", 1}})));
     layer_1 = socket_->getData();

     int marketIter = 0;
     int townIter = 0;
     int storageIter = 0;

     QJsonArray posts = layer_1["posts"].toArray();

     for (auto const &post : posts) {
         int postType = post.toObject()["type"].toInt();

         switch(postType) {
         case 1:
             this->map()->towns()[townIter].update(post.toObject());
             townIter++;
             break;
         case 2:
             this->map()->markets()[marketIter].update(post.toObject());
             marketIter++;
             break;
         case 3:
             this->map()->storages()[storageIter].update(post.toObject());
             storageIter++;
             break;
         }
     }
}

Edge& Game::getLine(Train *train, Vertex start, Vertex end, enum WaysType wayType) {
    Edge *moveEdge = new Edge();

    switch(wayType) {
    case WaysType::RETURN:
        moveEdge = train->waysMarket()[this->map()->graph().idx().at(start.idx())][this->map()->graph().idx().at(end.idx())];
        break;

    case WaysType::MARKET:
        moveEdge = train->waysMarket()[this->map()->graph().idx().at(start.idx())][this->map()->graph().idx().at(end.idx())];
        break;

    case WaysType::STORAGE:
        moveEdge = train->waysStorage()[this->map()->graph().idx().at(start.idx())][this->map()->graph().idx().at(end.idx())];
        break;

    case WaysType::ANYTING:
        moveEdge = train->waysMarket()[this->map()->graph().idx().at(start.idx())][this->map()->graph().idx().at(end.idx())];
        break;
    }

    return *moveEdge;
}

int Game::getIdx(int position) {
    int idx;

    for(auto it = this->map()->graph().idx().begin(); it != this->map()->graph().idx().end(); ++it) {
        if(this->map()->graph().idx()[(*it).first] == position) {
            idx = it->first;
        }
    }

    return idx;
}

void Game::upgradeAction(std::vector<Town*> towns, std::vector<Train*> trains){
    QJsonArray townsArray;
    QJsonArray trainsArray;
    QJsonObject request;
    for(auto town: towns){
        townsArray.append(town->idx());
    }
    for(auto train: trains){
        trainsArray.append(train->idx());
    }
    request["posts"] = townsArray;
    request["trains"] = trainsArray;

    this->socket_->sendData(Request(Action::UPGRADE, request));
    socket_->getData();

}

void Game::wayStrategy(Train* trainPlayer){
    if(trainPlayer->edge() == nullptr){//поезд стоит, не может ехать
        if(trainPlayer->currentVertex() == trainPlayer->finalVertex()){//Поезд достиг точки назначения
            if(trainPlayer->finalVertex()->isPostIdxNull() == false){//Мы на каком то посту
                switch (static_cast<int>(trainPlayer->finalVertex()->post().type())) {
                case 1:{//Поезд в городе
                    if(trainPlayer->finalVertex()->postIdx() != player().town().idx()){//Поезд не дома, в городе
                    }
                    else{//Поезд дома, чисто чилит пока что

                    }
                    break;}
                case 2:{//Поезд в маркете, забрал продукты, мама будет довольна
                    trainPlayer->setCurrentVertex(trainPlayer->finalVertex());
                    trainPlayer->setFinalVertex(&player().town().vertex());
                    if(trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                            [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]->vertex1().idx()
                            == trainPlayer->currentVertex()->idx()){ trainPlayer->
                                setEdge(trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]);}
                            else{trainPlayer->
                                setEdge(trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]);}
                    trainPlayer->setWaysType(static_cast<WaysType>(1));
                    break;}
                case 3:{//Поезд в стораже, одевается в доспехи наверно
                    trainPlayer->setCurrentVertex(trainPlayer->finalVertex());
                    trainPlayer->setFinalVertex(&player().town().vertex());
                    if(trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                            [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]->vertex1().idx()
                            == trainPlayer->currentVertex()->idx()){ trainPlayer->
                                setEdge(trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]);}
                            else{trainPlayer->
                                setEdge(trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]);}
                    trainPlayer->setWaysType(static_cast<WaysType>(1));
                    break;}
                }

            }
            else{//Мы не на посту, чё то соображаем полагаю

            }
        }
        else{//Поезд НЕ достиг точки назначения, WHY или в городе нашем
            if(trainPlayer->finalVertex() == nullptr){//Тут логика отправления поезда
                if(static_cast<int>(this->player().trains()[0]->waysType()) == 2){
                    if(trainPlayer->idx() == this->player().trains()[2]->idx()){
                        trainPlayer->setCurrentVertex(&this->player().town().vertex());
                        trainPlayer->setFinalVertex(&findPostVertex(PostType::MARKET, this->player().town().vertex(), trainPlayer));
                        trainPlayer->setEdge(trainPlayer->waysMarket()[map()->graph().idx().at(this->player().town().vertex().idx())]
                                [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]);
                        trainPlayer->setWaysType(static_cast<WaysType>(2));
                    }
                    else{
                    trainPlayer->setCurrentVertex(&this->player().town().vertex());
                    trainPlayer->setFinalVertex(&findPostVertex(PostType::STORAGE, this->player().town().vertex(), trainPlayer));
                    trainPlayer->setEdge(trainPlayer->waysStorage()[map()->graph().idx().at(this->player().town().vertex().idx())]
                            [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]);
                    trainPlayer->setWaysType(static_cast<WaysType>(3));
                    }
                }
                else{
                    if(this->player().town().product() <= this->player().town().productCapacity()){
                        trainPlayer->setCurrentVertex(&this->player().town().vertex());
                        trainPlayer->setFinalVertex(&findPostVertex(PostType::MARKET, this->player().town().vertex(), trainPlayer));
                        trainPlayer->setEdge(trainPlayer->waysMarket()[map()->graph().idx().at(this->player().town().vertex().idx())]
                                [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]);
                        trainPlayer->setWaysType(static_cast<WaysType>(2));
                    }
                }
            }
            else{

            }
        }
    }
    else{//Поезд едет

        if(trainPlayer->speed() == 0){
            if(trainPlayer->edge()->vertex1().idx() == trainPlayer->finalVertex()->idx() ||
                    trainPlayer->edge()->vertex2().idx() == trainPlayer->finalVertex()->idx()){
                if(trainPlayer->finalVertex()->idx() == this->player().town().vertex().idx()){
                    trainPlayer->setFinalVertex(nullptr);
                    trainPlayer->setCurrentVertex(&this->player().town().vertex());
                }
                else{
                    trainPlayer->setCurrentVertex(trainPlayer->finalVertex());
                }
                trainPlayer->setEdge(nullptr);
                this->wayStrategy(trainPlayer);
            }
            else{
                switch(static_cast<int>(trainPlayer->waysType())){
                case 1:{
                    if(trainPlayer->currentVertex()->idx() == trainPlayer->edge()->vertex1().idx()){
                        trainPlayer->setCurrentVertex(&trainPlayer->edge()->vertex2());
                    }
                    else{
                        trainPlayer->setCurrentVertex(&trainPlayer->edge()->vertex1());
                    }
                    trainPlayer->setEdge(trainPlayer->waysReturn()
                            [map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                            [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]);
                    break;}
                case 2:{
                    if(trainPlayer->currentVertex()->idx() == trainPlayer->edge()->vertex1().idx()){
                        trainPlayer->setCurrentVertex(&trainPlayer->edge()->vertex2());
                    }
                    else{
                        trainPlayer->setCurrentVertex(&trainPlayer->edge()->vertex1());
                    }
                    trainPlayer->setEdge(trainPlayer->waysMarket()
                            [map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                            [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]);
                    break;}
                case 3:{
                    if(trainPlayer->currentVertex()->idx() == trainPlayer->edge()->vertex1().idx()){
                        trainPlayer->setCurrentVertex(&trainPlayer->edge()->vertex2());
                    }
                    else{
                        trainPlayer->setCurrentVertex(&trainPlayer->edge()->vertex1());
                    }
                    trainPlayer->setEdge(trainPlayer->waysStorage()
                            [map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                            [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]);
                    break;}
                case 4:{
                    if(trainPlayer->currentVertex()->idx() == trainPlayer->edge()->vertex1().idx()){
                        trainPlayer->setCurrentVertex(&trainPlayer->edge()->vertex2());
                    }
                    else{
                        trainPlayer->setCurrentVertex(&trainPlayer->edge()->vertex1());
                    }
                    trainPlayer->setEdge(trainPlayer->waysAll()
                            [map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                            [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]);
                    break;}
                }
            }
            for(auto train: this->player().trains()){//Здесь ивейдим столкновения поездов
                 if(train->idx() != trainPlayer->idx()){
                             if(trainPlayer->edge()
                                     == train->edge()){
                                 if((trainPlayer->edge()->vertex1().idx() == trainPlayer->currentVertex()->idx()
                                         && trainPlayer->currentVertex()->idx() < trainPlayer->edge()->vertex2().idx()
                                         && train->speed() == -1)
                                   ||(trainPlayer->edge()->vertex1().idx() == trainPlayer->currentVertex()->idx()
                                        && trainPlayer->currentVertex()->idx() > trainPlayer->edge()->vertex2().idx()
                                        && train->speed() == 1)
                                   ||(trainPlayer->edge()->vertex2().idx() == trainPlayer->currentVertex()->idx()
                                      && trainPlayer->currentVertex()->idx() < trainPlayer->edge()->vertex1().idx()
                                      && train->speed() == -1)
                                   ||(trainPlayer->edge()->vertex2().idx() == trainPlayer->currentVertex()->idx()
                                            && trainPlayer->currentVertex()->idx() > trainPlayer->edge()->vertex1().idx()
                                            && train->speed() == 1)){
                                 for(auto& edge: trainPlayer->currentVertex()->edges()){
                                     if(edge.get().vertex1().idx() == trainPlayer->currentVertex()->idx()){
                                         if(trainPlayer->waysAll()[this->map()->graph().idx().at(edge.get().vertex2().idx())]
                                                 [this->map()->graph().idx().at(trainPlayer->finalVertex()->idx())] != nullptr){
                                             trainPlayer->setEdge(trainPlayer->waysAll()[this->map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                                     [this->map()->graph().idx().at(edge.get().vertex2().idx())]);
                                         }

                                     }
                                     else{
                                         if(trainPlayer->waysAll()[this->map()->graph().idx().at(edge.get().vertex1().idx())]
                                                 [this->map()->graph().idx().at(trainPlayer->finalVertex()->idx())] != nullptr){
                                             trainPlayer->setEdge(trainPlayer->waysAll()[this->map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                                     [this->map()->graph().idx().at(edge.get().vertex1().idx())] );
                                         }

                                     }
                                 }
                                 }
                             }
                     }
             }
        }
        else{//если поезд едет с любой скоростью != 0
                for(auto train: this->player().trains()){//Здесь ивейдим столкновения поездов
                     if(train->idx() != trainPlayer->idx()){
                                 if(trainPlayer->edge()
                                         == train->edge()){
                                 }
                     }

                }

        }
    }

}

void Game::upgradeStrategy(Train* trainPlayer, std::vector<Town*> upgradeTowns, std::vector<Train*> upgradeTrains){
    if(trainPlayer->currentVertex()->idx() == this->player().town().vertex().idx()){
        if(this->player().town().armor() >= trainPlayer->nextLevelPrice() && trainPlayer->level() != 3){
            upgradeTrains.push_back(trainPlayer);
            this->upgradeAction(upgradeTowns, upgradeTrains);
            upgradeTrains.pop_back();
            return;
        }
    }
    if(this->player().town().level() != 3){
        if(this->player().town().armor() >= this->player().town().nextLevelPrice()){
            upgradeTowns.push_back(&this->player().town());
            this->upgradeAction(upgradeTowns, upgradeTrains);
            upgradeTowns.pop_back();
            return;
        }
    }
}

void Game::printMap(enum PostType type) {
    switch (type) {
    case PostType::TOWN :
        for (auto &town : this->map()->towns()) {
            qDebug() << town.name()
                     << "Population:" << town.population()
                     << "Product:" << town.product()
                     << "Armor:" << town.armor();
        }

        break;
    case PostType::MARKET :
        for (auto &market : this->map()->markets()) {
            qDebug() << market.name()
                     << "Product:" << market.product();
        }

        break;
    case PostType::STORAGE :
        for (auto &storage : this->map()->storages()) {
            qDebug() << storage.name()
                     << "Armor:" << storage.armor();
        }

        break;
    }
}

void Game::printPlayerData(Train *train, Town *town) {
    qDebug() << "CLIENT: Product:" << town->product()
             << "Population:" << town->population()
             << "Armor:" << town->armor()
             << "Train:" << train->lineIdx()
             << train->position()
             << train->speed()
             << train->goods();
}

