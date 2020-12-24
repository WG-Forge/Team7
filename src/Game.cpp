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
    socket_->getData();

    this->player().town().comsumeProduct(this->player().town().population());
//    this->userInfo();
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



    while(connected_) {
        emit mapChanged(std::make_shared<Map>(*map_), *player_);
    for(int i = 0; i < player().trains().size(); ++i){

        Train *train = this->player().trains()[i];


    }
        QApplication::processEvents();
    }
}

Vertex Game::getGoods(Train *train, PostType postType, Vertex startVertex) {
    Vertex currentVertex = startVertex;
    Vertex endVertex;
    Vertex homeVertex = this->player().town().vertex();
    Market *currentMarket;
    Storage *currentStorage;

    switch(postType) {
    case PostType::STORAGE:
        qDebug() << "Get armor";
        endVertex = this->findPostVertex(PostType::STORAGE, currentVertex, train);
        for (auto &storage : this->map()->storages()) {
            if (storage.vertex().idx() == endVertex.idx()) currentStorage = &storage;
        }

        if (currentStorage->armor() == 0) {
            qDebug() << "No more ARMOR";
        }

        qDebug() << "Path from: " << currentVertex.idx()
                 << "To market: " << endVertex.idx()
                 << "GOINT TO:" << currentStorage->name()
                 << "REST: " << currentStorage->armor();

        break;
    case PostType::MARKET:
        qDebug() << "Get products";
        endVertex = this->findPostVertex(PostType::MARKET, currentVertex, train);
        for (auto &market : this->map()->markets()) {
            if (market.vertex().idx() == endVertex.idx()) currentMarket = &market;
        }

        if (currentMarket->product() == 0) {
            qDebug() << "No more PRODUCTS";
        }

        qDebug() << "Path from: " << currentVertex.idx()
                 << "To market: " << endVertex.idx()
                 << "GOINT TO:" << currentMarket->name()
                 << "REST: " << currentMarket->product();

        break;
    case PostType::TOWN:
        qDebug() << "Get null";
        return endVertex;
        break;
    }

    // поехать в пост
    currentVertex = this->sendTrain(train, currentVertex, endVertex, postType);
    // забрать ресы (не уверен что это здесь нужно, т.к. потом всё равно обновляется карта)
    switch(postType) {
    case PostType::STORAGE:
        train->changeGoodsAmount(currentStorage->takeArmor(train->goods()));
        qDebug() << "TAKEN FROM:" << currentStorage->name() << "REST: " << currentStorage->armor();
        break;
    case PostType::MARKET:
        train->changeGoodsAmount(currentMarket->takeProduct(train->goods()));
        qDebug() << "TAKEN FROM:" << currentMarket->name() << "REST: " << currentMarket->product();
        break;
    }

    printPlayerData(train, &this->player().town()); // принт клиентской инфы
    this->userInfo(); // инфа о игроке с сервера + обновление, если не совпадает (обновляет не все)
    printPlayerData(train, &this->player().town()); // принт клиентской инфы

    //    this->printMap(postType); // принт изменённых постов
        this->updatePosts(); // тут есть принт новой инфы с сервера (там же и обновляется карта)
    //    this->printMap(postType); // принт изменённых постов

    // из поста домой
    qDebug() << "Path from: " << currentVertex.idx() << "to: " << homeVertex.idx();
    currentVertex = this->sendTrain(train, currentVertex, homeVertex, postType);
    // отдать ресы домой (не уверен что это здесь нужно, т.к. потом всё равно обновляется карта)
    switch(postType) {
    case PostType::STORAGE:
        this->player().town().addGoods(train->goods(), GoodsType::armor);
        this->updateInfo();
        break;
    case PostType::MARKET:
        this->player().town().addGoods(train->goods(), GoodsType::product);
        this->updateInfo();
        break;
    }
    this->unloadTrain(train);

    printPlayerData(train, &this->player().town()); // принт клиентской инфы
    this->userInfo(); // инфа о игроке с сервера + обновление, если не совпадает (обновляет не все)
    printPlayerData(train, &this->player().town()); // принт клиентской инфы

//    this->printMap(postType); // принт изменённых постов
    this->updatePosts(); // тут есть принт новой инфы с сервера (там же и обновляется карта, но обновляет не всё)
//    this->printMap(postType); // принт изменённых постов

    return currentVertex;
}

