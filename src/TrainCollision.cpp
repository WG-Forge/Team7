#include "Events/TrainCollision.h"

TrainCollision::TrainCollision(const QJsonObject &event){
    if(!event.contains("train") || !event.contains("tick") || !event.contains("type")){
         throw std::invalid_argument("Wrong JSON graph format.");
    }
    train_ = event["train"].toInt();
    tick_ = event["tick"].toInt();
    type_ = static_cast<EventType>(event["type"].toInt());
}
