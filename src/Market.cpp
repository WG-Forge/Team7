#include "Market.h"

Market::Market(const QJsonObject& market){
    if(!market.contains("idx") || !market.contains("name") || !market.contains("type")){
        throw std::invalid_argument("Wrong JSON graph format.");
    }
    if (market.contains("events")){
        if(!market["events"].isArray()){
            throw std::invalid_argument("Wrong JSON graph format.");
        }
        QJsonArray eventsJsonArray = market["events"].toArray();
        for(auto const &event: eventsJsonArray){
            if (!event.isObject())
                throw std::invalid_argument("Wrong JSON graph format.");
            events_.emplace_back(event.toObject());
        }
    }
    idx_ = market["idx"].toInt();
    name_ = market["name"].toString();
    point_idx_ = market["point_idx"].toInt();
    if(market.contains("product")){
        product_ = market["product"].toInt();
        product_capacity_ = market["product_capacity"].toInt();
        replenishment_ = market["replenishment"].toInt();
    }
    type_ = static_cast<PostType>(market["type"].toInt());
}

int Market::point_idx(){
    return point_idx_;
}

int Market::product(){
    return product_;
}

int Market::product_capacity(){
    return product_capacity_;
}

int Market::replenishment(){
    return replenishment_;
}
