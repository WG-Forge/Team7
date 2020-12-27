#ifndef PLAYER_H
#define PLAYER_H

#include "Post.h"
#include "Train.h"
#include "Posts/Town.h"

#include <QObject>

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = nullptr);
    explicit Player(const QJsonObject &playerData);
    Player(const Player &player);

    void setTown(Town &town) { town_ = &town; };
    void setTrain(Train &train) { trains_.emplace_back(&train); };
    void setInGame(bool inGame) { inGame_ = inGame; };
    void setPassword(const QString &pass) { password_ = pass; };
    void setTicks(int current, int max) { currentTick_ = current; maxTicks_ = max; };

    QString name() { return name_; };
    QString idx() { return idx_; };
    QString password() { return password_; };
    int rating() { return rating_; };
    int maxTicks() { return maxTicks_; };
    int currentTick() { return currentTick_; };
    bool inGame() { return inGame_; };
    Town& town() { return *town_; };
    std::vector<Train *> &trains() { return trains_; };

    void update(const QJsonObject &playerData);
signals:

private:
    QString name_;
    QString idx_;
    int rating_;
    QString password_;
    bool inGame_;
    Town *town_;
    std::vector<Train *> trains_;
    int maxTicks_;
    int currentTick_;
};

#endif // PLAYER_H
