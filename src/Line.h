#ifndef LINE_H
#define LINE_H
#include "Point.h"

class Line{
public:
    Line(int idx, int length, Point &p1, Point &p2): idx(idx), length(length), point1(p1), point2(p2){};
private:
    int idx;
    int length;
    Point &point1;
    Point &point2;
};
#endif // LINE_H
