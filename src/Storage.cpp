#include "Storage.h"

Storage::Storage(const QJsonObject& storage){
    if(!storage.contains("idx") || !storage.contains("name") || !storage.contains("type")){
        throw std::invalid_argument("Wrong JSON graph format.");
    }
    if (storage.contains("events")){
        if(!storage["events"].isArray()){
            throw std::invalid_argument("Wrong JSON graph format.");
        }
        QJsonArray postsJsonArray = storage["events"].toArray();
        for(auto post: postsJsonArray){
            if (!post.isObject())
                throw std::invalid_argument("Wrong JSON graph format.");
            events_.emplace_back(post.toObject());
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
    type_ = static_cast<PostType>(storage["type"].toInt());
}

int Storage::point_idx(){
    return point_idx_;
}

int Storage::armor(){
    return armor_;
}

int Storage::armor_capacity(){
    return armor_capacity_;
}

int Storage::replenishment(){
    return replenishment_;
}
