#ifndef MAP_H
#define MAP_H

#include "Market.h"
#include "Storage.h"
#include "Town.h"
#include "Train.h"

class Map
{
public:
    explicit Map() = default;
    explicit Map(const QJsonObject &map);

private:
    std::vector<Market> markets_;
    std::vector<Storage> storages_;
    std::vector<Town> towns_;
    std::vector<Train> trains_;
    int idx_;
};

#endif // MAP_H
