#ifndef TRAIN_H
#define TRAIN_H
#include "Event.h"
#include "Enums/GoodsType.h"
#include "Events/HijackersAssault.h"
#include "Events/RefugeesArrival.h"
#include "Events/TrainCollision.h"
#include "Events/ParasitesAssault.h"
#include "Vertex.h"
#include "Socket.h"
#include "Edge.h"

#include <QJsonObject>
#include <QJsonArray>
#include <stdexcept>

class Train{
public:
    explicit Train() = default;
    explicit Train(const QJsonObject &train);

    void addFuel(int fuelAmount) { fuel_ += fuelAmount; };
    void move(int distance, int lineIdx, int speed, int position) {
        fuel_ -= distance * fuelConsumption_;
        if (lineIdx_ != lineIdx) lineIdx_ = lineIdx;
        if (speed_ != speed) speed_ = speed;
        if (position_ != position) position_ = position;
    };
    void setPosition(int position) { position_ = position; };
    void changeGoodsAmount(int goodsAmount) {
        goods_ += goodsAmount;
    };

    int cooldown();
    void trainWays(const std::vector<std::vector<int>>& masMarket,
                   const  std::vector<std::vector<Edge*>>& pMarket,
                   const std::vector<std::vector<int>>& masStorage,
                   const  std::vector<std::vector<Edge*>>& pStorage);
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
    std::vector<std::vector<int>> waysLengthStorage() { return waysLengthStorage_; };
    std::vector<std::vector<int>> waysLengthMarket() { return waysLengthMarket_; };
    std::vector<std::vector<Edge *>> waysStorage() { return waysStorage_; };
    std::vector<std::vector<Edge *>> waysMarket() { return waysMarket_; };
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
    std::vector<std::vector<int>> waysLengthMarket_;
    std::vector<std::vector<Edge*>> waysMarket_;
    std::vector<std::vector<int>> waysLengthStorage_;
    std::vector<std::vector<Edge*>> waysStorage_;

};

#endif // TRAIN_H
