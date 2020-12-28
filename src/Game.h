#ifndef GAME_H
#define GAME_H

#include "Graph.h"
#include "Socket.h"
#include "Map.h"
#include "Player.h"
#include "Enums/WaysType.h"

#include <QDebug>
#include <queue>
#include <unordered_map>
#include <QVector>

template<typename T, typename priority_t>
struct PriorityQueue {
  typedef std::pair<priority_t, T> PQElement;
  std::priority_queue<PQElement, std::vector<PQElement>,
                 std::greater<PQElement>> elements;

  inline bool empty() const {
     return elements.empty();
  }

  inline void put(T item, priority_t priority) {
    elements.emplace(priority, item);
  }

  T get() {
    T best_item = elements.top().second;
    elements.pop();
    return best_item;
  }
};

struct Ratings {
    QString name;
    int rating;
};

class Game : public QObject {
    Q_OBJECT

    public:
        explicit Game(QObject *parent = nullptr);

        // A* ALGORITHM
        double heuristic(Vertex *v1, Vertex *v2);
        std::vector<Vertex*> shortestWay(Train *train, Vertex &start, Vertex &goal);

        // UNREAL STRATEGY
        void strategy(Train* trainPlayer);
        Vertex getGoods(Train *train, PostType type, Vertex currentVertex);

        void wayStrategy(Train* trainPlayer);
        void upgradeStrategy(Train* trainPlaye, std::vector<Town*> upgradeTowns, std::vector<Train*> upgradeTrainsr);


        // GAME ACTIONS
        void start();
        void end();
        void gameCycle();
        void login(QString userName);
        void logout();
        void getMap();
        void makeMap();
        void connectToServer();
        bool productProblem();

        // SOCKET ACTIONS
        void tick();
        bool isGameStarted(const QString &name, const int &players);
        void updatePosts();
        void updateUser();
        void getGamesList();
        void sendTrain(Train *train);
        void hostGame(QString name, int players, int ticks);
        void moveAction(Train *train, Edge *edge, int speed);
        void connectToGame(QString gameName, int players, int ticks);
        void upgradeAction(std::vector<Town*> towns, std::vector<Train*> trains);
        int moveTrain(const int start, const int end, enum PostType type, Train *train);
        bool avoidTrains(Train* train);

        // MOVE TRAIN

        // А ТУТ ЧОТА ДА
        int getIdx(int position);
        int returnToHome(int currentPosition);
        int findEdge(int start, int end, int speed);
        Vertex& findPostVertex(PostType type, Vertex currentVertex, Train *train);
        Edge& getLine(Train *train, Vertex start, Vertex end, enum WaysType wayType);

        // SOME RETURNS
        Socket& socket() { return *socket_; };
        Player& player() { return *player_; };
        std::shared_ptr<Map> map() { return map_; };
        std::vector<Player>& enemies() { return enemies_; };
        Edge& currentMoveLine() { return *currentMoveLine_; };

        int currentTick() { return currentTick_; };
        int enemiesAmount() { return enemiesAmount_; };
        int totalTicks() { return totalTicks_; };
        QString gameName() { return gameName_; };

        void setCurrentTick(const int tick) { currentTick_ = tick; };
        void setEnemiesCount(const int amount) { enemiesAmount_ = amount; };
        void setTotalTicks(const int total) { totalTicks_ = total; };
        void setGameName(const QString &name) { gameName_ = name; };

        // PRINTS
        void printPlayerData(Train *train, Town *town);
        void printPosts();
        void printMap(enum PostType type);

    public slots:
        void init();
        void disconnect();
        void updateGames();
        void connectToGame(const QString &userName, const QString &password, const QString &gameName, const int &players, const int &ticks);

    signals:
        void playerChanged(Player *player, bool isReady);
        void mapChanged(std::shared_ptr<Map> map, Player *player, bool ggg, std::vector<QString> *playersNames, std::vector<int> *ratings);
        void getGames(const QJsonObject &gamesData);
        void showMap();
        void gameEnd(const int);

    private:
        bool connected_ = false;
        Edge *currentMoveLine_;
        QString gameName_;
        int currentTick_;
        int totalTicks_;
        int enemiesAmount_;

        Socket *socket_ = nullptr;
        Player *player_ = nullptr;
        std::vector<Player> enemies_;
        std::shared_ptr<Map> map_ = nullptr;
        QJsonObject layer_0, layer_1, layer_2;
        std::vector<int> playersRatings;
        std::vector<QString> playersNames;
};

#endif // GAME_H
