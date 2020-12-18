#include "Game.h"
#include "Enums/ActionEnum.h"

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


void Game::login(QString userName) {
    qDebug() << "LogIn";

    QJsonObject data;
    data["name"] = userName;
    socket_->sendData(Request(Action::LOGIN, data));
    QJsonObject userData = socket_->getData();

    player_ = new Player(userData);
}


void Game::logout() {
    qDebug() << "LogOut";

    socket_->Close();
}

void Game::connectToServer() {
    qDebug() << "Connecting";

    socket_ = new Socket();
    socket_->Connect();
}

void Game::disconnect() {
    qDebug() << "Connection closed";

    socket_->Close();
}

void Game::tick() {
    this->player().town().addProduct(this->player_->town().population() * (-1));

    socket_->sendData(Request(Action::TURN, QJsonObject()));
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

    int wayLength = 1000000;
    int shortestIdx = 0;
    const int USER_POST_IDX = this->player().town().pointIdx();
    const int USER_POST_POS = this->map()->graph().idx().at(USER_POST_IDX);
    QString shortestName = "";

    while(true) {
        if (this->player_->town().product() < 400) {
            for (auto &market : this->map()->markets()) {
                int marketPos = this->map()->graph().idx().at(market.pointIdx());
                int currentLength = this->map()->trains()[0].waysLength()[USER_POST_POS][marketPos];

                if (currentLength < wayLength) {
                    wayLength = currentLength;
                    shortestIdx = market.pointIdx();
                    shortestName = market.name();
                }
            }

            int currentPathLength = 0;

            qDebug() << shortestName << shortestIdx;
            qDebug() << this->player().town().product();

            for (auto &vertex : this->map()->graph().vertices()) {
                if (vertex.idx() == shortestIdx) {
                    for (auto &edge : vertex.edges()) {
                        qDebug() << edge.get().vertex1().idx() << edge.get().vertex2().idx();
                    }
                }
            }

            this->player().town().addProduct(40);
            this->tick();
        } else {
            break;
        }
    }
}


