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

Player::Player(const Player &player) {
    rating_ = player.rating_;
    town_ = new Town(*player.town_);
    name_ = player.name_;
    idx_ = player.idx_;
    inGame_ = player.inGame_;

    for (auto &train : player.trains_)
        trains_.push_back(new Train(*train));
}

//void Player::setTown(QJsonObject town) {
//    town_ = new Town(town);
//}

//void Player::setTrain(QJsonObject train) {
//    trains_.emplace_back(train);
//}
