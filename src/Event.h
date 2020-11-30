#ifndef EVENT_H
#define EVENT_H

#include "Enums/EventType.h"

#include <QJsonObject>

class Event
{
public:
    explicit Event() = default;
    explicit Event(const QJsonObject& event);

private:
    enum EventType event_;
};

#endif // EVENT_H
