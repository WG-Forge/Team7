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

    for (auto userIdx : map_->playersIdx()) {
        playersRatings.emplace_back(layer_1["ratings"].toObject()[userIdx].toObject()["rating"].toInt());
        playersNames.emplace_back(layer_1["ratings"].toObject()[userIdx].toObject()["name"].toString());
    }
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
    qDebug() << "IS GAME START BEGIN?";

    for (auto game : response["games"].toArray()) {
        if (game.toObject()["name"].toString() == name) {
            if (game.toObject()["state"].toInt() == 2) {
                qDebug() << game.toObject()["name"].toString()
                         << game.toObject()["num_players"].toInt()
                         << game.toObject()["state"].toInt();
                return true;
            }
        }
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
//    qDebug() << response;

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
    emit mapChanged(std::make_shared<Map>(*map_), player_, true, &playersNames, &playersRatings);
    emit showMap();

    bool isStarted = this->isGameStarted(gameName, players);
    qDebug() << "IS:" << isStarted;

    while (!isStarted) {
        qDebug() << "Waiting for players..";
        isStarted = this->isGameStarted(gameName, players);
    }

    this->gameCycle();
}

void Game::gameCycle() {
    int currentIdx = 0,
        currentPos = 0;
    QJsonObject response;
    qDebug() << this->map()->playersIdx();

    const int USER_POST_IDX = this->player().town().pointIdx();
    const int USER_POST_POS = this->map()->graph().idx().at(USER_POST_IDX);
    currentIdx = USER_POST_IDX;
    currentPos = USER_POST_POS;
    int i = 0;
    for (auto &train : this->player().trains()) {
        train->setCurrentVertex(&this->player().town().vertex());
        train->setWaitingTime(i);
        if(i == 1){
            i += (train->goodsCapacity() + 45 - this->map()->storages()[0].armorCapacity()) / this->map()->storages()[0].replenishment();
        }
        else{
            if(i != 0){
                i += (train->goodsCapacity() + 45) / this->map()->storages()[0].replenishment();
            }
            else{
            ++i;}
        }
    }

    emit mapChanged(std::make_shared<Map>(*map_), player_, true, &playersNames, &playersRatings);
    int tickCount = 0;

    std::vector<Town*> upgradeTowns;
    std::vector<Train*> upgradeTrains;

    while(connected_) {
        for(auto &train : this->player().trains()){

                if(train->waitingTime() == 0){
                    this->wayStrategy(train);
//                    this->printPlayerData(train, &this->player().town());
                }
                if(train->speed() == 0 && train->waitingTime() == 0 && train->cooldown() == 0){
                this->upgradeStrategy(train, upgradeTowns, upgradeTrains);
                }
                if(train->waitingTime() == 0 && train->cooldown() == 0){
                    this->sendTrain(train);
                }
                if(train->waitingTime() != 0){
                train->setWaitingTime(train->waitingTime() - 1);
                }

            }

        tickCount++;
        this->tick();
        this->updateUser();
        this->updatePosts();
        this->setCurrentTick(this->currentTick() + 1);

        player_->setTicks(this->currentTick(), this->totalTicks());
        emit mapChanged(std::make_shared<Map>(*map_), player_, false, &playersNames, &playersRatings);
        emit playerChanged(player_, true);
        QApplication::processEvents();
    }
}

