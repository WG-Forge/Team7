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
            events_.emplace_back(event.toObject());
        }
    }
    if (train.contains("fuel")){
        fuel_ = train["fuel"].toInt();
    }
    if (train.contains("fuel_capacity")){
        fuel_capacity_ = train["fuel_capacity"].toInt();
    }
    if (train.contains("fuel_consumption")){
        fuel_consumption_ = train["fuel_consumption"].toInt();
    }
    if (train.contains("goods")){
        goods_ = train["goods"].toInt();
    }
    if (train.contains("goods_capacity")){
        goods_capacity_ = train["goods_capacity"].toInt();
    }
    /*if (train.contains("goods_type")){
        goods_type_ = train["goods_type"].toObject();
    }*/
    if (train.contains("level")){
        level_ = train["level"].toInt();
    }
    if (train.contains("next_level_price")){
        next_level_price_ = train["next_level_price"].toInt();
    }
    idx_ = train["idx"].toInt();
    line_idx_ = train["line_idx"].toInt();
    player_idx_ = train["player_idx"].toString();
    position_ = train["position"].toInt();
    speed_ = train["speed"].toInt();
}

int Train::cooldown(){
    return cooldown_;
}

std::vector<Event>& Train::events() {
    return events_;
}

int Train::fuel(){
    return fuel_;
}

int Train::fuel_capacity(){
    return fuel_capacity_;
}

int Train::fuel_consumption(){
    return fuel_consumption_;
}

int Train::goods(){
    return goods_;
}

int Train::goods_capacity(){
    return goods_capacity_;
}

/*enum GoodsType Train::goods_type(){
    return goods_type_;
}*/

int Train::idx(){
    return idx_;
}

int Train::level(){
    return level_;
}

int Train::line_idx(){
    return line_idx_;
}

int Train::next_level_price(){
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

