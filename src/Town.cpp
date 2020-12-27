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
        armorCapacity_ = town["armor_capacity"].toInt();
    }
    else{
        armor_ = -1;
    }
    if(town.contains("level")){
        level_ = town["level"].toInt();
        nextLevelPrice_ = town["next_level_price"].toInt();
    }
    else{
        level_ = -1;
    }
    if(town.contains("player_idx")){
        playerIdx_ = town["player_idx"].toString();
    }
    else{
        playerIdx_ = nullptr;
    }

    pointIdx_ = town["point_idx"].toInt();

    if(town.contains("population")){
        population_ = town["population"].toInt();
        populationCapacity_ = town["population_capacity"].toInt();
    }
    else{
        population_ = -1;
    }
    if(town.contains("product")){
        product_ = town["product"].toInt();
        productCapacity_ = town["product_capacity"].toInt();
    }
    else{
        product_ = -1;
    }
    if(town.contains("train_cooldown")){
        trainCooldown_ = town["trani_cooldown"].toInt();
    }
    else{
        trainCooldown_ = -1;
    }

    type_ = static_cast<PostType>(town["type"].toInt());
}

int Town::armor(){
    if(armor_ == -1)
        throw std::invalid_argument("No armor");
    return armor_;
}

int Town::armorCapacity(){
    if(armor_ == -1)
        throw std::invalid_argument("No armor");
    return armorCapacity_;
}

int Town::level(){
    if(level_ == -1)
        throw std::invalid_argument("No level");
    return level_;
}

int Town::nextLevelPrice(){
    if(level_ == -1)
        throw std::invalid_argument("No level");
    return nextLevelPrice_;
}

QString Town::playerIdx(){
    if(playerIdx_ == -1)
        throw std::invalid_argument("No player_idx");
    return playerIdx_;
}

//int Town::pointIdx(){
//    return pointIdx_;
//}

int Town::population(){
    if(population_ == -1)
        throw std::invalid_argument("No population");
    return population_;
}

int Town::populationCapacity(){
    if(population_ == -1)
        throw std::invalid_argument("No population");
    return populationCapacity_;
}

int Town::product(){
    if(product_ == -1)
        throw std::invalid_argument("No product");
    return product_;
}

int Town::productCapacity(){
    if(product_ == -1)
        throw std::invalid_argument("No product");
    return productCapacity_;
}

int Town::trainCooldown(){
    if(trainCooldown_ == -1)
        throw std::invalid_argument("No train_cooldown");
   return trainCooldown_;
}

void Town::update(const QJsonObject& data) {

    armor_ = data["armor"].toInt();
    armorCapacity_ = data["armor_capacity"].toInt();

    level_ = data["level"].toInt();
    if (level_ == 3) {
        isMaxLevel_ = true;
        nextLevelPrice_ = -1;
    } else {
        nextLevelPrice_ = data["next_level_price"].toInt();
    }

    population_ = data["population"].toInt();
    populationCapacity_ = data["population_capacity"].toInt();

    product_ = data["product"].toInt();
    productCapacity_ = data["product_capacity"].toInt();

    trainCooldown_ = data["train_cooldown"].toInt();
}

//int Town::idx(){
//    return idx_;
//}

//QString Town::name(){
//    return name_;
//}

//enum PostType Town::type(){
//    return type_;
//}

//std::vector<Event*>& Town::events(){
//    return events_;
//}
