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
    qDebug() << response;

    player_ = new Player(response);
    player_->setPassword(password);

    this->setEnemiesCount(players - 1);
    this->setGameName(gameName);
    this->setTotalTicks(ticks);
    this->setCurrentTick(0);
    player_->setTicks(0, this->totalTicks());

    this->getMap();
    this->makeMap();
    this->player().setInGame(true);
    emit playerChanged(player_, false);

    emit mapChanged(std::make_shared<Map>(*map_), player_, true);
    emit showMap();

    qDebug() << this->isGameStarted(gameName, players);
    while (!this->isGameStarted(gameName, players)) {
        qDebug() << "Waiting for players..";
        continue;
    }

    this->gameCycle();
}

void Game::gameCycle() {
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
        if(i != 0){
            i += 6;
        }
        else{
            ++i;
        }
    }

    emit mapChanged(std::make_shared<Map>(*map_), player_, true);
    int tickCount = 0;

    std::vector<Town*> upgradeTowns;
    std::vector<Train*> upgradeTrains;

    while(connected_) {
        for(auto &train : this->player().trains()){
                if(train->waitingTime() == 0){
                    this->wayStrategy(train);
                    this->printPlayerData(train, &this->player().town());
                    this->sendTrain(train);
                }
                if(train->waitingTime() != 0){
                train->setWaitingTime(train->waitingTime() - 1);
                }
                this->upgradeStrategy(train, upgradeTowns, upgradeTrains);
            }

        tickCount++;
        this->tick();
        this->updateUser();
        this->updatePosts();
        this->setCurrentTick(this->currentTick() + 1);
        player_->setTicks(this->currentTick(), this->totalTicks());
        for (auto &train : player_->trains()) {
            qDebug() << "GDF;JGSDS" << train->idx();
        }
        emit mapChanged(std::make_shared<Map>(*map_), player_, false);
        emit playerChanged(player_, true);

        QApplication::processEvents();
    }
}

