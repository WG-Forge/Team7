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
    void move(int lineIdx, int speed, int position) {
        if (lineIdx_ != lineIdx) lineIdx_ = lineIdx;
        if (speed_ != speed) speed_ = speed;
        if (position_ != position) position_ = position;
    };
    void setPosition(int position) {
        position_ = position;
    };
    void changeGoodsAmount(int goodsAmount) {
        goods_ = goodsAmount;
    };
    void upgrade(int level, int goodsCapacity, int fuelCapacity, int nextLevelPrice){
        level_ = level;
        goodsCapacity_ = goodsCapacity;
        fuelCapacity_ = fuelCapacity;
        nextLevelPrice_ = nextLevelPrice;
    };
    void setPriority(int newPriority) {
        priority_ = newPriority;
    };

    int cooldown();
    void trainWays(const std::vector<std::vector<int>>& masMarket,
                   const  std::vector<std::vector<Edge*>>& pMarket,
                   const std::vector<std::vector<int>>& masStorage,
                   const  std::vector<std::vector<Edge*>>& pStorage,
                   const std::vector<std::vector<int>>& mas,
                   const  std::vector<std::vector<Edge*>>& p);
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
    int speed();
    int priority() { return priority_; };
    bool isMaxLevel() { return isMaxLevel_; };

    std::vector<std::vector<int>> waysLengthStorage() { return waysLengthStorage_; };
    std::vector<std::vector<int>> waysLengthMarket() { return waysLengthMarket_; };
    std::vector<std::vector<Edge *>> waysStorage() { return waysStorage_; };
    std::vector<std::vector<Edge *>> waysMarket() { return waysMarket_; };
    void update(const QJsonObject& data);

    void setFinalVertex(Vertex *vertex) { finalVertex_ = vertex; };
    void setCurrentVertex(Vertex *vertex) { currentVertex_ = vertex; };
    void setFinalLinePosition(int position) { finalLinePosition_ = position; };
    void setNextVertex(Vertex *vertex) { nextVertex_ = vertex; };

    Vertex &finalVertex() { return *finalVertex_; };
    Vertex &currentVertex() { return *currentVertex_; };
    Vertex &nextVertex() { return *nextVertex_; };
    int finalLinePosition() { return finalLinePosition_; };

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
    int priority_;
    bool isMaxLevel_;
    std::vector<std::vector<int>> waysLengthMarket_;
    std::vector<std::vector<Edge*>> waysMarket_;
    std::vector<std::vector<int>> waysLengthStorage_;
    std::vector<std::vector<Edge*>> waysStorage_;
    std::vector<std::vector<int>> waysLengthReturn_;
    std::vector<std::vector<Edge*>> waysReturn_;

    int finalLinePosition_ = -1;        // конечная позиция на линии
    Vertex *finalVertex_ = nullptr;     // конечный вертекс (прям самый конечный, в который хочешь доехать)
    Vertex *currentVertex_ = nullptr;   // вертекс из которого выехал или в котором стоишь
    Vertex *nextVertex_ = nullptr;      // в который едешь сейчас aka промежуточный между currentVertex и finalVertex
};

#endif // TRAIN_H
