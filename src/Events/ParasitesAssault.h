#ifndef PARASITESASSAULT_H
#define PARASITESASSAULT_H

#include "Event.h"

class ParasitesAssault: public Event
{
public:
    explicit ParasitesAssault() = default;
    explicit ParasitesAssault(const QJsonObject &event);
    ~ParasitesAssault() = default;

private:
    enum EventType type_;
    int tick_;
    int parasitesPower_;
};

#endif // PARASITESASSAULT_H
