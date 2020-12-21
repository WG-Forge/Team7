#ifndef GAME_H
#define GAME_H

#include "Graph.h"
#include "Socket.h"
#include "Map.h"
#include "Player.h"

#include <QDebug>

class Game : public QObject {
    Q_OBJECT

    public:
        explicit Game(QObject *parent = nullptr);

        void out();
        void start();
        void end();
        void gameCycle();
        void login(QString userName);
        void logout();
        void connectToServer();
        void connectToGame();
        void getMap();
        void makeMap();
        void tick();

        int moveTrain(const int start, const int end, enum PostType type, Train *train);
        int moveAction(Edge *edge, int speed, Train *train, int startPosition);
        int findPostPos(PostType type, int currentPos, Train *train);

        int findEdge(int start, int end, int speed);
        int getIdx(int position);
        int returnToHome(int currentPosition);
        void unloadTrain(Train *train);

        void userInfo();

        Socket& socket() { return *socket_; };
        std::shared_ptr<Map> map() { return map_; };
        Player& player() { return *player_; };
        std::vector<Player>& enemies() { return enemies_; };

    public slots:
        void init(const QString &username);
        void disconnect();

    signals:
        void playerChanged(Player player);
        void mapChanged(std::shared_ptr<Map> map, Player player);

    private:
        Socket *socket_ = nullptr;
        std::shared_ptr<Map> map_ = nullptr;
        QJsonObject layer_0, layer_1, layer_2;
        Player *player_ = nullptr;
        std::vector<Player> enemies_;
};

#endif // GAME_H
