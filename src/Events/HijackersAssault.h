#ifndef HIJACKERSASSAULT_H
#define HIJACKERSASSAULT_H

#include "Event.h"

class HijackersAssault: public Event
{
public:
    explicit HijackersAssault() = default;
    explicit HijackersAssault(const QJsonObject &event);
    ~HijackersAssault() = default;

private:
    enum EventType type_;
    int tick_;
    int hijackers_power_;
};

#endif // HIJACKERSASSAULT_H
