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

void Game::getMap() {
    socket_->sendData(Request(Action::MAP, QJsonObject({{"layer", 0}})));
     layer_0 = socket_->getData();

    socket_->sendData(Request(Action::MAP, QJsonObject({{"layer", 1}})));
     layer_1 = socket_->getData();

    socket_->sendData(Request(Action::MAP, QJsonObject({{"layer", 10}})));
     layer_2 = socket_->getData();
}

void Game::drawMap() {
    qDebug() << "Draw map";
    map_ = std::make_unique<Map>(layer_0, layer_1, layer_2, *player_);
}

void Game::connectToGame() {

}


