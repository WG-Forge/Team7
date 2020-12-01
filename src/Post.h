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
    int idx();
    QString name();
    enum PostType type();
    std::vector<Event*>& events();

private:
    int idx_;
    QString name_;
    enum PostType type_;
    std::vector<Event*> events_;


};

#endif // POST_H
