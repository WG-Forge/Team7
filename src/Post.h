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

class Graph;
class Vertex;

class Post
{
public:
    explicit Post() = default;
    explicit Post(const QJsonObject &post);
    virtual ~Post() = default;

    void setVertex(Vertex &vertex);

    int idx() { return idx_; };
    int pointIdx() { return pointIdx_; };
    QString name() { return name_; };
    QString playerIdx() { return playerIdx_; };
    enum PostType type() { return type_; };
    std::vector<Event*>& events() { return events_; };
    Vertex& vertex();

protected:
    int idx_;
    int pointIdx_;
    Vertex *vertex_ = nullptr;
    QString name_;
    QString playerIdx_;
    enum PostType type_;
    std::vector<Event*> events_;
};

#endif // POST_H
