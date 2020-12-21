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
        fuel_capacity_ = train["fuel_capacity"].toInt();
        fuel_consumption_ = train["fuel_consumption"].toInt();
    }
    else{
        fuel_ = -1;
    }
    if (train.contains("goods")){
        goods_ = train["goods"].toInt();
        goods_capacity_ = train["goods_capacity"].toInt();
        goods_type_ = static_cast<GoodsType>(train["goods_type"].toInt());
    }
    else{
        goods_ = -1;
    }
    if (train.contains("level")){
        level_ = train["level"].toInt();
        next_level_price_ = train["next_level_price"].toInt();
    }
    else{
        level_ = -1;
    }
    if (train.contains("line_idx")){
        line_idx_ = train["line_idx"].toInt();
    }
    else{
        line_idx_ = -1;
    }
    idx_ = train["idx"].toInt();
    player_idx_ = train["player_idx"].toString();
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

int Train::fuel_capacity(){
    if(fuel_ == -1)
        throw std::invalid_argument("No fuel");
    return fuel_capacity_;
}

int Train::fuel_consumption(){
    if(fuel_ == -1)
        throw std::invalid_argument("No fuel");
    return fuel_consumption_;
}

int Train::goods(){
    if(goods_ == -1)
        throw std::invalid_argument("No goods");
    return goods_;

}

int Train::goods_capacity(){
    if(goods_ == -1)
        throw std::invalid_argument("No goods");
    return goods_capacity_;
}

enum GoodsType Train::goods_type(){
    if(goods_ == -1)
        throw std::invalid_argument("No goods");
    return goods_type_;
}

int Train::idx(){
    return idx_;
}

int Train::level(){
    if(level_ == -1)
        throw std::invalid_argument("No level");
    return level_;
}

int Train::line_idx(){
    if(line_idx_ == -1)
       throw std::invalid_argument("No line_idx");
    return line_idx_;
}

int Train::next_level_price(){
    if(level_ == -1)
        throw std::invalid_argument("No level");
    return next_level_price_;
}
QString Train::player_idx(){
    return player_idx_;
}

int Train::position(){
    return position_;
}

int Train::speed(){
    return position_;
}

void Train::trainWays(const std::vector<std::vector<int>>& masMarket,
                      const  std::vector<std::vector<Edge*>>& pMarket,
                      const std::vector<std::vector<int>>& masStorage,
                      const  std::vector<std::vector<Edge*>>& pStorage){
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
}