void Game::sendTrain(Train *train) {
    if (train->nextVertex() == nullptr) {
        qDebug() << "АТДИХАЕМ ПАЦАНВА"
                 << "Current:" << train->currentVertex()->idx();
        return;
    }

   if (train->nextVertex() == nullptr) return;

   Edge *currentLine = nullptr;
   for(auto& edge: train->currentVertex()->edges()){
//       qDebug() << "Line:" << edge.get().idx() << train->nextVertex()->idx();
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
}

void Game::moveAction(Train *train, Edge *moveLine, int speed) {
    QJsonObject request;
    request["line_idx"] = moveLine->idx();
    request["speed"] = speed;
    request["train_idx"] = train->idx();
    qDebug() << request;

    socket_->sendData(Request(Action::MOVE, request));
    QJsonObject response = socket_->getData();

//    if (response != QJsonObject()) {
//        qDebug() << "MOVEMENT ERROR" << response;
//        throw(response);
//    }
}

void Game::tick() {
    this->socket_->sendData(Request(Action::TURN, QJsonObject()));
    QJsonObject response;
    if (!socket_->waitForReadyRead()) {
        response = socket_->getData();
    }

    socket_->sendData(Request(Action::GAMES, QJsonObject()));
    if (!socket_->waitForReadyRead()) {
        response = socket_->getData();
    }

    for (auto game : response["games"].toArray()) {
        if (game.toObject()["name"].toString() == this->gameName()) {
            if (game.toObject()["state"].toInt() == 2) {
                qDebug() << " GAME IN PROGGRES";
                continue;
            } else {
                socket_->close();
                emit gameEnd(this->player().rating());
                emit disconnect();
            }
        }
    }
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

//    std::cout << "PATH: ";
//    for (auto &c : path) {
//        std::cout << c->idx() << " -> ";
//    }
//    std::cout << "\n";
//    std::cout << std::endl;

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

//        if (train->nextVertex() != nullptr) {
//            qDebug() << "TRAIN:" << train->idx()
//                     << "LINE:" << train->lineIdx() << train->edge()->idx()
//                     << "POSITION:" << train->position()
//                     << "CURRENT VERTEX:" << train->currentVertex()->idx()
//                     << "NEXT VERTEX" << train->nextVertex()->idx();
//        } else {
//            qDebug() << "TRAIN:" << train->idx()
//                     << "LINE:" << train->lineIdx()
//                     << "POSITION:" << train->position()
//                     << "CURRENT VERTEX:" << train->currentVertex()->idx()
//                     << "NEXT VERTEX: NULL";
//        }
    }
}

