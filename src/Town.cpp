#include "Town.h"

Town::Town(const QJsonObject& town){
    if(!town.contains("idx") || !town.contains("name") || !town.contains("type")){
        throw std::invalid_argument("Wrong JSON graph format.");
    }
    if (town.contains("events")){
        if(!town["events"].isArray()){
            throw std::invalid_argument("Wrong JSON graph format.");
        }
        QJsonArray eventsJsonArray = town["events"].toArray();
        for(auto event: eventsJsonArray){
            if (!event.isObject())
                throw std::invalid_argument("Wrong JSON graph format.");
            events_.emplace_back(event.toObject());
        }
    }
    idx_ = town["idx"].toInt();
    name_ = town["name"].toString();
    if(town.contains("armor")){
        armor_ = town["armor"].toInt();
        armor_capacity_ = town["armor_capacity"].toInt();
    }
    if(town.contains("level")){
        level_ = town["level"].toInt();
        next_level_price_ = town["next_level_price"].toInt();
    }
    if(town.contains("player_idx")){
        player_idx_ = town["player_idx"].toString();
    }
    point_idx_ = town["point_idx"].toInt();
    if(town.contains("population")){
        population_ = town["population"].toInt();
        population_capacity_ = town["population_capacity"].toInt();
    }
    if(town.contains("product")){
        product_ = town["product"].toInt();
        product_capacity_ = town["product_capacity"].toInt();
    }
    if(town.contains("train_cooldown")){
        train_cooldown_ = town["trani_cooldown"].toInt();
    }
    type_ = static_cast<PostType>(town["type"].toInt());
}

int Town::armor(){
    return product_;
}

int Town::armor_capacity(){
    return product_capacity_;
}

int Town::level(){
    return level_;
}

int Town::next_level_price(){
    return next_level_price_;
}

QString Town::player_idx(){
    return player_idx_;
}

int Town::point_idx(){
    return point_idx_;
}

int Town::population(){
    return product_;
}

int Town::population_capacity(){
    return product_capacity_;
}

int Town::product(){
    return product_;
}

int Town::product_capacity(){
    return product_capacity_;
}

int Town::train_cooldown(){
   return train_cooldown_;
}
