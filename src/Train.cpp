#include "Train.h"

Train::Train(const QJsonObject &train){
    if(!train.contains("idx") || !train.contains("line_idx") || !train.contains("player_idx") || !train.contains("position")
            || !train.contains("speed")){
        throw std::invalid_argument("Wrong JSON graph format.");
    }
    if (train.contains("cooldown")){
        cooldown_ = train["cooldown"].toInt();
    }
    if (train.contains("events")){
        if(!train["events"].isArray()){
            throw std::invalid_argument("Wrong JSON graph format.");
        }
        QJsonArray eventsJsonArray = train["events"].toArray();
        for(auto event: eventsJsonArray){
            if (!event.isObject())
                throw std::invalid_argument("Wrong JSON graph format.");
            int k = event.toObject()["type"].toInt();
            switch(k){
            case 1:
                events_.push_back(new TrainCollision(event.toObject()));
                break;
            case 2:
                events_.push_back(new HijackersAssault(event.toObject()));
                break;
            case 3:
                events_.push_back(new ParasitesAssault(event.toObject()));
                break;
            case 4:
                events_.push_back(new RefugeesArrival(event.toObject()));
                break;
            }
        }
    }
    if (train.contains("fuel")){
        fuel_ = train["fuel"].toInt();
        fuelCapacity_ = train["fuel_capacity"].toInt();
        fuelConsumption_ = train["fuel_consumption"].toInt();
    }
    else{
        fuel_ = -1;
    }
    if (train.contains("goods")){
        goods_ = train["goods"].toInt();
        goodsCapacity_ = train["goods_capacity"].toInt();
        goodsType_ = static_cast<GoodsType>(train["goods_type"].toInt());
    }
    else{
        goods_ = -1;
    }
    if (train.contains("level")){
        level_ = train["level"].toInt();
        nextLevelPrice_ = train["next_level_price"].toInt();
    }
    else{
        level_ = -1;
    }
    if (train.contains("line_idx")){
        lineIdx_ = train["line_idx"].toInt();
    }
    else{
        lineIdx_ = -1;
    }
    idx_ = train["idx"].toInt();
    playerIdx_ = train["player_idx"].toString();
    position_ = train["position"].toInt();
    speed_ = train["speed"].toInt();
}

int Train::cooldown(){
    return cooldown_;
}

std::vector<Event*>& Train::events() {
    return events_;
}

int Train::fuel(){
    if(fuel_ == -1)
        throw std::invalid_argument("No fuel");
    return fuel_;
}

int Train::fuelCapacity(){
    if(fuel_ == -1)
        throw std::invalid_argument("No fuel");
    return fuelCapacity_;
}

int Train::fuelConsumption(){
    if(fuel_ == -1)
        throw std::invalid_argument("No fuel");
    return fuelConsumption_;
}

int Train::goods(){
    if(goods_ == -1)
        throw std::invalid_argument("No goods");
    return goods_;
}

int Train::goodsCapacity(){
    if(goods_ == -1)
        throw std::invalid_argument("No goods");
    return goodsCapacity_;
}

enum GoodsType Train::goodsType(){
    if(goods_ == -1)
        throw std::invalid_argument("No goods");
    return goodsType_;
}

int Train::idx(){
    return idx_;
}

int Train::level(){
    if(level_ == -1)
        throw std::invalid_argument("No level");
    return level_;
}

int Train::lineIdx(){
    if(lineIdx_ == -1)
       throw std::invalid_argument("No line_idx");
    return lineIdx_;
}

int Train::nextLevelPrice(){
    if(level_ == -1)
        throw std::invalid_argument("No level");
    return nextLevelPrice_;
}
QString Train::playerIdx(){
    return playerIdx_;
}

int Train::position(){
    return position_;
}

int Train::speed(){
    return speed_;
}

void Train::trainWays(const std::vector<std::vector<int>>& masMarket,
                      const  std::vector<std::vector<Edge*>>& pMarket,
                      const std::vector<std::vector<int>>& masStorage,
                      const  std::vector<std::vector<Edge*>>& pStorage,
                      const std::vector<std::vector<int>>& mas,
                      const  std::vector<std::vector<Edge*>>& p,
                      const std::vector<std::vector<int>>& masAll,
                      const  std::vector<std::vector<Edge*>>& pAll){
   for(int i = 0; i < masMarket.size(); ++i){
       waysLengthMarket_.push_back(std::vector<int>(masMarket[i]));
   }
   for(int i = 0; i < pMarket.size(); ++i){
       waysMarket_.push_back(pMarket[i]);
   }
   for(int i = 0; i < masStorage.size(); ++i){
       waysLengthStorage_.push_back(std::vector<int>(masStorage[i]));
   }
   for(int i = 0; i < pStorage.size(); ++i){
       waysStorage_.push_back(pStorage[i]);
   }

   for(int i = 0; i < mas.size(); ++i){
       waysLengthReturn_.push_back(std::vector<int>(mas[i]));
   }
   for(int i = 0; i < p.size(); ++i){
       waysReturn_.push_back(p[i]);
   }

   for(int i = 0; i < masAll.size(); ++i){
       waysLengthAll_.push_back(std::vector<int>(masAll[i]));
   }
   for(int i = 0; i < pAll.size(); ++i){
       waysAll_.push_back(pAll[i]);
   }
}

void Train::update(const QJsonObject &train){

    if (train.contains("fuel")){
        fuel_ = train["fuel"].toInt();
        fuelCapacity_ = train["fuel_capacity"].toInt();
        fuelConsumption_ = train["fuel_consumption"].toInt();
    }
    else{
        fuel_ = -1;
    }
    if (train.contains("goods")){
        goods_ = train["goods"].toInt();
        goodsCapacity_ = train["goods_capacity"].toInt();
        goodsType_ = static_cast<GoodsType>(train["goods_type"].toInt());
    }
    else{
        goods_ = -1;
    }
    if (train.contains("level")){
        level_ = train["level"].toInt();
        if (level_ == 3) {
            isMaxLevel_ = true;
            nextLevelPrice_ = -1;
        } else {
            nextLevelPrice_ = train["next_level_price"].toInt();
        }
    }
    else{
        level_ = -1;
    }
    if (train.contains("line_idx")){
        lineIdx_ = train["line_idx"].toInt();
    }
    else{
        lineIdx_ = -1;
    }

    position_ = train["position"].toInt();
    speed_ = train["speed"].toInt();
}

