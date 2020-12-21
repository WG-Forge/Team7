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

void Map::makeWays(){
    int verticesNumber = graph_.vertices().size();
    std::vector<std::vector<int>> masMarket(verticesNumber);
    std::vector<std::vector<Edge*>> pMarket(verticesNumber);
    std::vector<std::vector<int>> masStorage(verticesNumber);
    std::vector<std::vector<Edge*>> pStorage(verticesNumber);
    int INF = 100000000;

    for(int t = 0; t < trains_.size(); ++t){
        for (int i = 0 ; i < verticesNumber; ++i){
            masMarket[i] = std::vector<int>(verticesNumber, INF);
            pMarket[i] = std::vector<Edge*>(verticesNumber);
            masMarket[i][i] = 0;
            masStorage[i] = std::vector<int>(verticesNumber, INF);
            pStorage[i] = std::vector<Edge*>(verticesNumber);
            masStorage[i][i] = 0;
        }
        for(int i = 0 ; i < verticesNumber;++i){
            int u =  graph_.vertices()[i].idx();
            for(int j = 0; j < graph_.vertices()[i].edges().size(); ++j){
                if(u == graph_.vertices()[i].edges()[j].get().vertex1().idx()){

                    masMarket[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())]
                        [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())] =
                        graph_.vertices()[i].edges()[j].get().length();
                    pMarket[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())]
                            [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())]
                            = &graph_.vertices()[i].edges()[j].get();

                    masStorage[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())]
                        [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())] =
                        graph_.vertices()[i].edges()[j].get().length();
                    pStorage[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())]
                            [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())]
                            = &graph_.vertices()[i].edges()[j].get();

                }
                else{
                    masMarket[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())]
                        [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())] =
                        graph_.vertices()[i].edges()[j].get().length();
                    pMarket[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())]
                            [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())]
                            = &graph_.vertices()[i].edges()[j].get();

                    masStorage[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())]
                        [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())] =
                        graph_.vertices()[i].edges()[j].get().length();
                    pStorage[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())]
                            [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())]
                            = &graph_.vertices()[i].edges()[j].get();

                }
                //NoStorages
               if( graph_.vertices()[i].edges()[j].get().vertex1().isPostIdxNull() == false){
                    if(static_cast<int>(graph_.vertices()[i].edges()[j].get().vertex1().post().type()) == 3){
                        masMarket[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())]
                            [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())] =
                            -1;
                        masMarket[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())]
                            [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())] =
                            -1;
                    }
                }
               if( graph_.vertices()[i].edges()[j].get().vertex2().isPostIdxNull() == false){
                   if(static_cast<int>(graph_.vertices()[i].edges()[j].get().vertex2().post().type()) == 3){
                       masMarket[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())]
                           [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())] =
                           -1;
                       masMarket[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())]
                           [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())] =
                           -1;
                   }
               }
               //NoMarkets
               if( graph_.vertices()[i].edges()[j].get().vertex1().isPostIdxNull() == false){
                    if(static_cast<int>(graph_.vertices()[i].edges()[j].get().vertex1().post().type()) == 2){
                        masStorage[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())]
                            [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())] =
                            -1;
                        masStorage[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())]
                            [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())] =
                            -1;

                    }
                }
                if( graph_.vertices()[i].edges()[j].get().vertex2().isPostIdxNull() == false){
                    if(static_cast<int>(graph_.vertices()[i].edges()[j].get().vertex2().post().type()) == 2){
                        masStorage[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())]
                            [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())] =
                            -1;
                        masStorage[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())]
                            [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())] =
                            -1;
                    }
                }

                if(masMarket[i][j] == INF && graph_.vertices()[i].isPostIdxNull() == false){
                   if(static_cast<int>(graph_.vertices()[i].post().type()) == 3){
                        masMarket[i][j] = -1;
                        masMarket[j][i] = -1;

                    }
                }
                if(masMarket[i][j] == INF && graph_.vertices()[j].isPostIdxNull() == false){
                    if(static_cast<int>(graph_.vertices()[j].post().type()) == 3){
                         masMarket[i][j] = -1;
                         masMarket[j][i] = -1;
                     }
                }
                if(masStorage[i][j] == INF && graph_.vertices()[i].isPostIdxNull() == false){
                   if(static_cast<int>(graph_.vertices()[i].post().type()) == 2){
                        masStorage[i][j] = -1;
                        masStorage[j][i] = -1;

                    }
                }
                if(masStorage[i][j] == INF && graph_.vertices()[j].isPostIdxNull() == false){
                    if(static_cast<int>(graph_.vertices()[j].post().type()) == 2){
                         masStorage[i][j] = -1;
                         masStorage[j][i] = -1;
                     }
                }
            }
        }
        for(int k = 0; k < verticesNumber; ++k){
            for(int i = 0; i < verticesNumber; ++i){
                for(int j = 0; j < verticesNumber; ++j){
                    if(masMarket[i][j] != -1 && masMarket[i][k] != -1 && masMarket[k][j] != -1){
                        if(masMarket[i][j] > masMarket[i][k] + masMarket[k][j]){
                                pMarket[i][j] = pMarket[i][k];
                        }
                        masMarket[i][j] = std::min(masMarket[i][j], masMarket[i][k] + masMarket[k][j]);
                    }
                    if(masStorage[i][j] != -1 && masStorage[i][k] != -1 && masStorage[k][j] != -1){
                        if(masStorage[i][j] > masStorage[i][k] + masStorage[k][j]){
                                pStorage[i][j] = pStorage[i][k];
                        }
                        masStorage[i][j] = std::min(masStorage[i][j], masStorage[i][k] + masStorage[k][j]);
                    }
                }
            }
        }
        trains_[t].trainWays(masMarket, pMarket, masStorage, pStorage);
    }
}

std::vector<Train>& Map::trains(){
    return trains_;
}