Vertex Game::sendTrain(Train *train, Vertex start, Vertex end, enum PostType postType) {
    Vertex currentStart = start;
    Vertex currentEnd = end;
    Edge currentLine = this->getLine(train, currentStart, currentEnd, postType);

    if (currentStart.idx() == currentLine.vertex1().idx()) {
        currentEnd = currentLine.vertex2();
    } else currentEnd = currentLine.vertex1();

    int speed = 0;
    int position = 0;
    int direction = 0;

    while (currentStart.idx() != end.idx()) {
        if (currentStart.idx() < currentEnd.idx()) {
            speed = 1;
        } else speed = -1;

        if (currentStart.idx() == currentLine.vertex1().idx()) {
            position = 0;
        } else position = currentLine.length();
        qDebug() << "Line: " << currentLine.idx()
                 << "Pos: " << position
                 << "From: " << currentStart.idx()
                 << "To: " << currentEnd.idx();

        this->moveAction(train, currentLine, speed, position);
        currentStart = currentEnd;

        if (currentStart.idx() == end.idx()) {
            break;
        }

        currentLine = this->getLine(train, currentStart, end, postType);
        if (currentStart.idx() == currentLine.vertex1().idx()) {
            currentEnd = currentLine.vertex2();
        } else currentEnd = currentLine.vertex1();
    }

    return currentStart;
}

