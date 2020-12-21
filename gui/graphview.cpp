#include "graphview.h"
#include "ui_graphview.h"
#include <QPainter>

GraphView::GraphView(QWidget *parent) : QWidget(parent), ui(new Ui::GraphView) {
    ui->setupUi(this);
}

GraphView::~GraphView() {
    delete ui;
}

void GraphView::setMap(std::shared_ptr<Map> m, Player &player) {
    map_ = m;
    player_ = &player;
}

void GraphView::paintEvent(QPaintEvent *event) {
    ui->loadingLabel->setVisible(map_ == nullptr);

    if (map_) {
        QPainter painter(this);
        Graph& graph = map_->graph();
        float W = painter.device()->width();
        float H = painter.device()->height();
        float scale = std::min(W / 16 * 9, H) * 0.95;
        double circleSize = 6;
        double userCircleSize = 10;
        double textRectMargin = 2;

        painter.translate(W / 2, H / 2);

        for (Edge &edge : graph.edges()) {
            painter.drawLine(edge.vertex1().position().toPointF() * scale, edge.vertex2().position().toPointF() * scale);
            int posX1 = edge.vertex1().position().x() * scale;
            int posY1 = edge.vertex1().position().y() * scale;
            int posX2 = edge.vertex2().position().x() * scale;
            int posY2 = edge.vertex2().position().y() * scale;

            // отрисовка данных edge (помогает разбираться в путях для поезда)
            painter.drawText(posX1 + (posX2 - posX1) / 2, posY1 + (posY2 - posY1) / 2, QString::number(edge.idx())); // edge idx
            painter.drawText(posX1 + (posX2 - posX1) / 2, posY1 + 15 + (posY2 - posY1) / 2, QString::number(edge.length())); // edge length
            painter.drawText(posX1 - 20 + (posX2 - posX1) / 2, posY1 + (posY2 - posY1) / 2, QString::number(edge.vertex1().idx())); // edge vertex 1
            painter.drawText(posX1 + 20 + (posX2 - posX1) / 2, posY1 + (posY2 - posY1) / 2, QString::number(edge.vertex2().idx())); // edge vertex 2
        }

        for (Vertex &vertex : graph.vertices()) {
            if (!vertex.isPostIdxNull()) {
                int posX = vertex.position().x() * scale;
                int posY = vertex.position().y() * scale;
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
                    default:
                        painter.setBrush(Qt::white);
                }

                if (vertex.post().playerIdx() == player_->idx()) {
                    painter.setBrush(Qt::yellow);
                    painter.drawEllipse(posX - userCircleSize / 2, posY - userCircleSize / 2, userCircleSize, userCircleSize);
                    continue;
                }

                painter.drawEllipse(posX - circleSize / 2, posY - circleSize / 2, circleSize, circleSize);
            }
        }

        QFontMetrics metrics(painter.font());
        painter.setBrush(Qt::white);
        for (Vertex &vertex : graph.vertices()) {
            if (!vertex.isPostIdxNull()) {
                QRectF boundingRect(metrics.boundingRect(vertex.post().name()));
                boundingRect.setTopLeft(boundingRect.topLeft() - QPointF(textRectMargin, textRectMargin));
                boundingRect.setSize(boundingRect.size() + QSize(textRectMargin, textRectMargin));

                painter.save();
                painter.translate(vertex.position().toPointF() * scale + QPointF(circleSize / 2, -circleSize / 2) - boundingRect.bottomLeft());
                painter.drawRect(boundingRect);
                painter.drawText(0, 0, vertex.post().name());
                painter.drawText(0, -20, QString::number(vertex.post().vertex().idx()));
                painter.restore();
            }
        }
    }
}
