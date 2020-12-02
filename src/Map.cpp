#include "Map.h"

Map::Map(const QJsonObject &map){
    if (!map.contains("trains") || !map["trains"].isArray() || !map.contains("posts") || !map["posts"].isArray() || !map.contains("idx"))
        throw std::invalid_argument("Wrong JSON graph format.");
    QJsonArray trainsJsonArray = map["trains"].toArray();
    QJsonArray postsJsonArray = map["posts"].toArray();
    for (auto const &post : postsJsonArray) {
        if (!post.isObject())
            throw std::invalid_argument("Wrong JSON graph format.");
        int k = post.toObject()["type"].toInt();
        switch(k){
        case 1:
            towns_.emplace_back(post.toObject());
            break;
        case 2:
            markets_.emplace_back(post.toObject());
            break;
        case 3:
            storages_.emplace_back(post.toObject());
            break;
        }
    }
    for (auto const &train : trainsJsonArray) {
        if (!train.isObject())
            throw std::invalid_argument("Wrong JSON graph format.");
        trains_.emplace_back(train.toObject());
    }
    idx_ = map["idx"].toInt();
}

std::vector<Town>& Map::towns(){
    return towns_;
}
