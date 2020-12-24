#ifndef TRAIN_H
#define TRAIN_H
#include "Event.h"
#include "Enums/GoodsType.h"
#include "Enums/WaysType.h"
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

    void upgrade(int level, int goodsCapacity, int fuelCapacity, int nextLevelPrice){
        level_ = level;
        goodsCapacity_ = goodsCapacity;
        fuelCapacity_ = fuelCapacity;
        nextLevelPrice_ = nextLevelPrice;
    };

    void setFinalVertex(Vertex *vertex) { finalVertex_ = vertex; };
    void setCurrentVertex(Vertex *vertex) { currentVertex_ = vertex; };
    void setNextVertex(Vertex *vertex) { nextVertex_ = vertex; };
    void setFinalLinePosition(int position) { finalLinePosition_ = position; };
    void update(const QJsonObject& data);

    int cooldown();
    int fuel();
    int fuelCapacity();
    int fuelConsumption();
    int goods();
    int goodsCapacity();
    int idx();
    int level();
    int lineIdx();
    int nextLevelPrice();
    int position();
    int speed();
    int finalLinePosition() { return finalLinePosition_; };
    int priority() { return priority_; };
    bool isMaxLevel() { return isMaxLevel_; };
    bool cheat(){return cheat_; };
    QString playerIdx();
    std::vector<Event*>& events();

    Vertex* finalVertex(){return finalVertex_; };
    Vertex* currentVertex(){return currentVertex_; };
    Vertex* nextVertex(){return nextVertex_; };
    enum GoodsType goodsType();

    std::vector<std::vector<int>> waysLengthStorage() { return waysLengthStorage_; };
    std::vector<std::vector<int>> waysLengthMarket() { return waysLengthMarket_; };
    std::vector<std::vector<int>> waysLengthReturn() { return waysLengthReturn_; };
    std::vector<std::vector<int>> waysLengthAll() { return waysLengthAll_; };
    std::vector<std::vector<Edge *>> waysStorage() { return waysStorage_; };
    std::vector<std::vector<Edge *>> waysMarket() { return waysMarket_; };
    std::vector<std::vector<Edge *>> waysReturn() { return waysReturn_; };
    std::vector<std::vector<Edge *>> waysAll() { return waysAll_; };
    void trainWays(const std::vector<std::vector<int>>& masMarket,
                   const  std::vector<std::vector<Edge*>>& pMarket,
                   const std::vector<std::vector<int>>& masStorage,
                   const  std::vector<std::vector<Edge*>>& pStorage,
                   const std::vector<std::vector<int>>& mas,
                   const  std::vector<std::vector<Edge*>>& p,
                   const std::vector<std::vector<int>>& masAll,
                   const  std::vector<std::vector<Edge*>>& pAll);

private:
    int cooldown_;
    int fuel_;
    int fuelCapacity_;
    int fuelConsumption_;
    int goods_;
    int goodsCapacity_;
    int idx_;
    int level_;
    int lineIdx_;
    int nextLevelPrice_;
    int position_;
    int speed_;
    int priority_;
    int finalLinePosition_ = -1;
    bool isMaxLevel_;
    bool cheat_ = false;
    QString playerIdx_;
    std::vector<Event*> events_;
    enum GoodsType goodsType_;
     // конечная позиция на линии
    Vertex *finalVertex_ = nullptr;     // конечный вертекс (прям самый конечный, в который хочешь доехать)
    Vertex *currentVertex_ = nullptr;   // вертекс из которого выехал или в котором стоишь
    Vertex *nextVertex_ = nullptr;      // в который едешь сейчас aka промежуточный между currentVertex и finalVertex

    std::vector<std::vector<int>> waysLengthMarket_;
    std::vector<std::vector<Edge*>> waysMarket_;
    std::vector<std::vector<int>> waysLengthStorage_;
    std::vector<std::vector<Edge*>> waysStorage_;
    std::vector<std::vector<int>> waysLengthReturn_;
    std::vector<std::vector<Edge*>> waysReturn_;
    std::vector<std::vector<int>> waysLengthAll_;
    std::vector<std::vector<Edge*>> waysAll_;
};

#endif // TRAIN_H
