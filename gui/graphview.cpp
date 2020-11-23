#include "graphview.h"
#include <QPainter>
#include <iostream>

GraphView::GraphView(QWidget *parent) : QWidget(parent) {

}

void GraphView::setGraph(Graph *graph) {
    graph_ = graph;
}

void GraphView::paintEvent(QPaintEvent *event) {
    if (graph_) {
        QPainter painter(this);
        float W = painter.device()->width();
        float H = painter.device()->height();
        float scale = std::min(W / 16 * 9, H) * 0.95;

        painter.translate(W / 2, H / 2);

        for (Edge &edge : graph_->edges()) {
            painter.drawLine(edge.vertex1().position().toPointF() * scale, edge.vertex2().position().toPointF() * scale);
        }
    }
}
