#include "Posts/Town.h"

Town::Town(const QJsonObject& town){
    if(!town.contains("idx") || !town.contains("name") || !town.contains("type")){
        throw std::invalid_argument("Wrong JSON graph format.");
    }
    if (town.contains("events")){
        if(!town["events"].isArray()){
            throw std::invalid_argument("Wrong JSON graph format.");
        }
        QJsonArray eventsJsonArray = town["events"].toArray();
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
    idx_ = town["idx"].toInt();
    name_ = town["name"].toString();
    if(town.contains("armor")){
        armor_ = town["armor"].toInt();
        armor_capacity_ = town["armor_capacity"].toInt();
    }
    else{
        armor_ = -1;
    }
    if(town.contains("level")){
        level_ = town["level"].toInt();
        next_level_price_ = town["next_level_price"].toInt();
    }
    else{
        level_ = -1;
    }
    if(town.contains("player_idx")){
        player_idx_ = town["player_idx"].toString();
    }
    else{
        player_idx_ = nullptr;
    }
    point_idx_ = town["point_idx"].toInt();
    if(town.contains("population")){
        population_ = town["population"].toInt();
        population_capacity_ = town["population_capacity"].toInt();
    }
    else{
        population_ = -1;
    }
    if(town.contains("product")){
        product_ = town["product"].toInt();
        product_capacity_ = town["product_capacity"].toInt();
    }
    else{
        product_ = -1;
    }
    if(town.contains("train_cooldown")){
        train_cooldown_ = town["trani_cooldown"].toInt();
    }
    else{
        train_cooldown_ = -1;
    }
    type_ = static_cast<PostType>(town["type"].toInt());
}

int Town::armor(){
    if(armor_ == -1)
        throw std::invalid_argument("No armor");
    return product_;
}

int Town::armor_capacity(){
    if(armor_ == -1)
        throw std::invalid_argument("No armor");
    return product_capacity_;
}

int Town::level(){
    if(level_ == -1)
        throw std::invalid_argument("No level");
    return level_;
}

int Town::next_level_price(){
    if(level_ == -1)
        throw std::invalid_argument("No level");
    return next_level_price_;
}

QString Town::player_idx(){
    if(player_idx_ == -1)
        throw std::invalid_argument("No player_idx");
    return player_idx_;
}

int Town::point_idx(){
    return point_idx_;
}

int Town::population(){
    if(population_ == -1)
        throw std::invalid_argument("No population");
    return product_;
}

int Town::population_capacity(){
    if(population_ == -1)
        throw std::invalid_argument("No population");
    return product_capacity_;
}

int Town::product(){
    if(product_ == -1)
        throw std::invalid_argument("No product");
    return product_;
}

int Town::product_capacity(){
    if(product_ == -1)
        throw std::invalid_argument("No product");
    return product_capacity_;
}

int Town::train_cooldown(){
    if(train_cooldown_ == -1)
        throw std::invalid_argument("No train_cooldown");
   return train_cooldown_;
}

int Town::idx(){
    return idx_;
}

QString Town::name(){
    return name_;
}

enum PostType Town::type(){
    return type_;
}

std::vector<Event*>& Town::events(){
    return events_;
}
