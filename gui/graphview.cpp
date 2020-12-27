#include "graphview.h"
#include "ui_graphview.h"
#include <QPainter>

GraphView::GraphView(QWidget *parent) : QWidget(parent), ui(new Ui::GraphView) {
    ui->setupUi(this);
}

GraphView::~GraphView() {
    delete ui;
}

void GraphView::setMap(std::shared_ptr<Map> m, Player *player, bool ggg) {
    if (ggg) {
        map_ = m;
        qDebug() << "MAP UPDATE";
    }
    player_ = player;
}

void GraphView::paintEvent(QPaintEvent *event) {
    ui->loadingLabel->setVisible(map_ == nullptr);

    if (player_) {
        QPainter painter(this);
        Graph& graph = map_->graph();
        float W = painter.device()->width();
        float H = painter.device()->height();
        float scale = std::min(W / 16 * 9, H) * 0.95;
        double trainSize = 12;
        float trainX;
        float trainY;
        float edgeX1, edgeX2, edgeY1, edgeY2;
        float partLength;
        int position = 0;

        painter.translate(W / 2, H / 2);

        for (auto &train : player_->trains()) {
            if (train->edge() != nullptr) {
                if (train->edge()->vertex1().idx() == train->currentVertex()->idx()) {
                    position = train->position();
                    edgeX1 = train->edge()->vertex1().position().x() * scale;
                    edgeY1 = train->edge()->vertex1().position().y() * scale;
                    edgeX2 = train->edge()->vertex2().position().x() * scale;
                    edgeY2 = train->edge()->vertex2().position().y() * scale;
                } else {
                    position = train->edge()->length() - train->position();
                    edgeX1 = train->edge()->vertex2().position().x() * scale;
                    edgeY1 = train->edge()->vertex2().position().y() * scale;
                    edgeX2 = train->edge()->vertex1().position().x() * scale;
                    edgeY2 = train->edge()->vertex1().position().y() * scale;
                }

                partLength = sqrt((pow(edgeX1 - edgeX2, 2) + pow(edgeY1 - edgeY2, 2))) / train->edge()->length();

                if (edgeY1 == edgeY2) {
                    if (edgeX1 < edgeX2) {
                        trainX = (edgeX1 + partLength * position);
                    } else trainX = (edgeX1 - partLength * position);

                    trainY = (edgeY1);
                } else if (edgeX1 == edgeX2) {
                    if (edgeY1 < edgeY2) {
                        trainY = (edgeY1 + partLength * position);
                    } else trainY = (edgeY1 - partLength * position);

                    trainX = (edgeX1);
                }

                QRect r(trainX - trainSize / 2, trainY - trainSize / 2, trainSize, trainSize);

                painter.setBrush(Qt::green);
                painter.drawEllipse(r);
                painter.setBrush(Qt::black);
                painter.setFont(QFont("Times", 11, QFont::Bold));
                painter.drawText(r, Qt::AlignCenter, QString::number(train->idx()));
            }
            update();
        }
        update();

//        qDebug() << "PLAYEEEEEEEEEEEEEEEEEEEEEEEEEER";
    }

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
            if (edge.idx() == 700) painter.setBrush(Qt::red);
            painter.setBrush(Qt::black);
            painter.drawLine(edge.vertex1().position().toPointF() * scale, edge.vertex2().position().toPointF() * scale);
            float posX1 = edge.vertex1().position().x() * scale;
            float posY1 = edge.vertex1().position().y() * scale;
            float posX2 = edge.vertex2().position().x() * scale;
            float posY2 = edge.vertex2().position().y() * scale;

            // отрисовка данных edge (помогает разбираться в путях для поезда)
            painter.drawText(posX1 + (posX2 - posX1) / 2, posY1 + (posY2 - posY1) / 2, QString::number(edge.idx())); // edge idx
//            painter.drawText(posX1 + (posX2 - posX1) / 2, posY1 + 15 + (posY2 - posY1) / 2, QString::number(edge.length())); // edge length
            painter.drawText(posX1 - 20 + 20 + (posX2 - posX1) / 2, posY1 + 10 + (posY2 - posY1) / 2, QString::number(edge.vertex1().idx())); // edge vertex 1
            painter.drawText(posX1 + 20 - 20 + (posX2 - posX1) / 2, posY1 - 10 + (posY2 - posY1) / 2, QString::number(edge.vertex2().idx())); // edge vertex 2
        }

        for (Vertex &vertex : graph.vertices()) {
            if (!vertex.isPostIdxNull()) {
                float posX = vertex.position().x() * scale;
                float posY = vertex.position().y() * scale;
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
