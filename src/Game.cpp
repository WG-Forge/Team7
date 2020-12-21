#include "Game.h"
#include "Enums/ActionEnum.h"
#include <QThread>

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

    this->socket_->close();
}

void Game::disconnect() {
    qDebug() << "Connection closed";

    this->socket_->close();
}

void Game::tick() {
    this->socket_->sendData(Request(Action::TURN, QJsonObject()));
    socket_->getData();

//    socket_->sendData(Request(Action::PLAYER, QJsonObject()));
//    QJsonObject response = socket_->getData();
//    qDebug() << response["trains"].toArray()[0].toObject()["line_idx"].toInt()
//            << response["trains"].toArray()[0].toObject()["position"].toInt()
//            << response["trains"].toArray()[0].toObject()["speed"].toInt();

//    this->player().trains()[0]->move(1, response["trains"].toArray()[0].toObject()["line_idx"].toInt(),
//            response["trains"].toArray()[0].toObject()["speed"].toInt(), response["trains"].toArray()[0].toObject()["position"].toInt());
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

void Game::gameCycle() {
    qDebug() << "cycle";

    int shortestIdx = 0,
        shortestPos = 0,
        currentIdx = 0,
        currentPos = 0;
    QString shortestName = "";

    Market *currentMarket = new Market();
    const int USER_POST_IDX = this->player().town().pointIdx();
    const int USER_POST_POS = this->map()->graph().idx().at(USER_POST_IDX);
    currentIdx = USER_POST_IDX;
    currentPos = USER_POST_POS;

    for (auto &market : this->map()->markets()) {
        qDebug() << market.name() << market.product();
    }

    while(true) {
        if (this->player_->town().product() < 400) {

            shortestPos = this->findPost(PostType::MARKET, currentPos);
            for (auto &market : this->map()->markets()) {
                if (this->getIdx(shortestPos) == market.pointIdx()) {
                    currentMarket = &market;
                }
            }

            if (shortestPos == -1) {
                qDebug() << "There is no goods any more..." <<
                            "Total product amount: " << this->player().town().product() <<
                            "Total population amount: " << this->player().town().population();
            }

            currentPos = this->moveTrain(currentPos, shortestPos);

            int currentCapacity = this->player().trains()[0]->goodsCapacity() - this->player().trains()[0]->goods();
            this->player().trains()[0]->changeGoodsAmount(currentMarket->takeProduct(currentCapacity));

            socket_->sendData(Request(Action::PLAYER, QJsonObject()));
            QJsonObject response = socket_->getData();
            qDebug() << response;

            currentPos = this->moveTrain(currentPos, USER_POST_POS);

            socket_->sendData(Request(Action::PLAYER, QJsonObject()));
            response = socket_->getData();
            qDebug() << response;

            this->unloadTrain(this->player().trains()[0]);
            qDebug() << "Current town products amount: " << response["town"].toObject()["product"].toInt();
            qDebug() << "Откуда взял: " << currentMarket->name() << currentMarket->product();
            // можно сделать так, чтобы после посещения магазина, если не полностью забит, поехать в другой магазин и заполнить до фулла
            // в findEdge проверять будет ли некст точка - тем типом поста, который нам нужен
            // если линия уже занята поездом или ведёт не к тому типу поста - обновлять все trains.ways, но слишком затратно...
            break;
        } else {
            break;
        }
    }

    socket_->sendData(Request(Action::PLAYER, QJsonObject()));
    qDebug() << socket_->getData()["trains"].toArray()[0];
//    QThread::sleep(12);
}

int Game::moveTrain(const int startPos, const int endPos) {
    int currentPos = startPos;
    int currentIdx = this->getIdx(currentPos);
    int toPos = this->player().trains()[0]->ways()[startPos][endPos];
    int toIdx = this->getIdx(toPos);
    int speed = 0;

    while (true) {
        (currentIdx > toIdx)? speed = -1 : speed = 1;
        qDebug() << "From: " << currentIdx << "To: " << toIdx;
        currentPos = this->findEdge(currentPos, toPos, speed);

        if (currentPos == endPos) return currentPos;
        if (currentPos == -1) {
            qDebug() << "Всё?";
            return currentPos;
        }

        toPos =  this->player().trains()[0]->ways()[currentPos][endPos];
        toIdx = this->getIdx(toPos);
        currentIdx = this->getIdx(currentPos);
    }

   return -1;
}

void Game::moveAction(int trainIdx, Edge &edge, int speed, Train *train, int startPosition) {
    QJsonObject request;
    request["line_idx"] = edge.idx();
    request["speed"] = speed;
    request["train_idx"] = trainIdx;

    socket_->sendData(Request(Action::MOVE, request));
    socket_->getData();

    int deadEnd = 0;
    (speed > 0)? deadEnd = edge.length() : deadEnd = 0;
    train->setPosition(startPosition);

    while (train->position() != deadEnd) {
        this->tick();
        train->move(1, edge.idx(), speed, train->position() + speed);
    }
}

int Game::findEdge(int startPos, int endPos, int speed) {
    for (auto &vertex : this->map()->graph().vertices()) {
        if (this->map()->graph().idx().at(vertex.idx()) == startPos) {
            for (auto &edge : vertex.edges()) {
                if (endPos == this->map()->graph().idx().at(edge.get().vertex1().idx()) ||
                    endPos == this->map()->graph().idx().at(edge.get().vertex2().idx()))
                {
                    int startPosition = 0;
                    qDebug() << edge.get().idx();
                    if (startPos == this->map()->graph().idx().at(edge.get().vertex2().idx())) {
                        startPosition = edge.get().length();
                    }
                    this->moveAction(this->player().trains()[0]->idx(), edge, speed, this->player().trains()[0], startPosition);

                    return endPos;
                }
            }
        }
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

int Game::returnToHome(int currentPosition) {
    qDebug() << "Go home";

    int shortestIdx = 0,
        shortestPos = 0,
        currentIdx = 0,
        currentPos = 0,
        wayLength = 1000000,
        toPos = 0,
        toIdx = 0;
    QString shortestName = "";

    Market *currentMarket = new Market();
    const int USER_POST_IDX = this->player().town().pointIdx();
    const int USER_POST_POS = this->map()->graph().idx().at(USER_POST_IDX);
    currentIdx = USER_POST_IDX;
    currentPos = USER_POST_POS;

    return 0;
}

int Game::findPost(PostType type, int currentPos) {
    bool isGoods = false;
    int shortestPos = 0;
    int wayLength = 10000;

    switch(type) {
    case PostType::TOWN:
        for (auto &town : this->map()->towns()) {
            int marketPos = this->map()->graph().idx().at(town.pointIdx());
            int currentLength = this->map()->trains()[0].waysLength()[currentPos][marketPos];

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
            int currentLength = this->map()->trains()[0].waysLength()[currentPos][marketPos];

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
            int currentLength = this->map()->trains()[0].waysLength()[currentPos][marketPos];

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

