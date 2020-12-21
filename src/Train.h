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
    void trainWays(const std::vector<std::vector<int>>& masMarket,
                   const  std::vector<std::vector<Edge*>>& pMarket,
                   const std::vector<std::vector<int>>& masStorage,
                   const  std::vector<std::vector<Edge*>>& pStorage);
    std::vector<Event*>& events();
    int fuel();
    int fuel_capacity();
    int fuel_consumption();
    int goods();
    int goods_capacity();
    enum GoodsType goods_type();
    int idx();
    int level();
    int line_idx();
    int next_level_price();
    QString player_idx();
    int position();
    int speed();

private:
    int cooldown_;
    std::vector<Event*> events_;
    int fuel_;
    int fuel_capacity_;
    int fuel_consumption_;
    int goods_;
    int goods_capacity_;
    enum GoodsType goods_type_;
    int idx_;
    int level_;
    int line_idx_;
    int next_level_price_;
    QString player_idx_;
    int position_;
    int speed_;
    std::vector<std::vector<int>> waysLengthMarket_;
    std::vector<std::vector<Edge*>> waysMarket_;
    std::vector<std::vector<int>> waysLengthStorage_;
    std::vector<std::vector<Edge*>> waysStorage_;

};

#endif // TRAIN_H
