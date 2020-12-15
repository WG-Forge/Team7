#include "Graph.h"
#include "Map.h"

Graph::Graph(const QJsonObject &graph, Map &map) {
    if (!graph.contains("lines") || !graph["lines"].isArray() || !graph.contains("points") || !graph["points"].isArray())
        throw std::invalid_argument("Wrong JSON graph format.");

    QJsonArray edgesJsonArray = graph["lines"].toArray();
    QJsonArray verticesJsonArray = graph["points"].toArray();

    for (auto const &vertex : verticesJsonArray) {
        if (!vertex.isObject())
            throw std::invalid_argument("Wrong JSON graph format.");
        vertices_.emplace_back(vertex.toObject());

    }
    int i = 0;
    for (Vertex &v : vertices_) {
        idx_.emplace(v.idx(),i);
        i++;
        verticesMap_.emplace(v.idx(), v);
        if (v.isPostIdxNull()) continue;

        for (Post &post : map.posts()) {
            if (post.point_idx() == v.idx() && post.idx() == v.postIdx()) {
                v.setPost(post);
                post.setVertex(v);
                continue;
            }
        }
    }

    for (auto const &edge : edgesJsonArray) {
        if (!edge.isObject())
            throw std::invalid_argument("Wrong JSON graph format.");

        edges_.emplace_back(edge.toObject(), verticesMap_);
    }

    for (Edge &e : edges_) {
        e.vertex1().addEdge(e);
        e.vertex2().addEdge(e);
    }
}

std::vector<Vertex>& Graph::vertices() {
    return vertices_;
}

std::vector<Edge>& Graph::edges() {
    return edges_;
}

Vertex &Graph::vertex(int idx) {
    return verticesMap_.at(idx);
}


void Graph::setCoords(const QJsonObject coordsData) {
    if (!coordsData.contains("coordinates") || !coordsData["coordinates"].isArray())
        throw std::invalid_argument("Wrong JSON graph format.");

    QJsonArray coordsJsonArray = coordsData["coordinates"].toArray();
    for (Vertex &vertex : vertices_) {
        for (auto const &coords : coordsJsonArray) {
            QJsonObject coord = coords.toObject();

            if (coord["idx"].toInt() == vertex.idx()) {
                int coordX = coord["x"].toInt(), coordY = coord["y"].toInt();
                vertex.setPosition(QVector2D(coordX, coordY));
                continue;
            }
        }
    }
}

void Graph::calcCoords(float aspectRatio, const QJsonObject coordsData) {
    const float W = aspectRatio, H = 1;
    const int placeMaxAttempts = 8;

    for (int i = 0; i < placeMaxAttempts; ++i) {
        placeVertices(W, H);

        if (!isSelfIntersecting())
            break;

        if (i >= placeMaxAttempts - 1)
            qWarning("Warning: Can't create non-self-intersecting graph layout. Graph is probably non planar");
    }

   this->setCoords(coordsData);
    fitToSize(W, H);
}

void Graph::placeVertices(float W, float H) {
    const float iterations = 400;
    const float k = 0.2 * sqrt(W * H / vertices_.size());
    const float initialT = W / 10;

    float t = initialT;

    float angle = 0;
    for (Vertex &v : vertices_) {
        v.setPosition(QVector2D(W / 2 * sin(angle), H / 2 * cos(angle)));
        angle += 2 * M_PI / vertices_.size();
    }

    for (int i = 0; i < iterations; ++i) {
        for (Vertex &v : vertices_) {
            QVector2D disp(0, 0);

            for (Vertex &u : vertices_) {
                if (&u != &v) {
                    QVector2D delta(v.position() - u.position());
                    disp += delta.normalized() * k * k / delta.length();
                }
            }

            for (Edge &e : v.edges()) {
                Vertex &u = (&e.vertex1() != &v) ? e.vertex1() : e.vertex2();

                QVector2D delta(v.position() - u.position());
                disp -= delta.normalized() * delta.length() * delta.length() / k;
            }

            v.setPosition(v.position() + disp.normalized() * std::min(disp.length(), t));
        }

        t -= initialT / iterations;
    }
}

bool Graph::isSelfIntersecting() {
    std::vector<QLineF> edgeLines;
    edgeLines.reserve(edges_.size());

    for (Edge &e : edges_) {
        edgeLines.emplace_back(e.vertex1().position().toPointF(), e.vertex2().position().toPointF());
    }

    for (int i = 0; i < edges_.size(); ++i) {
        for (int j = i + 1; j < edges_.size(); ++j) {
            if (&edges_[i].vertex1() == &edges_[j].vertex1() || &edges_[i].vertex1() == &edges_[j].vertex2())
                continue;

            if (&edges_[i].vertex2() == &edges_[j].vertex1() || &edges_[i].vertex2() == &edges_[j].vertex2())
                continue;

            if (edgeLines[i].intersects(edgeLines[j], nullptr) == QLineF::BoundedIntersection)
                return true;
        }
    }

    return false;
}

void Graph::fitToSize(float W, float H) {
    float minX = FLT_MAX;
    float minY = FLT_MAX;
    float maxX = -FLT_MAX;
    float maxY = -FLT_MAX;

    for (Vertex &v : vertices_) {
        minX = std::min(minX, v.position().x());
        minY = std::min(minY, v.position().y());
        maxX = std::max(maxX, v.position().x());
        maxY = std::max(maxY, v.position().y());
    }

    QVector2D center((minX + maxX) / 2, (minY + maxY) / 2);
    float dx = maxX - minX;
    float dy = maxY - minY;
    float scale = (dx / dy < W / H) ? (H / dy) : (W / dx);

    for (Vertex &v : vertices_) {
        v.setPosition((v.position() - center) * scale);
    }
}

std::map<int,int>& Graph::idx(){
    return idx_;
}