void Game::sendTrain(Train *train) {
    qDebug() << "SEND FUNCTION" << train->speed();

    std::cout << "SEND FUNCTION PATH:" << std::endl;
    for (auto &path : train->currentPath()) {
        std::cout << path->idx() << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;

    if (train->nextVertex() == nullptr) {
        qDebug() << "АТДИХАЕМ ПАЦАНВА"
                 << "Current:" << train->currentVertex()->idx();
        return;
    }

    qDebug() << "FROM SEND FUNCTION:" << "Current:" << train->currentVertex()->idx() << "Final:" << train->finalVertex()->idx();
//    if (train->currentVertex()->idx() == train->finalVertex()->idx()) return;

   if (train->nextVertex() == nullptr) return;

   Edge *currentLine = nullptr;
   for(auto& edge: train->currentVertex()->edges()){
       qDebug() << "Line:" << edge.get().idx() << train->nextVertex()->idx();
       if((edge.get().vertex1().idx() == train->currentVertex()->idx() && edge.get().vertex2().idx() == train->nextVertex()->idx())
              || (edge.get().vertex1().idx() == train->nextVertex()->idx() && edge.get().vertex2().idx() == train->currentVertex()->idx())){
           currentLine = &edge.get();
        }
   }

   Vertex start;
   Vertex end;
   int speed = train->speed();
   train->setEdge(currentLine);

   if(train->speed() == 0){
       if (currentLine->vertex1().idx() == train->currentVertex()->idx()) {
           train->setFinalLinePosition(currentLine->length());
       } else {
           train->setFinalLinePosition(0);
       }
       if (train->currentVertex()->idx() < train->nextVertex()->idx()) {
           speed = 1;
       } else speed = -1;
       this->moveAction(train, currentLine, speed);
   }

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

std::vector<Vertex*> Game::shortestWay(Train *train, Vertex &start, Vertex &goal) {
    qDebug() << "TRAIN:" << train->idx() << train->lineIdx() << train->position() << start.idx() << goal.idx();
    int minVertex = this->map()->graph().minVertexIdx();
    int newCost = 0;
    double priority = 0;

    PriorityQueue<Vertex *, double> frontier2;
    frontier2.put(&start, 0);

    std::unordered_map<int, double> cost_so_far;
    std::unordered_map<int, Vertex *> came_from;
    std::vector<Vertex *> neighbors;
    Vertex *current;

    cost_so_far.insert(std::pair<int, double>(start.idx(), 0));
    came_from.insert(std::pair<int, Vertex*>(start.idx(), &start));

    while (!frontier2.empty()) {
        current = frontier2.get();

        if (current->idx() == goal.idx()) {
            break;
        }

        for (auto next : current->edges()) {
            if (current->idx() == next.get().vertex1().idx()) neighbors.emplace_back(&next.get().vertex2());
            else neighbors.emplace_back(&next.get().vertex1());
        }

        for (auto &next : neighbors) {
            double new_cost = cost_so_far[current->idx()] + this->map()->graph().lengthBetween(current->idx(), next->idx());

            if (!cost_so_far.count(next->idx()) || new_cost < cost_so_far[next->idx()]) {
                cost_so_far[next->idx()] = new_cost;
                double priority = new_cost + heuristic(next, &goal);
                frontier2.put(next, priority);
                came_from[next->idx()] = current;
            }
        }
        neighbors.clear();
    }


    std::vector<Vertex *> path; // ВОТ ТУТ ПУТЬ, МОЖНО ЕГО ВЕРНУТЬ ТАМ ИЛИ ЧЁ-ТО ЕЩЁ

    current = &goal;
    path.emplace_back(current);
    int tt = goal.idx();

    while(current->idx() != start.idx()) {
        current = came_from[current->idx()];
        path.emplace_back(current);
    }
//    path.emplace_back(&start);

    std::reverse(path.begin(), path.end());
    std::cout << "PATH: ";
    for (auto &c : path) {
        std::cout << c->idx() << " -> ";
    }
    std::cout << "\n";
    std::cout << std::endl;

   train->setCurrentPath(path);
   // this->map()->graph().restoreMatrix(); // ТУТ ВОССТАНАВЛИВАЕТСЯ МАТРИЦА, НО МОЖНО ЭТОГО И НЕ ДЕЛАТЬ
    return path;
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
        if (train->idx() == 1) {
            if (train->nextVertex() != nullptr) {
                qDebug() << "TRAIN:" << train->idx()
                         << "LINE:" << train->lineIdx() << train->edge()->idx()
                         << "POSITION:" << train->position()
                         << "CURRENT VERTEX:" << train->currentVertex()->idx()
                         << "NEXT VERTEX" << train->nextVertex()->idx();
            } else {
                qDebug() << "TRAIN:" << train->idx()
                         << "LINE:" << train->lineIdx()
                         << "POSITION:" << train->position()
                         << "CURRENT VERTEX:" << train->currentVertex()->idx()
                         << "NEXT VERTEX: NULL";
            }
        }
    }
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
    if(trainPlayer->cooldown() != 0){
        trainPlayer->setCurrentVertex(&this->player().town().vertex());
        trainPlayer->setNextVertex(nullptr);
        trainPlayer->setFinalVertex(nullptr);
        return;
    }
    if(trainPlayer->nextVertex() == nullptr){//поезд стоит, не может ехать
        if(trainPlayer->currentVertex() == trainPlayer->finalVertex()){//Поезд достиг точки назначения
            qDebug() << "Поезд достиг точки назначения";
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
                    this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                    trainPlayer->setCurrentIndex(1);
                    trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                    trainPlayer->setWaysType(static_cast<WaysType>(1));
                    //this->avoidTrains(trainPlayer);
                    break;}
                case 3:{//Поезд в стораже, одевается в доспехи наверно
                    trainPlayer->setCurrentVertex(trainPlayer->finalVertex());
                    trainPlayer->setFinalVertex(&player().town().vertex());
                    this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                    trainPlayer->setCurrentIndex(1);
                    trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                    trainPlayer->setWaysType(static_cast<WaysType>(1));
                    //this->avoidTrains(trainPlayer);
                    break;}
                }

            }
            else{//Мы не на посту, чё то соображаем полагаю

            }
        }
        else{//Поезд НЕ достиг точки назначения, WHY или в городе нашем
            if(trainPlayer->finalVertex() == nullptr){//Тут логика отправления поезда
                if(static_cast<int>(this->player().trains()[0]->waysType()) == 2){
                    if(this->productProblem()){
                        trainPlayer->setCurrentVertex(&this->player().town().vertex());
                        trainPlayer->setFinalVertex(&findPostVertex(PostType::MARKET, this->player().town().vertex(), trainPlayer));
                        this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                        trainPlayer->setCurrentIndex(1);
                        trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                        trainPlayer->setWaysType(static_cast<WaysType>(2));
                    }
                    else{
                        trainPlayer->setCurrentVertex(&this->player().town().vertex());
                        trainPlayer->setFinalVertex(&findPostVertex(PostType::STORAGE, this->player().town().vertex(), trainPlayer));
                        this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                        trainPlayer->setCurrentIndex(1);
                        trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                        trainPlayer->setWaysType(static_cast<WaysType>(3));
                    }
                }
                else{
                    if(this->player().town().product() <= this->player().town().productCapacity()){
                        trainPlayer->setCurrentVertex(&this->player().town().vertex());
                        trainPlayer->setFinalVertex(&findPostVertex(PostType::MARKET, this->player().town().vertex(), trainPlayer));
                        this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                        trainPlayer->setCurrentIndex(1);
                        trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                        trainPlayer->setWaysType(static_cast<WaysType>(2));
                    }
                }
               // this->avoidTrains(trainPlayer);
            }
            else{

            }
        }
    }
    else{//Поезд едет
        if(trainPlayer->speed() == 0){
            if(trainPlayer->nextVertex()->idx() == trainPlayer->finalVertex()->idx()){
                if(trainPlayer->finalVertex()->idx() == this->player().town().vertex().idx()){
                    trainPlayer->setFinalVertex(nullptr);
                    trainPlayer->setCurrentVertex(&this->player().town().vertex());
                    trainPlayer->setWaysType(WaysType::ANYTING);
                }
                else{
                    trainPlayer->setCurrentVertex(trainPlayer->finalVertex());
                }
                trainPlayer->setNextVertex(nullptr);
                wayStrategy(trainPlayer);
            }
            else{
                trainPlayer->setCurrentVertex(trainPlayer->nextVertex());
                trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex() + 1]);
                trainPlayer->setCurrentIndex(trainPlayer->currentIndex() + 1);
            }

        }
        else{//если поезд едет с любой скоростью != 0


        }
        //this->avoidTrains(trainPlayer);
    }

}

