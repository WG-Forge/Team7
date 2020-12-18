#ifndef TRAIN_H
#define TRAIN_H
#include "Event.h"
#include "Enums/GoodsType.h"
#include "Events/HijackersAssault.h"
#include "Events/RefugeesArrival.h"
#include "Events/TrainCollision.h"
#include "Events/ParasitesAssault.h"
#include "Vertex.h"

#include <QJsonObject>
#include <QJsonArray>
#include <stdexcept>

class Train{
public:
    explicit Train() = default;
    explicit Train(const QJsonObject &train);

    int cooldown();
    void trainWays(const std::vector<std::vector<int>>& mas, const std::vector<std::vector<int>>& p);
    std::vector<Event*>& events();
    int fuel();
    int fuelCapacity();
    int fuelConsumption();
    int goods();
    int goodsCapacity();
    enum GoodsType goodsType();
    int idx();
    int level();
    int lineIdx();
    int nextLevelPrice();
    QString playerIdx();
    int position();
    std::vector<std::vector<int>> ways() { return ways_; };
    std::vector<std::vector<int>> waysLength() { return waysLenght_; };
    int speed();

private:
    int cooldown_;
    std::vector<Event*> events_;
    int fuel_;
    int fuelCapacity_;
    int fuelConsumption_;
    int goods_;
    int goodsCapacity_;
    enum GoodsType goodsType_;
    int idx_;
    int level_;
    int lineIdx_;
    int nextLevelPrice_;
    QString playerIdx_;
    int position_;
    int speed_;
    std::vector<std::vector<int>> waysLenght_;
    std::vector<std::vector<int>> ways_;

};

#endif // TRAIN_H
