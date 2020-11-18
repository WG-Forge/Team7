#include "graphview.h"
#include <QPainter>
#include <iostream>

GraphView::GraphView(QWidget *parent) : QWidget(parent) {

}

void GraphView::setGraph(Graph *graph) {
    graph_ = graph;
}

void GraphView::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    for (Edge &edge : graph_->edges()) {
        painter.drawLine(edge.vertex1().position().toPointF(), edge.vertex2().position().toPointF());
    }
}
