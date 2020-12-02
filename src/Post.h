#ifndef POST_H
#define POST_H

#include "Event.h"
#include "Events/HijackersAssault.h"
#include "Events/RefugeesArrival.h"
#include "Events/TrainCollision.h"
#include "Events/ParasitesAssault.h"
#include "Enums/PostType.h"

#include <QJsonObject>
#include <QJsonArray>
#include <stdexcept>

class Post
{
public:
    explicit Post() = default;
    virtual ~Post() = default;
    virtual int idx() = 0;
    virtual QString name() = 0;
    virtual enum PostType type() = 0;
    virtual std::vector<Event*>& events() = 0;
};

#endif // POST_H
