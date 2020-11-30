#include "Event.h"

Event::Event(const QJsonObject& event){
    event_ = static_cast<EventType>(event["type"].toInt());
}
