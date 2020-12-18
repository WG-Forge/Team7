#include "Events/RefugeesArrival.h"

RefugeesArrival::RefugeesArrival(const QJsonObject &event){
    if(!event.contains("refugees_number") || !event.contains("tick") || !event.contains("type")){
         throw std::invalid_argument("Wrong JSON graph format.");
    }
    refugeesNumber_ = event["refugees_number"].toInt();
    tick_ = event["tick"].toInt();
    type_ = static_cast<EventType>(event["type"].toInt());
}
