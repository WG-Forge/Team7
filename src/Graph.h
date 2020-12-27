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

//struct VertexStr {
//    Vertex *vertex;
//    int vertexIdx;
//};

//struct EdgeStr {
//    Edge *edge;
//    int edgeLength
//};

class Graph {
    public:
        explicit Graph() = default;
        explicit Graph(const QJsonObject &graph, Map &map);

        void calcCoords(float aspectRatio, const QJsonObject coordsData);
        void setCoords(const QJsonObject coords);

        int minVertexIdx() { return minVertexIdx_; };
        std::vector<Vertex>& vertices();
        std::vector<Edge>& edges();
        std::map<int,int>& idx();
        std::map<int,int>& idxEdges();
        Vertex& vertex(int idx);

        std::vector<std::vector<int>> matrixOrig() { return matrixOrig_; };         // дефолтная матрица смежностей
        std::vector<std::vector<int>> matrixChanged() { return matrixChanged_; };   // изменённая (при создании такая же как и дефолтная)

        void restoreMatrix() {
            int index = 0;
            for (auto &t : matrixOrig_) {
                matrixChanged_[index].clear();
                matrixChanged_[index] = t;
                index++;
            }
//            std::copy(&matrixOrig_[0][0], &matrixOrig_[0][0] + matrixOrig_.size() * matrixOrig_[0].size(), &matrixChanged_[0][0]); // восстановить матрицу
        }
        int lengthBetween(int v1, int v2) { return matrixChanged_[v1 - minVertexIdx_][v2 - minVertexIdx_]; }; //  расстояние между двумя вертексами
        void setNewLength(int v1, int v2, int newLength) { matrixChanged_[v1 - minVertexIdx_][v2 - minVertexIdx_] = newLength; }; // изменить расстояние между двумя вертексами

    private:
        std::vector<Vertex> vertices_;
        std::map<int, int> idx_;
        std::map<int, int> idxEdges_;
        std::vector<Edge> edges_;
        std::map<int, std::reference_wrapper<Vertex>> verticesMap_;

        int minVertexIdx_ = 0;
        bool isSelfIntersecting();
        void placeVertices(float W, float H);
        void fitToSize(float W, float H);
        std::vector<std::vector<int>> matrixOrig_;
        std::vector<std::vector<int>> matrixChanged_;
};

#endif // GRAPH_H
