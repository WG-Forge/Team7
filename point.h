#ifndef POINTS_H
#define POINTS_H
#include <QPointF>

class Point{
public:
    Point() = default;
    Point(int idx,int post_idx): idx(idx), post_idx(post_idx), pos(){};
    void SetPosition(int x, int y);
private:
    int idx;
    int post_idx;
    QPointF pos;
};

#endif // POINTS_H
