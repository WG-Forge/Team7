#ifndef TRAINCOLLISION_H
#define TRAINCOLLISION_H

#include "Event.h"

class TrainCollision: public Event
{
public:
    explicit TrainCollision() = default;
    explicit TrainCollision(const QJsonObject &event);
    ~TrainCollision() = default;
    int train(){return train_;};

private:
    enum EventType type_;
    int tick_;
    int train_;
};

#endif // TRAINCOLLISION_H
