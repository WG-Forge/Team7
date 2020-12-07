#include "graphview.h"
#include "ui_graphview.h"
#include <QPainter>

GraphView::GraphView(QWidget *parent) : QWidget(parent), ui(new Ui::GraphView) {
    ui->setupUi(this);
}

GraphView::~GraphView() {
    delete ui;
}

void GraphView::setMap(std::unique_ptr<Map> m) {
    map_ = std::move(m);
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
        double textRectMargin = 2;

        painter.translate(W / 2, H / 2);

        for (Edge &edge : graph.edges()) {
            painter.drawLine(edge.vertex1().position().toPointF() * scale, edge.vertex2().position().toPointF() * scale);
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
                painter.restore();
            }
        }
    }
}
