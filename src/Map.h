#ifndef MAP_H
#define MAP_H

#include "Post.h"
#include "Posts/Market.h"
#include "Posts/Storage.h"
#include "Posts/Town.h"
#include "Train.h"
#include "Graph.h"
#include "Player.h"

class Map {
    public:
        explicit Map(const QJsonObject &staticObj, const QJsonObject &dynamicObj, const QJsonObject &pointsCoords, Player& player);

        Graph& graph();

        std::vector<Town>& towns();
        std::vector<Market>& markets();
        std::vector<Storage>& storages();
        std::vector<Post>& posts();
        Post& userPost() { return userPost_; };

        std::vector<Train>& trains();
        void makeWays();

    private:
        std::vector<Market> markets_;
        std::vector<Storage> storages_;
        std::vector<Town> towns_;
        std::vector<Train> trains_;
        std::vector<Post> posts_;
        int idx_;
        Graph graph_;
        Post userPost_;
};

#endif // MAP_H
