#include "Edge.h"
#include <QJsonArray>
#include <stdexcept>

Edge::Edge(const QJsonObject &edge, const QMap<int, Vertex*> &vertices) {
    if (!edge.contains("idx") || !edge["idx"].isDouble() || !edge.contains("length") || !edge["length"].isDouble() || !edge.contains("points") || !edge["points"].isArray())
        throw std::invalid_argument("Wrong JSON graph format.");

    idx_ = edge["idx"].toInt();
    length_ = edge["length"].toInt();

    QJsonArray v = edge["points"].toArray();
    if (v.size() != 2 || !v[0].isDouble() || !v[1].isDouble())
        throw std::invalid_argument("Wrong JSON graph format.");

    int v1 = v[0].toInt();
    int v2 = v[1].toInt();
    if (!vertices.contains(v1) || !vertices.contains(v2))
        throw std::invalid_argument("Wrong JSON graph format.");

    vertex1_ = vertices[v1];
    vertex2_ = vertices[v2];
}

Edge::Edge(int idx, int length, Vertex &v1, Vertex &v2) : idx_(idx), length_(length), vertex1_(&v1), vertex2_(&v2){}

int Edge::idx() {
    return idx_;
}

int Edge::length() {
    return length_;
}

Vertex& Edge::vertex1() {
    return *vertex1_;
}

Vertex& Edge::vertex2() {
    return *vertex2_;
}

