#ifndef GRAPH_H
#define GRAPH_H

#include "Vertex.h"
#include "Edge.h"

#include <QJsonObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <map>
#include <iostream>
#include <exception>
#include <QVector2D>
#include <algorithm>
#include <QLineF>
#include <QtMath>

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
