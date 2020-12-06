#include "graphview.h"
#include <QPainter>
#include <iostream>

GraphView::GraphView(QWidget *parent) : QWidget(parent) {

}

void GraphView::setGraph(std::unique_ptr<Graph> graph) {
    graph_ = std::move(graph);
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

        for (Vertex &vertex : graph_->vertices()) {
            int posX = vertex.position().x() * scale;
            int posY = vertex.position().y() * scale;
            int rectSizeW = 7, rectSizeH = 13;
            double circleSize = 6;
            enum PostType type = vertex.post().type();

            switch(type) {
            case PostType::TOWN:
                painter.setBrush(Qt::green);
                break;
            case PostType::STORAGE:
                painter.setBrush(Qt::blue);
                break;
            case PostType::MARKET:
                painter.setBrush(Qt::red);
                break;
            }

            painter.drawEllipse(posX - circleSize / 2, posY - circleSize / 2, circleSize, circleSize);

            if (vertex.isPostIdxNull()) continue;

            int nameLength = vertex.post().name().size();

            painter.setBrush(Qt::white);
            painter.drawRect(posX - 2 - (rectSizeW * nameLength)/2,  posY - 20, rectSizeW * nameLength, rectSizeH);
            painter.drawText(posX - (rectSizeW * nameLength)/2, posY - 10, vertex.post().name());
//            qDebug() << painter.font();
        }
    }
}