void Game::moveAction(Train *train, Edge moveLine, int speed, int position) {
    QJsonObject request;
    request["line_idx"] = moveLine.idx();
    request["speed"] = speed;
    request["train_idx"] = train->idx();
//    qDebug() << request;

    socket_->sendData(Request(Action::MOVE, request));
    socket_->getData();

    train->move(moveLine.idx(), speed, position);
    if (position == 0) {
        while (train->position() != moveLine.length()) {
            if (train->position() + 1 == moveLine.length()) speed = 0;

            this->tick();
            train->move(moveLine.idx(), speed, train->position() + 1);

//            printPlayerData(train, &this->player().town());
//            this->userInfo();
        }
    } else {
        while (train->position() != 0) {
            if (train->position() - 1 == 0) speed = 0;

            this->tick();
            train->move(moveLine.idx(), speed, train->position() - 1);

//            printPlayerData(train, &this->player().town());
//            this->userInfo();
        }
    }
    qDebug() << "Приехал в вертекс и обновил инфу";
    printPlayerData(train, &this->player().town());
    this->userInfo();
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

Edge& Game::getLine(Train *train, Vertex start, Vertex end, enum PostType postType) {
    Vertex currentStart = start;
    Vertex currentEnd = end;
    Edge *edge = new Edge();

    switch(postType) {
    case PostType::MARKET:
        edge = train->waysMarket()[this->map()->graph().idx().at(currentStart.idx())][this->map()->graph().idx().at(currentEnd.idx())];
        break;
    case PostType::STORAGE:
        edge = train->waysStorage()[this->map()->graph().idx().at(currentStart.idx())][this->map()->graph().idx().at(currentEnd.idx())];
        break;
    case PostType::TOWN:
        break;
    }

    return *edge;
}

void Game::updatePosts() {
    socket_->sendData(Request(Action::MAP, QJsonObject({{"layer", 1}})));
     layer_1 = socket_->getData();

//     qDebug() << layer_1;
     int marketIter = 0;
     int townIter = 0;
     int storageIter = 0;

     QJsonArray posts = layer_1["posts"].toArray();

     for (auto const &post : posts) {
         int postType = post.toObject()["type"].toInt();

         switch(postType) {
         case 1:
             this->map()->towns()[townIter].update(post.toObject());
//             qDebug() << "Server:" << post.toObject()["name"].toString()
//                      << "Population:" << post.toObject()["population"].toInt()
//                      << "Product:" << post.toObject()["product"].toInt()
//                      << "Armor:" << post.toObject()["armor"].toInt();
             townIter++;
             break;
         case 2:
             this->map()->markets()[marketIter].update(post.toObject());
//             qDebug() << "Server:" << post.toObject()["name"].toString()
//                      << "Product:" << post.toObject()["product"].toInt();
             marketIter++;
             break;
         case 3:
             this->map()->storages()[storageIter].update(post.toObject());
//             qDebug() << "Server:" << post.toObject()["name"].toString()
//                      << "Armor:" << post.toObject()["armor"].toInt();
             storageIter++;
             break;
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

void Game::userInfo() {
    socket_->sendData(Request(Action::PLAYER, QJsonObject()));
    QJsonObject response = socket_->getData();

    if (this->player().town().population() != response["town"].toObject()["population"].toInt()) {
        this->player().town().setPopulation(response["town"].toObject()["population"].toInt());
    }
    if (this->player().town().product() != response["town"].toObject()["population"].toInt()) {
        this->player().town().setGoods(response["town"].toObject()["product"].toInt(), GoodsType::product);
    }
    if (this->player().town().armor() != response["town"].toObject()["armor"].toInt()) {
        this->player().town().setGoods(response["town"].toObject()["armor"].toInt(), GoodsType::armor);
    }
    int trainIndex = 0;
    for (auto &train : this->player().trains()) {
        if (train->goods() != response["trains"].toArray()[trainIndex].toObject()["goods"].toInt()) {
            train->changeGoodsAmount(response["trains"].toArray()[trainIndex].toObject()["goods"].toInt());
        }

        trainIndex++;
    }

    qDebug() << "SERVER: Product:" << response["town"].toObject()["product"].toInt()
             << "Population:" << response["town"].toObject()["population"].toInt()
             << "Armor:" << response["town"].toObject()["armor"].toInt()
             << "Train:" << response["trains"].toArray()[0].toObject()["line_idx"].toInt()
             << response["trains"].toArray()[0].toObject()["position"].toInt()
             << response["trains"].toArray()[0].toObject()["speed"].toInt()
             << response["trains"].toArray()[0].toObject()["goods"].toInt();
}

void Game::updateInfo()
{
    socket_->sendData(Request(Action::PLAYER, QJsonObject()));
    QJsonObject response = socket_->getData();
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

void Game::unloadTrain(Train *train) {
//    this->player().town().addProduct(train->goods());
    train->changeGoodsAmount(train->goods() * (-1));
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

void Game::strategy(Train* trainPlayer, Vertex* finalVertex, enum WaysType waysType, Vertex* nextVertex){
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
                    finalVertex = &player().town().vertex();
                    Edge* edge = trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                            [map()->graph().idx().at(finalVertex->idx())];
                    if(edge->vertex1().idx() == trainPlayer->currentVertex()->idx()){ nextVertex = &trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                [map()->graph().idx().at(finalVertex->idx())]->vertex2();}
                            else{nextVertex = &trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                [map()->graph().idx().at(finalVertex->idx())]->vertex1();}
                    waysType = static_cast<WaysType>(4);
                    break;}
                case 3:{//Поезд в стораже, одевается в доспехи наверно
                    finalVertex = &player().town().vertex();
                    Edge* edge = trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                            [map()->graph().idx().at(finalVertex->idx())];
                    if(edge->vertex1().idx() == trainPlayer->currentVertex()->idx()){ nextVertex = &trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                [map()->graph().idx().at(finalVertex->idx())]->vertex2();}
                            else{nextVertex = &trainPlayer->waysAll()[map()->graph().idx().at(trainPlayer->currentVertex()->idx())]
                                [map()->graph().idx().at(finalVertex->idx())]->vertex1();}
                    waysType = static_cast<WaysType>(4);
                    break;}
                }

            }
            else{//Мы не на посту, чё то соображаем полагаю

            }
        }
        else{//Поезд НЕ достиг точки назначения, WHY или в городе нашем
            if(trainPlayer->finalVertex() == nullptr){
                if(this->player().town().product() < this->player().town().productCapacity()){
                    trainPlayer->setFinalVertex(&findPostVertex(PostType::MARKET, this->player().town().vertex(), trainPlayer));
                    waysType = static_cast<WaysType>(2);
                    trainPlayer->setNextVertex(&trainPlayer->waysMarket()[this->player().town().vertex().idx()][finalVertex->idx()]->vertex2());
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
        trainPlayer->setNextVertex(&trainPlayer->waysMarket()[trainPlayer->nextVertex()->idx()][trainPlayer->finalVertex()->idx()]->vertex2());


    }

}
