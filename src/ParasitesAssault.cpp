#include "Events/ParasitesAssault.h"

ParasitesAssault::ParasitesAssault(const QJsonObject &event){
    if(!event.contains("parasites_power") || !event.contains("tick") || !event.contains("type")){
         throw std::invalid_argument("Wrong JSON graph format.");
    }
    parasites_power_ = event["parasites_power"].toInt();
    tick_ = event["tick"].toInt();
    type_ = static_cast<EventType>(event["type"].toInt());
}

