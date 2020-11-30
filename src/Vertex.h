#ifndef POINTS_H
#define POINTS_H

#include <QJsonObject>
#include <QVector2D>
#include <functional>
#include <stdexcept>

class Edge;

class Vertex{
    public:
        explicit Vertex(const QJsonObject &vertex);
        explicit Vertex(int idx, int postIdx, bool postIdxNull);

        void addEdge(Edge &edge);
        void setPosition(const QVector2D &position);

        int idx();
        int postIdx();
        bool isPostIdxNull();
        QVector2D position();
        std::vector<std::reference_wrapper<Edge>>& edges();

    private:
        int idx_ = 0;
        int postIdx_ = 0;
        bool postIdxNull_ = false;
        QVector2D pos_;
        std::vector<std::reference_wrapper<Edge>> edges_;
};

#endif // POINTS_H