void Game::updatePosts() {
    socket_->sendData(Request(Action::MAP, QJsonObject({{"layer", 1}})));
    layer_1 = socket_->getData();

    int marketIter = 0;
    int townIter = 0;
    int storageIter = 0;
    int playerIter = 0;

    QJsonArray posts = layer_1["posts"].toArray();
    QJsonArray trains = layer_1["trains"].toArray();

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

    for (auto userIdx : map_->playersIdx()) {
        playersRatings[playerIter] = layer_1["ratings"].toObject()[userIdx].toObject()["rating"].toInt();
        playersNames[playerIter] = layer_1["ratings"].toObject()[userIdx].toObject()["name"].toString();
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
        int count = 0;
        for(auto& train: this->player().trains()){
            if(trainPlayer->idx() != train->idx()){
            if(train->cooldown() != 0 && train->waitingTime() == 0){
                trainPlayer->setWaitingTime(count + 2);
                count++;
            }
            }

        }
        trainPlayer->setFinalVertex(nullptr);
        trainPlayer->setCurrentVertex(&this->player().town().vertex());
        trainPlayer->setWaysType(WaysType::ANYTING);
        trainPlayer->setNextVertex(nullptr);
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
                    if(trainPlayer->goods() != trainPlayer->goodsCapacity()){
                        if(avoidTrains(trainPlayer)){
                        trainPlayer->setWaitingTime(0);
                        }
                        else{
                        trainPlayer->setWaitingTime(trainPlayer->waitingTime() + 1);
                        return;
                           }
                        }
                    trainPlayer->setCurrentVertex(trainPlayer->finalVertex());
                    trainPlayer->setFinalVertex(&player().town().vertex());
                    this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                    trainPlayer->setCurrentIndex(1);
                    trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                    //this->avoidTrains(trainPlayer);
                    break;}
                case 3:{//Поезд в стораже, одевается в доспехи наверно
                    if(trainPlayer->goods() != trainPlayer->goodsCapacity()){
                        if(avoidTrains(trainPlayer)){
                        trainPlayer->setWaitingTime(0);
                        }
                        else{
                        if(this->player().trains()[0]->level() == 3 &&
                                this->player().trains()[1]->level() == 3 &&
                                this->player().trains()[2]->level() == 3 &&
                                this->player().trains()[3]->level() == 3 &&
                                this->player().town().level() == 2){
                            if(this->player().town().armor() + 10 + this->player().town().nextLevelPrice() <= trainPlayer->goods()){
                                trainPlayer->setWaitingTime(0);
                                trainPlayer->setCurrentVertex(trainPlayer->finalVertex());
                                trainPlayer->setFinalVertex(&player().town().vertex());
                                this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                                trainPlayer->setCurrentIndex(1);
                                trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                                return;
                            }
                        }
                        else{
                        trainPlayer->setWaitingTime(trainPlayer->waitingTime() + 1);
                        return;
                           }
                        }
                    }
                    trainPlayer->setCurrentVertex(trainPlayer->finalVertex());
                    trainPlayer->setFinalVertex(&player().town().vertex());
                    this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                    trainPlayer->setCurrentIndex(1);
                    trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                    return;
                    //this->avoidTrains(trainPlayer);
                    break;
                    }
                }
            }
            else{//Мы не на посту, чё то соображаем полагаю

            }
}
        else{//Поезд НЕ достиг точки назначения, WHY или в городе нашем
            if(trainPlayer->finalVertex() == nullptr){//Тут логика отправления поезда
                    if(this->productProblem() == true){
                         int count = 0;
                         int countUp = 0;
                        for(auto& train: this->player().trains()){
                           if(train->waysType() == WaysType::MARKET){
                               count++;
                           }
                           if(train->level() == 3){
                               countUp += train->level();
                           }
                        }
                        if(this->player().town().level() == 3){
                            countUp += this->player().town().level();
                        }
                        switch(count){
                        case 0:
                            trainPlayer->setCurrentVertex(&this->player().town().vertex());
                            trainPlayer->setFinalVertex(&findPostVertex(PostType::MARKET, this->player().town().vertex(), trainPlayer));
                            this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                            trainPlayer->setCurrentIndex(1);
                            trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                            trainPlayer->setWaysType(static_cast<WaysType>(2));
                            return;
                            break;
                        case 1:
                            trainPlayer->setCurrentVertex(&this->player().town().vertex());
                            trainPlayer->setFinalVertex(&findPostVertex(PostType::MARKET, this->player().town().vertex(), trainPlayer));
                            this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                            trainPlayer->setCurrentIndex(1);
                            trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                            trainPlayer->setWaysType(static_cast<WaysType>(2));
                            return;
                            break;
                        case 2:
                            trainPlayer->setCurrentVertex(&this->player().town().vertex());
                            trainPlayer->setFinalVertex(&findPostVertex(PostType::MARKET, this->player().town().vertex(), trainPlayer));
                            this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                            trainPlayer->setCurrentIndex(1);
                            trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                            trainPlayer->setWaysType(static_cast<WaysType>(2));
                            return;
                            break;
                        case 3:{
                            if(trainPlayer->level() != 3){
                                trainPlayer->setCurrentVertex(&this->player().town().vertex());
                                trainPlayer->setFinalVertex(&findPostVertex(PostType::STORAGE, this->player().town().vertex(), trainPlayer));
                                this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                                trainPlayer->setCurrentIndex(1);
                                trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                                trainPlayer->setWaysType(static_cast<WaysType>(3));
                                return;
                            }
                            else{
                                trainPlayer->setCurrentVertex(&this->player().town().vertex());
                                trainPlayer->setFinalVertex(&findPostVertex(PostType::MARKET, this->player().town().vertex(), trainPlayer));
                                this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                                trainPlayer->setCurrentIndex(1);
                                trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                                trainPlayer->setWaysType(static_cast<WaysType>(2));
                                return;
                                }
                            break;
                        }
                        }
                        }
                    else{
                        if(trainPlayer->level() != 3){
                        trainPlayer->setCurrentVertex(&this->player().town().vertex());
                        trainPlayer->setFinalVertex(&findPostVertex(PostType::STORAGE, this->player().town().vertex(), trainPlayer));
                        this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                        trainPlayer->setCurrentIndex(1);
                        trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                        trainPlayer->setWaysType(static_cast<WaysType>(3));
                        return;}
                        else{
                            trainPlayer->setCurrentVertex(&this->player().town().vertex());
                            trainPlayer->setFinalVertex(&findPostVertex(PostType::MARKET, this->player().town().vertex(), trainPlayer));
                            this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                            trainPlayer->setCurrentIndex(1);
                            trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                            trainPlayer->setWaysType(static_cast<WaysType>(2));
                        }
                    }
                    }
        }
    }
    else{//Поезд едет
        int count = 0;
        int countUp = 0;
       for(auto& train: this->player().trains()){
          if(train->waysType() == WaysType::MARKET){
              count++;
          }
          if(train->level() == 3){
              countUp += train->level();
          }
       }
       if(this->player().town().level() == 3){
           countUp += this->player().town().level();
       }
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
                    bool wow = productProblem();
                    if((wow && trainPlayer->goods() == 0 && (count < 3 || countUp == 15) && (trainPlayer->level() != 1 || this->player().town().population() >= 3)
                            )|| (trainPlayer->level() == 3 && trainPlayer->goods() == 0)){
                        trainPlayer->setFinalVertex(&findPostVertex(PostType::MARKET, *trainPlayer->currentVertex(), trainPlayer));
                        this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                        trainPlayer->setCurrentIndex(1);
                        trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                        trainPlayer->setWaysType(static_cast<WaysType>(2));
                    }
                    if(count == 4 && trainPlayer->level() != 3 && trainPlayer->goods() == 0){
                        trainPlayer->setFinalVertex(&findPostVertex(PostType::STORAGE, *trainPlayer->currentVertex(), trainPlayer));
                        this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                        trainPlayer->setCurrentIndex(1);
                        trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                        trainPlayer->setWaysType(static_cast<WaysType>(3));
                    }
                    if(trainPlayer->goodsType() == GoodsType::product && trainPlayer->goods() != 0
                            && this->player().town().product() - trainPlayer->waysAll()[this->map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                            [this->map()->graph().idx().at(this->player().town().vertex().idx())]->length() * this->player().town().population() + trainPlayer->goods()
                            >= this->player().town().productCapacity()
                            && trainPlayer->nextVertex()->idx() == this->player().town().vertex().idx()){
                        trainPlayer->setWaitingTime(trainPlayer->waitingTime() + 1);
                        trainPlayer->setCurrentVertex(trainPlayer->currentPath()[trainPlayer->currentIndex() - 2]);
                        trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex() - 1]);
                        trainPlayer->setCurrentIndex(trainPlayer->currentIndex() - 1);
                        avoidTrains(trainPlayer);
                        return;
                    }
                    avoidTrains(trainPlayer);
                    return;
                }
