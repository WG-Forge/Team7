#ifndef GRAPH_H
#define GRAPH_H

#include "Vertex.h"
#include "Edge.h"
#include <QJsonObject>

class Graph{
    public:
        explicit Graph(const QJsonObject &graph);

        void print();
        void calcCoords();

        std::vector<Vertex>& vertices();
        std::vector<Edge>& edges();

    private:
        std::vector<Vertex> vertices_;
        std::vector<Edge> edges_;
};

#endif // GRAPH_H
