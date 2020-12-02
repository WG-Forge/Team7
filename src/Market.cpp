#include "Posts/Market.h"

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
    idx_ = market["idx"].toInt();
    name_ = market["name"].toString();
    point_idx_ = market["point_idx"].toInt();
    if(market.contains("product")){
        product_ = market["product"].toInt();
        product_capacity_ = market["product_capacity"].toInt();
        replenishment_ = market["replenishment"].toInt();
    }
    else{
        product_ = -1;
    }
    type_ = static_cast<PostType>(market["type"].toInt());
}


int Market::point_idx(){
    return point_idx_;
}

int Market::product(){
    if(product_ == -1)
        throw std::invalid_argument("No product");
    return product_;
}

int Market::product_capacity(){
    if(product_ == -1)
        throw std::invalid_argument("No product");
    return product_capacity_;
}

int Market::replenishment(){
    if(product_ == -1)
        throw std::invalid_argument("No product");
    return replenishment_;
}

int Market::idx(){
    return idx_;
}

QString Market::name(){
    return name_;
}

enum PostType Market::type(){
    return type_;
}

std::vector<Event*>& Market::events(){
    return events_;
}
