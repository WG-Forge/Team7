#ifndef GAME_H
#define GAME_H

#include "Graph.h"
#include "Socket.h"
#include "Map.h"
#include "Player.h"
#include "Enums/WaysType.h"

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
        void updatePosts();
        void updateUser();
        void upgrade(Town *town, std::vector<Train *> trains);

        Vertex getGoods(Train *train, PostType type, Vertex currentVertex);
//        Vertex sendTrain(Train *train, Vertex begin, Vertex end, enum PostType type);
        void moveAction(Train *train, Edge edge, int speed);
        int moveTrain(const int start, const int end, enum PostType type, Train *train);
        void sendTrain(Train *train, enum WaysType type);

        Vertex& findPostVertex(PostType type, Vertex currentVertex, Train *train);
        Edge& getLine(Train *train, Vertex start, Vertex end, enum WaysType wayType);
        int findEdge(int start, int end, int speed);
        int getIdx(int position);
        int returnToHome(int currentPosition);
        void unloadTrain(Train *train);

        void printPlayerData(Train *train, Town *town);
        void printPosts();
        void printMap(enum PostType type);

        Socket& socket() { return *socket_; };
        std::shared_ptr<Map> map() { return map_; };
        Player& player() { return *player_; };
        std::vector<Player>& enemies() { return enemies_; };
        Edge& currentMoveLine() { return *currentMoveLine_; };

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
        bool connected_ = false;
        Edge *currentMoveLine_;
};

#endif // GAME_H
