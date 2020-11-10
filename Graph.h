#ifndef GRAPH_H
#define GRAPH_H
#include <vector>
#include "Point.h"
#include "Line.h"
#include "QFile"
class Graph{
public:
    Graph() = default;
    Graph(FILE json);
    void calcCoords();
private:
    std::vector<Point> pints;
    std::vector<Line> lines;
};

#endif // GRAPH_H
