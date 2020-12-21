#ifndef GAME_H
#define GAME_H

#include "Graph.h"
#include "Socket.h"
#include "Map.h"
#include "Player.h"

#include <QDebug>

class Game : public QObject
{
public:
    explicit Game(QObject *parent = nullptr);

    void out();
    void start();
    void end();
    void login(QString userName);
    void logout();
    void connectToServer();
    void connectToGame();
    void disconnect();
    void getMap();
    void makeMap();
    void gameCycle();
    void tick();
    int moveTrain(const int start, const int end);
    void moveAction(int trainIdx, Edge &edge, int speed, Train *train, int startPosition);
    int findEdge(int start, int end, int speed);
    int getIdx(int position);
    int returnToHome(int currentPosition);
    int findPost(PostType type, int currentPos);
    void unloadTrain(Train *train);

    Socket& socket() { return *socket_; };
    std::shared_ptr<Map> map() { return map_; };
    Player& player() { return *player_; };
    std::vector<Player>& enemies() { return enemies_; };


signals:

private:
    Socket *socket_ = nullptr;
    std::shared_ptr<Map> map_ = nullptr;
    QJsonObject layer_0, layer_1, layer_2;
    Player *player_ = nullptr;
    std::vector<Player> enemies_;
};

#endif // GAME_H
