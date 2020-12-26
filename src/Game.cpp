#include "Game.h"
#include "Enums/ActionEnum.h"
#include <QThread>
#include <QApplication>

Game::Game(QObject *parent) : QObject(parent)
{

}

void Game::out() {
    qDebug() << "hi";
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

void Game::disconnect() {
    qDebug() << "Connection closed";
    this->player().setInGame(false);
    connected_ = false;

    this->socket_->close();
}

void Game::tick() {
    this->socket_->sendData(Request(Action::TURN, QJsonObject()));
    QJsonObject response = socket_->getData();
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

void Game::connectToGame() {
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

    for (auto &train : this->player().trains()) {
        train->setCurrentVertex(&this->player().town().vertex());
    }

    emit mapChanged(std::make_shared<Map>(*map_), *player_, true);
    int tickCount = 0;
    //this->shortestWay(this->player().trains()[0], this->player().town().vertex(), this->map()->graph().vertices()[5]);
    //return;
    while(connected_) {
        //break;
//        emit infoChange(*player_); // замутить обнову ui
        emit mapChanged(std::make_shared<Map>(*map_), *player_, false);

        for(auto &train : this->player().trains()){
            if (train->idx() != this->player().trains()[0]->idx()) continue;

            if (train->edge() != nullptr) {
                qDebug() << "Not noll:";
                qDebug() << "Current:" << train->currentVertex()->idx()
                         << "Next: " << train->nextVertex()->idx();
            } else qDebug() << "Null:" << tickCount;

            this->strategy(train);
            this->sendTrain(train);
        }

        tickCount++;
        this->tick();
        this->updateUser();
        this->updatePosts();
//        this->printPlayerData(this->player().trains()[0], &this->player().town());

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

void Game::shortestWay(Train *train, Vertex start, Vertex goal) {
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

    while (!frontier2.empty()) {
        current = frontier2.get();
        if (current->idx() == goal.idx()) break;

        for (auto next : current->edges()) {
//            qDebug() << next.get().idx();
            if (current->idx() == next.get().vertex1().idx()) neighbors.emplace_back(&next.get().vertex2());
            else neighbors.emplace_back(&next.get().vertex1());
        }

        for (auto &next : neighbors) {
            double new_cost = cost_so_far[current]
                    + this->map()->graph().matrix()[current->idx() - minVertex][next->idx() - minVertex];
//            qDebug() << next->idx() << new_cost;
            if (!cost_so_far.count(next) || new_cost < cost_so_far[next]) {
                cost_so_far[next] = new_cost;
                double priority = new_cost + heuristic(next, &goal);
                frontier2.put(next, priority);
                came_from[next] = current;
            }
        }
        neighbors.clear();
    }

    for (auto &from : came_from) {
        qDebug() << from.first->idx();
    }

    ////////////////////////////////////////////////////////// УОТ ТУТА НИХУЯ НЕ РАБОТАЕТ
    ///
    qDebug() << came_from[&goal];  // не выводит нужную хуйню почомутчо походу адрес меняется или чё или хуй пойми что-то с указателями или ссылками ебал я их в рот

    std::vector<Vertex *> path; //  НУ И ТОТ ТОЖЕ ПОТОМУ ЧТО ДО НЕ ПОЛУЧАЕТСЯ ПОЛУЧИТЬ CAME_FROM[VERTEX]
    current = &goal;
    path.emplace_back(current);

    while(current != &start) {
        current = came_from[current];
        path.emplace_back(current);
    }
    path.emplace_back(&start);

    std::reverse(path.begin(), path.end());
    qDebug() << "Goal:" << current->idx();
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

void Game::init(const QString &username) {
    connectToServer();
    login(username);
    getMap();
    makeMap();
    this->player().setInGame(true);

    emit playerChanged(*player_);

    connected_ = true;
    gameCycle();
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

void Game::strategy(Train* trainPlayer){
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
                    trainPlayer->setWaysType(static_cast<WaysType>(4));
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
                    trainPlayer->setWaysType(static_cast<WaysType>(4));
                    break;}
                }

            }
            else{//Мы не на посту, чё то соображаем полагаю

            }
        }
        else{//Поезд НЕ достиг точки назначения, WHY или в городе нашем
            if(trainPlayer->finalVertex() == nullptr){//Тут логика отправления поезда
                if(this->player().town().product() <= this->player().town().productCapacity()){
                    trainPlayer->setCurrentVertex(&this->player().town().vertex());
                    trainPlayer->setFinalVertex(&findPostVertex(PostType::MARKET, this->player().town().vertex(), trainPlayer));
                    trainPlayer->setEdge(trainPlayer->waysMarket()[map()->graph().idx().at(this->player().town().vertex().idx())]
                            [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]);
                    trainPlayer->setWaysType(static_cast<WaysType>(2));
                }
            }
        }
    }
    else{//Поезд едет
        for(auto train: this->map()->trains()){//Здесь ивейдим другие поезда
            if(train.idx() != trainPlayer->idx()){
                if(trainPlayer->speed() == 0){
                    if(map()->graph().edges()[map()->graph().idxEdges().at(train.lineIdx())].length() - train.position() <=
                            trainPlayer->waysLengthAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                            [map()->graph().idx().at(trainPlayer->nextVertex()->idx())]){

                    }
                }
                else{
                    if(trainPlayer->speed() == 1){

                    }
                    else{

                    }
                }
            }
        }
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
                this->strategy(trainPlayer);
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
        }
        else{//если поезд едет с любой скоростью != 0

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

