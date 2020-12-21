#include "Player.h"

Player::Player(QObject *parent) : QObject(parent)
{

}

Player::Player(const QJsonObject &playerData) {
    idx_ = playerData["idx"].toString();
    name_ = playerData["name"].toString();
    rating_ = playerData["rating"].toInt();
    inGame_ = playerData["in_game"].toBool();
}

//void Player::setTown(QJsonObject town) {
//    town_ = new Town(town);
//}

//void Player::setTrain(QJsonObject train) {
//    trains_.emplace_back(train);
//}
