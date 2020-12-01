#include "Storage.h"

Storage::Storage(const QJsonObject& storage){
    if(!storage.contains("idx") || !storage.contains("name") || !storage.contains("type")){
        throw std::invalid_argument("Wrong JSON graph format.");
    }
    if (storage.contains("events")){
        if(!storage["events"].isArray()){
            throw std::invalid_argument("Wrong JSON graph format.");
        }
        QJsonArray eventsJsonArray = storage["events"].toArray();
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
    idx_ = storage["idx"].toInt();
    name_ = storage["name"].toString();
    point_idx_ = storage["point_idx"].toInt();
    if(storage.contains("armor")){
        armor_ = storage["armor"].toInt();
        armor_capacity_ = storage["armor_capacity"].toInt();
        replenishment_ = storage["replenishment"].toInt();
    }
    else{
        armor_ = -1;
    }
    type_ = static_cast<PostType>(storage["type"].toInt());
}

int Storage::point_idx(){
    return point_idx_;
}

int Storage::armor(){
    if(armor_ == -1)
        throw std::invalid_argument("No armor");
    return armor_;
}

int Storage::armor_capacity(){
    if(armor_ == -1)
        throw std::invalid_argument("No armor");
    return armor_capacity_;
}

int Storage::replenishment(){
    if(armor_ == -1)
        throw std::invalid_argument("No armor");
    return replenishment_;
}
