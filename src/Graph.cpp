#include "Graph.h"
#include "rand.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <iostream>
#include <exception>
#include <QVector2D>
#include <algorithm>

Graph::Graph(const QJsonObject &graph) {
    if (!graph.contains("lines") || !graph["lines"].isArray() || !graph.contains("points") || !graph["points"].isArray())
        throw std::invalid_argument("Wrong JSON graph format.");

    QJsonArray edgesJsonArray = graph["lines"].toArray();
    QJsonArray verticesJsonArray = graph["points"].toArray();
    QMap<int, Vertex*> verticesMap;

    for (auto vertex : verticesJsonArray) {
        if (!vertex.isObject())
            throw std::invalid_argument("Wrong JSON graph format.");

        vertices_.emplace_back(vertex.toObject());
    }

    for (Vertex &v : vertices_)
        verticesMap[v.idx()] = &v;

    for (auto edge : edgesJsonArray) {
        if (!edge.isObject())
            throw std::invalid_argument("Wrong JSON graph format.");

        edges_.emplace_back(edge.toObject(), verticesMap);
    }

    for (Edge &e : edges_) {
        e.vertex1().addEdge(&e);
        e.vertex2().addEdge(&e);
    }
}

void Graph::print() {
    std::cout << "Vertices: " << std::endl;
    for (auto &v : vertices_) {
        std::cout << "idx: " << v.idx() << " degree: " << v.edges().size() << std::endl;
    }

    std::cout << std::endl << std::endl << "Edges:" << std::endl;
    for (auto &e : edges_) {
        std::cout << "1: " << e.vertex1().idx() << " 2: " << e.vertex2().idx() << std::endl;
    }
}

std::vector<Vertex>& Graph::vertices() {
    return vertices_;
}

std::vector<Edge>& Graph::edges() {
    return edges_;
}

void Graph::calcCoords() {
    const float W = 1800, H = 950;
    const float iterations = 400;
    const float coolK = 1.02;

    const float k = 0.2 * sqrt(W * H / vertices_.size());
    float t = W / 10;
    std::map<Vertex*, QVector2D> disp;

    for (Vertex &v : vertices_) {
        v.setPosition(QPointF(Rand::doubleInRange(W / 4, W * 3 / 4), Rand::doubleInRange(H / 4, H * 3 / 4)));
    }

    for (int i = 0; i < iterations; ++i) {
        for (Vertex &v : vertices_) {
            disp[&v] = QVector2D(0, 0);

            for (Vertex &u : vertices_) {
                if (&u != &v) {
                    QVector2D delta(v.position() - u.position());
                    disp[&v] += delta.normalized() * k * k / delta.length();
                }
            }
        }

        for (Edge &e : edges_) {
            Vertex &v = e.vertex1();
            Vertex &u = e.vertex2();
            QVector2D delta(v.position() - u.position());

            disp[&v] -= delta.normalized() * delta.length() * delta.length() / k;
            disp[&u] += delta.normalized() * delta.length() * delta.length() / k;
        }

        for (Vertex &v : vertices_) {
            v.setPosition(v.position() + disp[&v].normalized().toPointF() * std::min(disp[&v].length(), t));
            v.setPosition(QPointF(std::max(0.0f, std::min(W, (float) v.position().x())), std::max(0.0f, std::min(H, (float) v.position().y()))));
        }

        t /= coolK;
    }
}
