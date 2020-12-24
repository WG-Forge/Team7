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

void Game::upgrade(Town *town, std::vector<Train *> trains) {
    QJsonObject request;
    QJsonArray postsIndexes = {town->idx()};
    QJsonArray trainIndexes = {};

    for (auto &train : trains) {
        trainIndexes.append(train->idx());
    }
    request["posts"] = postsIndexes;
    request["trains"] = trainIndexes;
    qDebug() << request;

    this->socket_->sendData(Request(Action::UPGRADE, request));
    qDebug() << socket_->getData();
}

void Game::tick() {
    this->socket_->sendData(Request(Action::TURN, QJsonObject()));
    QJsonObject response = socket_->getData();
//    qDebug() << "Tick:" << response;
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

    Train *train = this->player().trains()[0];
    train->setFinalVertex(&this->findPostVertex(PostType::MARKET, train->currentVertex(), train));

    while(connected_) {
        emit mapChanged(std::make_shared<Map>(*map_), *player_);

//        this->getGoods(train, PostType::MARKET, this->player().town().vertex());

//        this->upgrade(&this->player().town(), this->player().trains());
//        break;
//        if (train->finalVertex() == nullptr) {
//            qDebug() << train->finalPosition();
//        }
//        for (auto &train : this->player().trains()) {

//            if (train->speed() == 0) {
//                qDebug() << "Train is stopped";

//                if (&train->currentVertex() != &train->finalVertex()) {
//                    qDebug() << "Vertex not equal"; // не доехал до конечной точки
//                    this->

//                    if (train->position() == train->finalLinePosition()) {
//                        qDebug() << "Line position is equal"; // доехал до конца текущей линии

//                    } else {
//                        qDebug() << "Line position is not equal"; // где-то на линии

//                    }
//                } else {
//                    qDebug() << "Vertex is equal"; // приехал в конечную точку

//                }
//            }
//            qDebug() << "Train is moving"
//                     << train->lineIdx()
//                     << train->currentVertex().idx()
//                     << train->nextVertex().idx()
//                     << train->speed();
//            break;
//        }

        this->sendTrain(train, WaysType::MARKET);
//        qDebug() << train->idx();

//        break;
        this->tick();
        this->updateUser();
        this->updatePosts();

        QApplication::processEvents();
    }
}

void Game::sendTrain(Train *train, enum WaysType wayType) {
    Edge currentLine;
    Vertex start;
    Vertex end;
    int speed = 0;

    qDebug() << "Send train:" << train->currentVertex().idx() << train->finalVertex().idx();
    currentLine = this->getLine(train, train->currentVertex(), train->finalVertex(), wayType);
    qDebug() << "Line" << currentLine.idx() << currentLine.vertex1().idx() << currentLine.vertex2().idx();

    if (train->speed() != 0) {
        qDebug() << "Edem FROM:" << train->currentVertex().idx() << "TO:" << train->nextVertex().idx();
    }

    if (currentLine.vertex1().idx() == train->currentVertex().idx()) {
        train->setNextVertex(&currentLine.vertex2());
        train->setFinalLinePosition(currentLine.length());
    } else {
        train->setNextVertex(&currentLine.vertex1());
        train->setFinalLinePosition(0);
    }

    if (train->currentVertex().idx() < train->nextVertex().idx()) {
        speed = 1;
    } else speed = -1;

    this->moveAction(train, currentLine, speed);
}

//Vertex Game::getGoods(Train *train, PostType postType, Vertex startVertex) {
//    Vertex currentVertex = startVertex;
//    Vertex endVertex;
//    Vertex homeVertex = this->player().town().vertex();
//    Market *currentMarket;
//    Storage *currentStorage;

//    switch(postType) {
//    case PostType::STORAGE:
//        qDebug() << "Get armor";
//        endVertex = this->findPostVertex(PostType::STORAGE, currentVertex, train);
//        for (auto &storage : this->map()->storages()) {
//            if (storage.vertex().idx() == endVertex.idx()) currentStorage = &storage;
//        }

//        if (currentStorage->armor() == 0) {
//            qDebug() << "No more ARMOR";
//        }

//        qDebug() << "Path from: " << currentVertex.idx()
//                 << "To market: " << endVertex.idx()
//                 << "GOINT TO:" << currentStorage->name()
//                 << "REST: " << currentStorage->armor();

//        break;
//    case PostType::MARKET:
//        qDebug() << "Get products";
//        endVertex = this->findPostVertex(PostType::MARKET, currentVertex, train);
//        for (auto &market : this->map()->markets()) {
//            if (market.vertex().idx() == endVertex.idx()) currentMarket = &market;
//        }

//        if (currentMarket->product() == 0) {
//            qDebug() << "No more PRODUCTS";
//        }

//        qDebug() << "Path from: " << currentVertex.idx()
//                 << "To market: " << endVertex.idx()
//                 << "GOINT TO:" << currentMarket->name()
//                 << "REST: " << currentMarket->product();