//                int count = 0;
               /*for(auto& train: this->player().trains()){
                  if(train->waysType() == WaysType::MARKET){
                      count++;
                  }
               }
                if(this->productProblem() && trainPlayer->goods() == 0 && count != this->player().trains().size() - 1){
                    trainPlayer->setFinalVertex(&findPostVertex(PostType::MARKET, *trainPlayer->currentVertex(), trainPlayer));
                    this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
                    trainPlayer->setCurrentIndex(1);
                    trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
                    trainPlayer->setWaysType(WaysType::MARKET);
                }*/
            }

        else{//если поезд едет с любой скоростью != 0


        }

    }

}

bool Game::avoidTrains(Train* trainPlayer){
    if(trainPlayer->speed() == 0){
    for(auto train: this->player().trains()){//Здесь ивейдим столкновения поездов
         if(train->idx() != trainPlayer->idx()){
             if(train->nextVertex() != nullptr){
                     if(trainPlayer->nextVertex() != nullptr){
                     if(trainPlayer->nextVertex()->idx() == train->nextVertex()->idx()){
                         if(trainPlayer->currentVertex()->idx() != train->currentVertex()->idx()){
                             if(abs(train->finalLinePosition() - train->position()) >= abs(trainPlayer->finalLinePosition() - trainPlayer->position())){
                             if(train->waitingTime() == 0){
                         trainPlayer->setWaitingTime(trainPlayer->waitingTime() + 1);
                         trainPlayer->setCurrentVertex(trainPlayer->currentPath()[trainPlayer->currentIndex() - 2]);
                         trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex() - 1]);
                         trainPlayer->setCurrentIndex(trainPlayer->currentIndex() - 1);
                         return true;
                             }
                             }
                         }
                     }
                     else{
                         if(trainPlayer->currentVertex()->idx() == train->nextVertex()->idx()
                                 && trainPlayer->nextVertex()->idx() == this->player().town().vertex().idx()){
                             if(abs(train->finalLinePosition() - train->position()) > 1 && trainPlayer->goods() != 0
                                     ){
                                return false;
                             }
                             else{
                                 trainPlayer->setWaitingTime(0);
                                 trainPlayer->setCurrentVertex(trainPlayer->nextVertex());
                                 trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex() + 1]);
                                 trainPlayer->setCurrentIndex(trainPlayer->currentIndex() + 1);
                                return true;
                             }
                         }
                     }
//                     if(trainPlayer->currentVertex()->idx() == train->nextVertex()->idx()){
//                         int pidaras = map()->graph().lengthBetween(trainPlayer->currentVertex()->idx(), trainPlayer->nextVertex()->idx());
//                         map()->graph().setNewLength(trainPlayer->currentVertex()->idx(), trainPlayer->nextVertex()->idx(), 1000000);
//                         this->shortestWay(trainPlayer, *trainPlayer->currentVertex(), *trainPlayer->finalVertex());
//                         trainPlayer->setCurrentIndex(1);
//                         map()->graph().setNewLength(trainPlayer->currentVertex()->idx(), trainPlayer->nextVertex()->idx(), pidaras);
//                         trainPlayer->setNextVertex(trainPlayer->currentPath()[trainPlayer->currentIndex()]);
//                     }
}
                     else{
                         if(trainPlayer->currentVertex() != nullptr){
                         if(train->nextVertex()->idx() == trainPlayer->currentVertex()->idx()){
                             if(abs(train->finalLinePosition() - train->position()) == 1){
                                     return true;
                             }
                         }
                         }
                     }

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

    return false;
}

