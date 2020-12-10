#ifndef GAME_H
#define GAME_H

#include "Graph.h"
#include "Socket.h"
#include "Map.h"

#include <QDebug>

class Game : public QObject
{
public:
    explicit Game(QObject *parent = nullptr);

    void out();
    void start();
    void end();
    void login();
    void logout();
    void connectToServer();
    void connectToGame();
    void disconnect();
    void getMap();
    void drawMap();

    Socket& socket() { return *socket_; };
    Map& map() { return *map_; };

signals:

private:
    Socket *socket_ = nullptr;
    Map *map_ = nullptr;
    QJsonObject layer_0, layer_1, layer_2;
};

#endif // GAME_H
