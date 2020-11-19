#include "Vertex.h"
#include "Edge.h"
#include <stdexcept>
Vertex::Vertex(const QJsonObject &vertex) {
    if (!vertex.contains("idx") || !vertex["idx"].isDouble() || !vertex.contains("post_idx") || !(vertex["post_idx"].isNull() || vertex["post_idx"].isDouble()))
        throw std::invalid_argument("Wrong JSON graph format.");

    idx_ = vertex["idx"].toInt();

    if (vertex["post_idx"].isNull())
        postIdxNull_ = true;
    else
        postIdx_ = vertex["post_idx"].toInt();
}

Vertex::Vertex(int idx, int postIdx, bool postIdxNull) : idx_(idx), postIdx_(postIdx), postIdxNull_(postIdxNull), pos_() {};

void Vertex::addEdge(Edge *edge) {
    edges_.push_back(edge);
}

void Vertex::setPosition(const QVector2D &position) {
    pos_ = position;
}

int Vertex::idx() {
    return idx_;
}

int Vertex::postIdx() {
    return postIdx_;
}

bool Vertex::isPostIdxNull() {
    return postIdxNull_;
}

QVector2D Vertex::position() {
    return pos_;
}

std::vector<Edge*>& Vertex::edges() {
    return edges_;
}