//        break;
//    case PostType::TOWN:
//        qDebug() << "Get null";
//        return endVertex;
//        break;
//    }

//    // поехать в пост
//    currentVertex = this->sendTrain(train, currentVertex, endVertex, postType);
//    // забрать ресы (не уверен что это здесь нужно, т.к. потом всё равно обновляется карта)
//    switch(postType) {
//    case PostType::STORAGE:
//        train->changeGoodsAmount(currentStorage->takeArmor(train->goods()));
//        qDebug() << "TAKEN FROM:" << currentStorage->name() << "REST: " << currentStorage->armor();
//        break;
//    case PostType::MARKET:
//        train->changeGoodsAmount(currentMarket->takeProduct(train->goods()));
//        qDebug() << "TAKEN FROM:" << currentMarket->name() << "REST: " << currentMarket->product();
//        break;
//    }

//    printPlayerData(train, &this->player().town()); // принт клиентской инфы
//    this->updateUser(); // инфа о игроке с сервера + обновление, если не совпадает (обновляет не все)
//    printPlayerData(train, &this->player().town()); // принт клиентской инфы

//    //    this->printMap(postType); // принт изменённых постов
//        this->updatePosts(); // тут есть принт новой инфы с сервера (там же и обновляется карта)
//    //    this->printMap(postType); // принт изменённых постов

//    // из поста домой
//    qDebug() << "Path from: " << currentVertex.idx() << "to: " << homeVertex.idx();
//    currentVertex = this->sendTrain(train, currentVertex, homeVertex, postType);
//    // отдать ресы домой (не уверен что это здесь нужно, т.к. потом всё равно обновляется карта)
//    switch(postType) {
//    case PostType::STORAGE:
//        this->player().town().addGoods(train->goods(), GoodsType::armor);
//        this->updateUser();
//        break;
//    case PostType::MARKET:
//        this->player().town().addGoods(train->goods(), GoodsType::product);
//        this->updateUser();
//        break;
//    }
//    this->unloadTrain(train);

//    printPlayerData(train, &this->player().town()); // принт клиентской инфы
//    this->updateUser(); // инфа о игроке с сервера + обновление, если не совпадает (обновляет не все)
//    printPlayerData(train, &this->player().town()); // принт клиентской инфы

////    this->printMap(postType); // принт изменённых постов
//    this->updatePosts(); // тут есть принт новой инфы с сервера (там же и обновляется карта, но обновляет не всё)
////    this->printMap(postType); // принт изменённых постов

//    return currentVertex;
//}

//Vertex Game::sendTrain(Train *train, Vertex start, Vertex end, enum PostType postType) {
//    Vertex currentStart = start;
//    Vertex currentEnd = end;
//    Edge currentLine = this->getLine(train, currentStart, currentEnd, postType);

//    if (currentStart.idx() == currentLine.vertex1().idx()) {
//        currentEnd = currentLine.vertex2();
//    } else currentEnd = currentLine.vertex1();

//    int speed = 0;
//    int position = 0;
//    int direction = 0;

//    while (currentStart.idx() != end.idx()) {
//        if (currentStart.idx() < currentEnd.idx()) {
//            speed = 1;
//        } else speed = -1;

//        if (currentStart.idx() == currentLine.vertex1().idx()) {
//            position = 0;
//        } else position = currentLine.length();
//        qDebug() << "Line: " << currentLine.idx()
//                 << "Pos: " << position
//                 << "From: " << currentStart.idx()
//                 << "To: " << currentEnd.idx();

//        this->moveAction(train, currentLine, speed, position);
//        currentStart = currentEnd;

//        if (currentStart.idx() == end.idx()) {
//            break;
//        }

//        currentLine = this->getLine(train, currentStart, end, postType);
//        if (currentStart.idx() == currentLine.vertex1().idx()) {
//            currentEnd = currentLine.vertex2();
//        } else currentEnd = currentLine.vertex1();
//    }

//    return currentStart;
//}

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
//    Vertex currentStart = start;
//    Vertex currentEnd = end;
    Edge *moveEdge = new Edge();

    switch(wayType) {
    case WaysType::RETURN:
//        for (auto &town : this->map()->towns()) {
//            marketPos = this->map()->graph().idx().at(town.pointIdx());
//            currentLength = train->waysLengthStorage()[currentPos][marketPos];

//            if (currentLength < wayLength) {
//                wayLength = currentLength;
//                shortestPos = marketPos;
//                *postVertex = town.vertex();
//            }
//        }
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


//    switch(postType) {
//    case PostType::MARKET:
//        edge = train->waysMarket()[this->map()->graph().idx().at(currentStart.idx())][this->map()->graph().idx().at(currentEnd.idx())];
//        break;
//    case PostType::STORAGE:
//        edge = train->waysStorage()[this->map()->graph().idx().at(currentStart.idx())][this->map()->graph().idx().at(currentEnd.idx())];
//        break;
//    case PostType::TOWN:
//        break;
//    }

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