void Game::avoidTrains(Train* trainPlayer){
    if(trainPlayer->speed() == 0){
    for(auto train: this->player().trains()){//Здесь ивейдим столкновения поездов
         if(train->idx() != trainPlayer->idx()){
                     if(trainPlayer->nextVertex()
                             == train->currentVertex()){
                            int pidaras = map()->graph().lengthBetween(trainPlayer->currentVertex()->idx(), trainPlayer->nextVertex()->idx());
                            map()->graph().setNewLength(trainPlayer->currentVertex()->idx(), trainPlayer->nextVertex()->idx(), 1000000);
                            this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                            trainPlayer->setCurrentIndex(2);
                            map()->graph().setNewLength(trainPlayer->currentVertex()->idx(), trainPlayer->nextVertex()->idx(), pidaras);
                            trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                     }

    }
    }
    }
    else{
        /*for(auto train: this->player().trains()){//Здесь ивейдим столкновения поездов
             if(train->idx() != trainPlayer->idx()){
                         if(trainPlayer->edge()
                                 == train->edge()){
                             if((train->speed() == 1 && trainPlayer->speed() == -1) || (train->speed() == -1 && trainPlayer->speed() == 1)){
                                 this->moveAction(trainPlayer, trainPlayer->edge(), -trainPlayer->speed());
                                 trainPlayer->setSpeed(-trainPlayer->speed());
                                 return;
                             }
                         }
             }
        }*/
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
bool Game::productProblem(){
    int sum = 0;
    int productProeb = 0;
    for(auto& train: this->player().trains()){
        if(static_cast<int>(train->waysType()) == 2){
            sum += train->goodsCapacity();
            productProeb += (train->waysLengthMarket()[this->map()->graph().idx().at(train->currentVertex()->idx())]
                    [this->map()->graph().idx().at(train->finalVertex()->idx())]
                    + train->waysLengthMarket()[this->map()->graph().idx().at(train->finalVertex()->idx())]
                    [this->map()->graph().idx().at(this->player().town().vertex().idx())]) * this->player().town().population();
        }
    }
    int nehvatka = this->player().town().productCapacity() - this->player().town().product() - productProeb;
    if(nehvatka >= sum){
        return true;
    }
    return false;
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