void Game::upgradeStrategy(Train* trainPlayer, std::vector<Town*> upgradeTowns, std::vector<Train*> upgradeTrains){

    if(trainPlayer->currentVertex()->idx() == this->player().town().vertex().idx()){
        int count = 0;
        int countUp = 0;
        for(auto& train: this->player().trains()){
                if(train->level() == 3){
                    count += train->level();
                }
                countUp += train->level();
        }
    if(trainPlayer->currentVertex()->idx() == this->player().town().vertex().idx()){
        if(this->player().town().armor() >= trainPlayer->nextLevelPrice() + 6 && trainPlayer->level() != 3){
                        upgradeTrains.push_back(trainPlayer);
                        this->upgradeAction(upgradeTowns, upgradeTrains);
                        upgradeTrains.pop_back();

        }
    }
    if(this->player().town().level() != 3 && (count > 3)){
        if(this->player().town().armor() >= this->player().town().nextLevelPrice() + 6){
            upgradeTowns.push_back(&this->player().town());
            this->upgradeAction(upgradeTowns, upgradeTrains);
            upgradeTowns.pop_back();
            return;
        }
    }
}
}
bool Game::productProblem(){
    int sum = 0;
    int productProeb = 0;
    for(auto& train: this->player().trains()){
        if(static_cast<int>(train->waysType()) == 2){
            if(train->finalVertex() != nullptr){
            sum += train->goodsCapacity();
            productProeb += (train->waysLengthMarket()[this->map()->graph().idx().at(train->currentVertex()->idx())]
                    [this->map()->graph().idx().at(train->finalVertex()->idx())]
                    + train->waysLengthMarket()[this->map()->graph().idx().at(train->finalVertex()->idx())]
                    [this->map()->graph().idx().at(this->player().town().vertex().idx())]) * (this->player().town().population());
            }
        }
    }
    int nehvatka = this->player().town().product() - productProeb + sum - 10;
    if(nehvatka <= this->player().town().productCapacity()){
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

