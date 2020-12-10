#include "Map.h"

Map::Map(const QJsonObject &staticObj, const QJsonObject &dynamicObj, const QJsonObject &pointsCoords, Player& player) {
    if (!dynamicObj.contains("trains") || !dynamicObj["trains"].isArray() || !dynamicObj.contains("posts") || !dynamicObj["posts"].isArray() || !dynamicObj.contains("idx"))
        throw std::invalid_argument("Wrong JSON graph format.");

    QJsonArray trainsJsonArray = dynamicObj["trains"].toArray();
    QJsonArray postsJsonArray = dynamicObj["posts"].toArray();

    for (auto const &post : postsJsonArray) {
        if (!post.isObject())
            throw std::invalid_argument("Wrong JSON graph format.");

        int k = post.toObject()["type"].toInt();
        switch(k){
        case 1:
            towns_.emplace_back(post.toObject());
            posts_.emplace_back(post.toObject());

            if(post.toObject()["player_idx"].toString() != nullptr) {
                if(post.toObject()["player_idx"].toString() == player.idx()) {
                    player.setTown(post.toObject());
                }
            }
            break;
        case 2:
            markets_.emplace_back(post.toObject());
            posts_.emplace_back(post.toObject());
            break;
        case 3:
            storages_.emplace_back(post.toObject());
            posts_.emplace_back(post.toObject());
            break;
        }
    }

    for (auto const &train : trainsJsonArray) {
        if (!train.isObject())
            throw std::invalid_argument("Wrong JSON graph format.");
        trains_.emplace_back(train.toObject());

        if(train.toObject()["player_idx"].toString() != nullptr) {
            if(train.toObject()["player_idx"].toString() == player.idx()) {
                player.setTrain(train.toObject());
            }
        }
    }

    idx_ = dynamicObj["idx"].toInt();

    graph_ = Graph(staticObj, *this);
    graph_.calcCoords(16.0f / 9, pointsCoords);
}

Graph &Map::graph() {
    return graph_;
}

std::vector<Town>& Map::towns(){
    return towns_;
}

std::vector<Market>& Map::markets(){
    return markets_;
}

std::vector<Storage>& Map::storages(){
    return storages_;
}

std::vector<Post>& Map::posts(){
    return posts_;
}
