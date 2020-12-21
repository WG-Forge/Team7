#ifndef REFUGEESARRIVAL_H
#define REFUGEESARRIVAL_H

#include "Event.h"

class RefugeesArrival: public Event
{
public:
    explicit RefugeesArrival() = default;
    explicit RefugeesArrival(const QJsonObject &event);
    ~RefugeesArrival() = default;

private:
    enum EventType type_;
    int tick_;
    int refugees_number_;
};

#endif // REFUGEESARRIVAL_H
