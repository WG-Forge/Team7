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

    this->userInfo();

//    qDebug() << "Tick:" << this->player().town().product();
    this->player().town().addProduct(this->player().town().population() * (-1));
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
    map_->makeWays();
}

void Game::connectToGame() {

}

void Game::userInfo() {
    socket_->sendData(Request(Action::PLAYER, QJsonObject()));
    QJsonObject response = socket_->getData();

    if (this->player().town().population() != response["town"].toObject()["population"].toInt()) {
        this->player().town().setPopulation(response["town"].toObject()["population"].toInt());
    }
    if (this->player().town().product() != response["town"].toObject()["population"].toInt()) {
        this->player().town().setGoods(response["town"].toObject()["product"].toInt());
    }

//    qDebug() << "Product:" << response["town"].toObject()["product"].toInt()
//            << "Population:" << response["town"].toObject()["population"].toInt()
//            << "Train: " << response["trains"].toArray()[0].toObject()["line_idx"].toInt()
//            << response["trains"].toArray()[0].toObject()["position"].toInt()
//            << response["trains"].toArray()[0].toObject()["speed"].toInt()
//            << response["trains"].toArray()[0].toObject()["goods"].toInt();
}

void Game::gameCycle() {
    qDebug() << "cycle";

    int shortestIdx = 0,
        shortestPos = 0,
        currentIdx = 0,
        currentPos = 0;
    QString shortestName = "";
    QJsonObject response;

    const int USER_POST_IDX = this->player().town().pointIdx();
    const int USER_POST_POS = this->map()->graph().idx().at(USER_POST_IDX);
    currentIdx = USER_POST_IDX;
    currentPos = USER_POST_POS;

    Market *currentMarket = new Market();
    Train *train = this->player().trains()[0];

    while(connected_) {
        emit mapChanged(std::make_shared<Map>(*map_), *player_);

        if (this->player_->town().product() < 300) {
            shortestPos = this->findPostPos(PostType::MARKET, currentPos, train); // OK

            if (shortestPos == -1) {
                qDebug() << "There is no goods any more..." <<
                            "Total product amount: " << this->player().town().product() <<
                            "Total population amount: " << this->player().town().population();
            }

            for (auto &market : this->map()->markets()) {
                if (this->getIdx(shortestPos) == market.pointIdx()) {
                    currentMarket = &market;
                }
            }

            currentPos = this->moveTrain(currentPos, shortestPos, currentMarket->type(), train);

            int currentCapacity = train->goodsCapacity() - train->goods();
            train->changeGoodsAmount(currentMarket->takeProduct(currentCapacity));

            this->userInfo();

            currentPos = this->moveTrain(currentPos, USER_POST_POS, currentMarket->type(), train);
            this->unloadTrain(train);

            this->userInfo();
            qDebug() << "Products::" << this->player().town().product();
        } else {
            break;
        }

        QApplication::processEvents();
    }
//    QThread::sleep(12);
}

int Game::moveTrain(const int startPos, const int endPos, enum PostType type, Train *train) {
    int currentPos = startPos;
    int currentIdx = this->getIdx(currentPos);
    int toPos = 0;
    int toIdx = 0;
    int speed = 0;
    int pathPos = 0;
    Edge *toLine;

    switch(type) {
    case PostType::MARKET:
        toLine = train->waysMarket()[startPos][endPos];
        break;
    case PostType::STORAGE:
        toLine  = train->waysStorage()[startPos][endPos];
        break;
    case PostType::TOWN:
        toLine  = train->waysMarket()[startPos][endPos];
        break;
    }

    if (toLine->vertex1().idx() == currentIdx) {
        toIdx = toLine->vertex2().idx();
        toPos = this->map()->graph().idx().at(toIdx);
        pathPos = 0;
    } else {
        toIdx = toLine->vertex1().idx();
        toPos = this->map()->graph().idx().at(toIdx);
        pathPos = toLine->length();
    }

    while (true) {

        (currentIdx > toIdx)? speed = -1 : speed = 1;
//        qDebug() << "From: " << currentIdx << "(" + QString::number(currentPos) + ")"
//                 << "To: " << toIdx << "(" + QString::number(toPos) + ")"
//                 << "Speed: " << speed;

        currentPos = this->moveAction(toLine, speed, train, pathPos);
        currentPos = toPos;
        currentIdx = this->getIdx(currentPos);

        if (currentPos == endPos) return currentPos;
        if (currentPos == -1) {
            qDebug() << "Всё?";
            return currentPos;
        }

        switch(type) {
        case PostType::MARKET:
            toLine = train->waysMarket()[currentPos][endPos];
            break;
        case PostType::STORAGE:
            toLine  = train->waysStorage()[currentPos][endPos];
            break;
        case PostType::TOWN:
            toLine  = train->waysMarket()[currentPos][endPos];
            break;
        }

        if (toLine->vertex1().idx() == currentIdx) {
            toIdx = toLine->vertex2().idx();
            toPos = this->map()->graph().idx().at(toIdx);
            pathPos = 0;
        } else {
            toIdx = toLine->vertex1().idx();
            toPos = this->map()->graph().idx().at(toIdx);
            pathPos = toLine->length();
        }
    }

   return -1;
}

int Game::moveAction(Edge *edge, int speed, Train *train, int startPosition) {
    QJsonObject request;
    request["line_idx"] = edge->idx();
    request["speed"] = speed;
    request["train_idx"] = train->idx();

    socket_->sendData(Request(Action::MOVE, request));
    socket_->getData();

    int deadEnd = 0;
    train->setPosition(startPosition);
    (startPosition == 0)? deadEnd = edge->length() : deadEnd = 0;

    train->movePreparation(edge->idx(), speed, startPosition);

    while (train->position() != deadEnd) {
        this->tick();
        train->setPosition(train->position() + speed);

//        qDebug() << "Local:"
//                << train->lineIdx()
//                << train->position()
//                << train->speed();
    }

    return 0;
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

int Game::findPostPos(PostType type, int currentPos, Train *train) {
    bool isGoods = false;
    int shortestPos = 0;
    int wayLength = 1000000;

    switch(type) {
    case PostType::TOWN:
        for (auto &town : this->map()->towns()) {
            int marketPos = this->map()->graph().idx().at(town.pointIdx());
            int currentLength = train->waysLengthStorage()[currentPos][marketPos];

            if (currentLength < wayLength) {
                wayLength = currentLength;
                shortestPos = marketPos;
            }
        }

        break;

    case PostType::MARKET:
        for (auto &market : this->map()->markets()) {
            if (market.product() == 0) continue;
            isGoods = true;
            int marketPos = this->map()->graph().idx().at(market.pointIdx());
            int currentLength = train->waysLengthMarket()[currentPos][marketPos];

            if (currentLength < wayLength) {
                wayLength = currentLength;
                shortestPos = marketPos;
            }
        }
        if (!isGoods) {
            return -1;
        }

        break;

    case PostType::STORAGE:
        for (auto &storage : this->map()->storages()) {
            if (storage.armor() == 0) continue;
            isGoods = true;
            int marketPos = this->map()->graph().idx().at(storage.pointIdx());
            int currentLength = train->waysLengthStorage()[currentPos][marketPos];

            if (currentLength < wayLength) {
                wayLength = currentLength;
                shortestPos = marketPos;
            }
        }
        if (!isGoods) {
            return -1;
        }

        break;
    }

    return shortestPos;
}

void Game::unloadTrain(Train *train) {
    this->player().town().addProduct(train->goods());
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

