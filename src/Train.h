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

    void setNextVertex(Vertex *vertex) { nextVertex_ = vertex; };
    void setFinalVertex(Vertex *vertex) { finalVertex_ = vertex; };
    void setCurrentVertex(Vertex *vertex) { currentVertex_ = vertex; };
    void setCurrentPath(std::vector<Vertex*> path) {currentPath_ = path;};

    void setEdge(Edge *edge) { edge_ = edge; };
    void setCurrentIndex(int index){currentIndex_ = index;};
    void setWaysType(WaysType waysType){ waysType_ = waysType; };
    void setFinalLinePosition(int position) { finalLinePosition_ = position; };
    void setWaitingTime(int x){ waitingTime_ = x;};
    void setSpeed(int speed){ speed_ = speed ;};
    void update(const QJsonObject& data);
    int currentIndex(){return currentIndex_;};

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
    int priority() { return priority_; };
    bool isMaxLevel() { return isMaxLevel_; };
    int waitingTime(){return waitingTime_;};
    bool cheat(){return cheat_; };
    QString playerIdx();
    std::vector<TrainCollision*>& events();
    void popEvent(){events_.pop_back();};

    Vertex* nextVertex(){return nextVertex_; };
    Vertex* finalVertex(){return finalVertex_; };
    Vertex* currentVertex(){return currentVertex_; };
    int finalLinePosition() { return finalLinePosition_; };
    Edge* edge(){return edge_; };
    enum GoodsType goodsType();

    std::vector<std::vector<int>> waysLengthStorage() { return waysLengthStorage_; };
    std::vector<std::vector<int>> waysLengthMarket() { return waysLengthMarket_; };
    std::vector<std::vector<int>> waysLengthReturn() { return waysLengthReturn_; };
    std::vector<std::vector<int>> waysLengthAll() { return waysLengthAll_; };
    std::vector<std::vector<Edge *>> waysStorage() { return waysStorage_; };
    std::vector<std::vector<Edge *>> waysMarket() { return waysMarket_; };
    std::vector<std::vector<Edge *>> waysReturn() { return waysReturn_; };
    std::vector<std::vector<Edge *>> waysAll() { return waysAll_; };
    std::vector<Vertex*> currentPath(){ return currentPath_;};
    WaysType waysType(){ return waysType_; };
    void trainWays(const std::vector<std::vector<int>>& masMarket,
                   const  std::vector<std::vector<Edge*>>& pMarket,
                   const std::vector<std::vector<int>>& masStorage,
                   const  std::vector<std::vector<Edge*>>& pStorage,
                   const std::vector<std::vector<int>>& mas,
                   const  std::vector<std::vector<Edge*>>& p,
                   const std::vector<std::vector<int>>& masAll,
                   const  std::vector<std::vector<Edge*>>& pAll);

    Vertex start;
    Vertex goal;
    Vertex current;
    Vertex next;
private:
    int cooldown_;
    int fuel_;
    int fuelCapacity_;
    int fuelConsumption_;
    int goods_;
    int goodsCapacity_;
    int currentIndex_;
    int idx_;
    int level_;
    int lineIdx_;
    int nextLevelPrice_;
    int position_;
    int speed_;
    int priority_;
    bool isMaxLevel_;
    bool cheat_ = false;
    QString playerIdx_;
    std::vector<TrainCollision*> events_;
    enum GoodsType goodsType_;

    int finalLinePosition_ = -1;
    Vertex *nextVertex_ = nullptr;   // вертекс из которого выехал или в котором стоишь
    Vertex *finalVertex_ = nullptr;     // конечный вертекс (прям самый конечный, в который хочешь доехать)
    Vertex *currentVertex_ = nullptr;   // вертекс из которого выехал или в котором стоишь
    Edge *edge_ = nullptr;      // в который едешь сейчас aka промежуточный между currentVertex и finalVertex
    enum WaysType waysType_;
    int waitingTime_;
    std::vector<std::vector<int>> waysLengthMarket_;
    std::vector<std::vector<Edge*>> waysMarket_;
    std::vector<std::vector<int>> waysLengthStorage_;
    std::vector<std::vector<Edge*>> waysStorage_;
    std::vector<std::vector<int>> waysLengthReturn_;
    std::vector<std::vector<Edge*>> waysReturn_;
    std::vector<std::vector<int>> waysLengthAll_;
    std::vector<std::vector<Edge*>> waysAll_;
    std::vector<Vertex*> currentPath_;
};

#endif // TRAIN_H
