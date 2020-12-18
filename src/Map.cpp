#include "Map.h"

Map::Map(const QJsonObject &staticObj, const QJsonObject &dynamicObj, const QJsonObject &pointsCoords, Player& player) {
    if (!dynamicObj.contains("trains") || !dynamicObj["trains"].isArray() || !dynamicObj.contains("posts") || !dynamicObj["posts"].isArray() || !dynamicObj.contains("idx"))
        throw std::invalid_argument("Wrong JSON graph format.");

    QJsonArray trainsJsonArray = dynamicObj["trains"].toArray();
    QJsonArray postsJsonArray = dynamicObj["posts"].toArray();

    idx_ = dynamicObj["idx"].toInt();
    graph_ = Graph(staticObj, *this);
    graph_.calcCoords(16.0f / 9, pointsCoords);


    for (auto const &post : postsJsonArray) {
        if (!post.isObject())
            throw std::invalid_argument("Wrong JSON graph format.");

        int k = post.toObject()["type"].toInt();
        switch(k){
        case 1:
            towns_.emplace_back(post.toObject());
            posts_.emplace_back(post.toObject());
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

    for (auto &vertex : graph_.vertices()) {
        if (vertex.isPostIdxNull()) continue;

        for (Post &post : this->posts()) {
            if (post.pointIdx() == vertex.idx() && post.idx() == vertex.postIdx()) {
                vertex.setPost(post);
                post.setVertex(vertex);
                continue;
            }

            enum PostType type = post.type();

            switch(type) {
            case PostType::TOWN:
                for (Town &town : this->towns()) {
                    if (town.pointIdx() == vertex.idx() && town.idx() == vertex.postIdx()) {
                        town.setVertex(vertex);

                        if (town.playerIdx() != nullptr) {
                            if (town.playerIdx() == player.idx()) {
                                player.setTown2(town);
                            }
                        }
                        break;
                    }
                }
                break;

            case PostType::MARKET:
                for (Market &market : this->markets()) {
                    if (market.pointIdx() == vertex.idx() && market.idx() == vertex.postIdx()) {
                        market.setVertex(vertex);
                        break;
                    }
                }
                break;

            case PostType::STORAGE:
                for (Storage &storage : this->storages()) {
                    if (storage.pointIdx() == vertex.idx() && storage.idx() == vertex.postIdx()) {
                        storage.setVertex(vertex);
                        break;
                    }
                }
                break;
            }
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

void Map::makeWays(){
    int n = graph_.vertices().size();
    std::vector<std::vector<int>> mas(n);
    std::vector<std::vector<int>> p(n);
    int INF = 100000200;

    for (int i = 0 ; i < n; ++i){
        mas[i] = std::vector<int>(n, INF);
        p[i] = std::vector<int>(n, INF);
        mas[i][i] = 0;
        p[i][i] = 0;
    }

    for(int t = 0; t < trains_.size(); ++t){
        for(int i = 0 ; i < n;++i){
            int u =  graph_.vertices()[i].idx();
            for(int j = 0; j < graph_.vertices()[i].edges().size(); ++j){
                if(u == graph_.vertices()[i].edges()[j].get().vertex1().idx()){
                    mas[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())]
                        [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())] =
                        graph_.vertices()[i].edges()[j].get().length();
                }
                else{
                    mas[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())]
                        [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())] =
                        graph_.vertices()[i].edges()[j].get().length();
                }
            }
        }
        for(int i = 0; i < n; ++i){
            for(int j = 0; j < n;++j){
                p[i][j] = mas[i][j];
                if(mas[i][j] != 0 && mas[i][j] != INF){
                    p[i][j] = j;
                }
            }
        }
        for(int k = 0; k < n; ++k){
            for(int i = 0; i < n; ++i){
                for(int j = 0; j < n; ++j){
                    if(mas[i][j] > mas[i][k] + mas[k][j]){
                            p[i][j] = p[i][k];
                    }
                    mas[i][j] = std::min(mas[i][j], mas[i][k] + mas[k][j]);
                }
            }
        }
        trains_[t].trainWays(mas,p);
    }
}

std::vector<Train>& Map::trains(){
    return trains_;
}


