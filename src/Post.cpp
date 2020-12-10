#include "Post.h"
#include "Graph.h"
#include "Vertex.h"

Post::Post(const QJsonObject &post) {
    if(!post.contains("idx") || !post.contains("name") || !post.contains("type")){
        throw std::invalid_argument("Wrong JSON graph format.");
    }
    if (post.contains("events")){
        if(!post["events"].isArray()){
            throw std::invalid_argument("Wrong JSON graph format.");
        }
        QJsonArray eventsJsonArray = post["events"].toArray();
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

    idx_ = post["idx"].toInt();
    name_ = post["name"].toString();
    type_ = static_cast<PostType>(post["type"].toInt());
    point_idx_ = post["point_idx"].toInt();
    playerIdx_ = post["player_idx"].toString();
}

void Post::setVertex(Vertex &vertex) {
    vertex_ = &vertex;
}

Vertex &Post::vertex() {
    return *vertex_;
}
