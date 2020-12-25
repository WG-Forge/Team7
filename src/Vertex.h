    #ifndef POINTS_H
#define POINTS_H

#include "Post.h"

#include <QJsonObject>
#include <QVector2D>
#include <functional>
#include <stdexcept>

class Edge;

class Vertex{
    public:
        explicit Vertex(const QJsonObject &vertex);
        explicit Vertex(int idx, int postIdx, bool postIdxNull);
        explicit Vertex() = default;

        void addEdge(Edge &edge);
        void setPosition(const QVector2D &position);
        void setPost(Post &post);
        void setPostIdx(int idx) { postIdx_ = idx; };
        void setPriority(double newPriority) { priority_ = newPriority; };

        int idx();
        int postIdx();
        double priority() { return priority_; };
        bool isPostIdxNull();
        QVector2D position();
        std::vector<std::reference_wrapper<Edge>>& edges();
        Post& post();

    private:
        int idx_ = 0;
        int postIdx_ = 0;
        bool postIdxNull_ = false;
        QVector2D pos_;
        std::vector<std::reference_wrapper<Edge>> edges_;
        Post *post_ = nullptr;
        double priority_ = 0;
};

#endif // POINTS_H
