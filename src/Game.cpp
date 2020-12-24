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

    while(connected_) {
        emit mapChanged(std::make_shared<Map>(*map_), *player_);

        for(auto &train : this->player().trains()){
//            Train *train = this->player().trains()[i];
            train->setCurrentVertex(&this->player().town().vertex());
            this->strategy(train);
            this->sendTrain(train, WaysType::MARKET);
        }

//        this->tick();
//        this->updateUser();
//        this->updatePosts();
        QApplication::processEvents();
    }
}
// устанавливает current / next / final
void Game::sendTrain(Train *train, enum WaysType wayType) {
    Edge currentLine;
    Vertex start;
    Vertex end;
    int speed = 0;

    qDebug() << "Send train:" << train->currentVertex()->idx() << train->finalVertex()->idx();
    currentLine = this->getLine(train, *train->currentVertex(), *train->finalVertex(), wayType);
    qDebug() << "Line" << currentLine.idx() << currentLine.vertex1().idx() << currentLine.vertex2().idx();

    if (train->speed() != 0) {
        qDebug() << "Edem FROM:" << train->currentVertex()->idx() << "TO:" << train->nextVertex()->idx();
    }

    if (currentLine.vertex1().idx() == train->currentVertex()->idx()) {
        train->setNextVertex(&currentLine.vertex2());
        train->setFinalLinePosition(currentLine.length());
    } else {
        train->setNextVertex(&currentLine.vertex1());
        train->setFinalLinePosition(0);
    }

    if (train->currentVertex()->idx() < train->nextVertex()->idx()) {
        speed = 1;
    } else speed = -1;

    this->moveAction(train, currentLine, speed);
}

void Game::moveAction(Train *train, Edge moveLine, int speed) {
    QJsonObject request;
    request["line_idx"] = moveLine.idx();
    request["speed"] = speed;
    request["train_idx"] = train->idx();
    qDebug() << request;

    socket_->sendData(Request(Action::MOVE, request));
    socket_->getData();
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

enum WaysType Game::strategy(Train* trainPlayer){
    if(trainPlayer->nextVertex() == nullptr){//поезд стоит
        if(trainPlayer->currentVertex() == trainPlayer->finalVertex()){//Поезд достиг точки назначения
            if(trainPlayer->finalVertex()->isPostIdxNull() != false){//Мы на каком то посту
                switch (static_cast<int>(trainPlayer->finalVertex()->post().type())) {
                case 1:{//Поезд в городе
                    if(trainPlayer->finalVertex()->postIdx() != player().town().idx()){//Поезд не дома, в городе

                    }
                    else{//Поезд дома, чисто чилит пока что

                    }
                    break;}
                case 2:{//Поезд в маркете, забрал продукты, мама будет довольна
                    trainPlayer->setFinalVertex(&player().town().vertex());
                    Edge* edge = trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                            [map()->graph().idx().at(trainPlayer->finalVertex()->idx())];
                    if(edge->vertex1().idx() == trainPlayer->currentVertex()->idx()){ trainPlayer->setNextVertex(&trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]->vertex2());}
                            else{trainPlayer->setNextVertex(&trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]->vertex1());}
                    return static_cast<WaysType>(4);
                    break;}
                case 3:{//Поезд в стораже, одевается в доспехи наверно
                    trainPlayer->setFinalVertex(&player().town().vertex());
                    Edge* edge = trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                            [map()->graph().idx().at(trainPlayer->finalVertex()->idx())];
                    if(edge->vertex1().idx() == trainPlayer->currentVertex()->idx()){ trainPlayer->setNextVertex(&trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]->vertex2());}
                            else{trainPlayer->setNextVertex(&trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]->vertex1());}
                    return static_cast<WaysType>(4);
                    break;}
                }

            }
            else{//Мы не на посту, чё то соображаем полагаю

            }
        }
        else{//Поезд НЕ достиг точки назначения, WHY или в городе нашем
            if(trainPlayer->finalVertex() == nullptr){
                if(this->player().town().product() <= this->player().town().productCapacity()){
                    trainPlayer->setCurrentVertex(&this->player().town().vertex());
                    trainPlayer->setFinalVertex(&findPostVertex(PostType::MARKET, this->player().town().vertex(), trainPlayer));
                    trainPlayer->setNextVertex(&trainPlayer->waysMarket()[this->player().town().vertex().idx()][trainPlayer->finalVertex()->idx()]->vertex2());
                    return static_cast<WaysType>(2);
                }
            }
        }
    }
    else{//Поезд едет
        /*for(auto train: this->map()->trains()){
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
        }*/
        trainPlayer->setCurrentVertex(trainPlayer->nextVertex());
        trainPlayer->setNextVertex(&trainPlayer->waysMarket()[map()->graph().idx().at(trainPlayer->nextVertex()->idx())]
                [map()->graph().idx().at(trainPlayer->finalVertex()->idx())]->vertex2());
        return static_cast<WaysType>(2);

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

