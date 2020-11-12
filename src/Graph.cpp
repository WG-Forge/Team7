#include "Graph.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <iostream>
#include <exception>

Graph::Graph(const QJsonObject &graph) {
    if (!graph.contains("lines") || !graph["lines"].isArray() || !graph.contains("points") || !graph["points"].isArray())
        throw std::invalid_argument("Wrong JSON graph format.");

    QJsonArray edgesJsonArray = graph["lines"].toArray();
    QJsonArray verticesJsonArray = graph["points"].toArray();
    QMap<int, Vertex*> verticesMap;

    for (auto vertex : verticesJsonArray) {
        if (!vertex.isObject())
            throw std::invalid_argument("Wrong JSON graph format.");

        vertices.emplace_back(vertex.toObject());
    }

    for (Vertex &v : vertices)
        verticesMap[v.idx()] = &v;

    for (auto edge : edgesJsonArray) {
        if (!edge.isObject())
            throw std::invalid_argument("Wrong JSON graph format.");

        edges.emplace_back(edge.toObject(), verticesMap);
    }

    for (Edge &e : edges) {
        e.vertex1().addEdge(&e);
        e.vertex2().addEdge(&e);
    }
}

void Graph::print() {
    std::cout << "Vertices: " << std::endl;
    for (auto &v : vertices) {
        std::cout << "idx: " << v.idx() << " degree: " << v.edges().size() << std::endl;
    }

    std::cout << std::endl << std::endl << "Edges:" << std::endl;
    for (auto &e : edges) {
        std::cout << "1: " << e.vertex1().idx() << " 2: " << e.vertex2().idx() << std::endl;
    }
}

void Graph::calcCoords(){
}
