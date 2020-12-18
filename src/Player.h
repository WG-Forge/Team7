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

    void setTown(QJsonObject town);
    void setTown2(Town &town) { town_ = &town; };
    void setTrain(QJsonObject train);

    QString name() { return name_; };
    QString idx() { return idx_; };
    int rating() { return rating_; };
    bool inGame() { return inGame_; };
    Town& town() { return *town_; };
    std::vector<Train>& trains() { return trains_; };

signals:

private:
    QString name_;
    QString idx_;
    int rating_;
    bool inGame_;
    Town *town_;
    std::vector<Train> trains_;
};

#endif // PLAYER_H
