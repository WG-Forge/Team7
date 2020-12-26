#include "Events/HijackersAssault.h"

HijackersAssault::HijackersAssault(const QJsonObject &event){
    if(!event.contains("hijackers_power") || !event.contains("tick") || !event.contains("type")){
         throw std::invalid_argument("Wrong JSON graph format.");
    }
    hijackersPower_ = event["hijackers_power"].toInt();
    tick_ = event["tick"].toInt();
    type_ = static_cast<EventType>(event["type"].toInt());
}
