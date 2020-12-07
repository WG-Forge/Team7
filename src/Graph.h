#ifndef GRAPH_H
#define GRAPH_H

#include "Vertex.h"
#include "Edge.h"
#include "Post.h"

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

class Map;

class Graph {
    public:
        explicit Graph() = default;
        explicit Graph(const QJsonObject &graph, Map &map);

        void calcCoords(float aspectRatio, const QJsonObject coordsData);
        void setCoords(const QJsonObject coords);

        std::vector<Vertex>& vertices();
        std::vector<Edge>& edges();
        Vertex& vertex(int idx);

    private:
        std::vector<Vertex> vertices_;
        std::vector<Edge> edges_;
        std::map<int, std::reference_wrapper<Vertex>> verticesMap_;

        bool isSelfIntersecting();
        void placeVertices(float W, float H);
        void fitToSize(float W, float H);
};

#endif // GRAPH_H
