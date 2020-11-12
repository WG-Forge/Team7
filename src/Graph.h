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

    private:
        std::vector<Vertex> vertices;
        std::vector<Edge> edges;
};

#endif // GRAPH_H
