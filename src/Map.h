#ifndef MAP_H
#define MAP_H

#include "Post.h"
#include "Posts/Market.h"
#include "Posts/Storage.h"
#include "Posts/Town.h"
#include "Train.h"

class Map
{
public:
    explicit Map() = default;
    explicit Map(const QJsonObject &map);
    std::vector<Town>& towns();
    std::vector<Market>& markets();
    std::vector<Storage>& storages();
    std::vector<Post>& posts();

private:
    std::vector<Market> markets_;
    std::vector<Storage> storages_;
    std::vector<Town> towns_;
    std::vector<Train> trains_;
    std::vector<Post> posts_;
    int idx_;
};

#endif // MAP_H
