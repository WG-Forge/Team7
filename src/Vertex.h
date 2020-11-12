#ifndef POINTS_H
#define POINTS_H

#include <QPointF>
#include <QJsonObject>

class Edge;

class Vertex{
    public:
        explicit Vertex(const QJsonObject &vertex);
        explicit Vertex(int idx, int postIdx, bool postIdxNull);

        void addEdge(Edge *edge);
        void setPosition(qreal x, qreal y);

        int idx();
        int postIdx();
        bool isPostIdxNull();
        std::vector<Edge*>& edges();

    private:
        int idx_ = 0;
        int postIdx_ = 0;
        bool postIdxNull_ = false;
        QPointF pos_;
        std::vector<Edge*> edges_;
};

#endif // POINTS_H
