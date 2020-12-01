#ifndef PARASITESASSAULT_H
#define PARASITESASSAULT_H

#include "src/Event.h"

class ParasitesAssault: public Event
{
public:
    explicit ParasitesAssault() = default;
    explicit ParasitesAssault(const QJsonObject &event);
    ~ParasitesAssault() = default;

private:
    enum EventType type_;
    int tick_;
    int parasites_power_;
};

#endif // PARASITESASSAULT_H
