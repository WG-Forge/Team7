#ifndef GRAPH_H
#define GRAPH_H

#include "Vertex.h"
#include "Edge.h"
#include <QJsonObject>

class Graph{
    public:
        explicit Graph(const QJsonObject &graph);

        void calcCoords(float aspectRatio);

        std::vector<Vertex>& vertices();
        std::vector<Edge>& edges();

    private:
        std::vector<Vertex> vertices_;
        std::vector<Edge> edges_;

        bool isSelfIntersecting();
        void placeVertices(float W, float H);
        void fitToSize(float W, float H);
};

#endif // GRAPH_H
