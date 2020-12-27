#include "graphview.h"
#include "ui_graphview.h"
#include <QPainter>

GraphView::GraphView(QWidget *parent) : QWidget(parent), ui(new Ui::GraphView) {
    ui->setupUi(this);

    ui->trainObject_1->setAlignment(Qt::AlignCenter);
    ui->trainObject_2->setAlignment(Qt::AlignCenter);
    ui->trainObject_3->setAlignment(Qt::AlignCenter);
    ui->trainObject_4->setAlignment(Qt::AlignCenter);
    ui->trainObject_5->setAlignment(Qt::AlignCenter);
    ui->trainObject_6->setAlignment(Qt::AlignCenter);
    ui->trainObject_7->setAlignment(Qt::AlignCenter);
    ui->trainObject_8->setAlignment(Qt::AlignCenter);
    ui->trainObject_9->setAlignment(Qt::AlignCenter);
    ui->trainObject_10->setAlignment(Qt::AlignCenter);
    ui->trainObject_11->setAlignment(Qt::AlignCenter);
    ui->trainObject_12->setAlignment(Qt::AlignCenter);
    ui->trainObject_13->setAlignment(Qt::AlignCenter);
    ui->trainObject_14->setAlignment(Qt::AlignCenter);
    ui->trainObject_15->setAlignment(Qt::AlignCenter);
    ui->trainObject_16->setAlignment(Qt::AlignCenter);

    ui->trainObject_1->hide();
    ui->trainObject_2->hide();
    ui->trainObject_3->hide();
    ui->trainObject_4->hide();
    ui->trainObject_5->hide();
    ui->trainObject_6->hide();
    ui->trainObject_7->hide();
    ui->trainObject_8->hide();
    ui->trainObject_9->hide();
    ui->trainObject_10->hide();
    ui->trainObject_11->hide();
    ui->trainObject_12->hide();
    ui->trainObject_13->hide();
    ui->trainObject_14->hide();
    ui->trainObject_15->hide();
    ui->trainObject_16->hide();
}

GraphView::~GraphView() {
    delete ui;
}

void GraphView::setMap(std::shared_ptr<Map> m, Player *player, bool ggg) {
    if (ggg) {
        map_ = m;
        qDebug() << "MAP UPDATE";
        isMap = ggg;
    } else isMap = !ggg;
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
        double trainSize = 16;
        float X1, X2, Y1, Y2, trainX, trainY;
        float partLength;
        int position = 0;
        QString enemyStyleSheet = "border-radius: 8px; background-color: red; color: black; font-weight: 600; text-align: center; border: 1px solid black;";
        QString playerStyleSheet = "border-radius: 8px; background-color: lightgreen; color: black; font-weight: 600; text-align: center; border: 1px solid black;";

        painter.translate(W / 2, H / 2);

        for (auto &train : player_->trains()) {
            QLabel *currentLabel;

            if (train->idx() == 1) currentLabel = ui->trainObject_1;
            if (train->idx() == 2) currentLabel = ui->trainObject_2;
            if (train->idx() == 3) currentLabel = ui->trainObject_3;
            if (train->idx() == 4) currentLabel = ui->trainObject_4;
            if (train->idx() == 5) currentLabel = ui->trainObject_5;
            if (train->idx() == 6) currentLabel = ui->trainObject_6;
            if (train->idx() == 7) currentLabel = ui->trainObject_7;
            if (train->idx() == 8) currentLabel = ui->trainObject_8;
            if (train->idx() == 9) currentLabel = ui->trainObject_9;
            if (train->idx() == 10) currentLabel = ui->trainObject_10;
            if (train->idx() == 11) currentLabel = ui->trainObject_11;
            if (train->idx() == 12) currentLabel = ui->trainObject_12;
            if (train->idx() == 13) currentLabel = ui->trainObject_13;
            if (train->idx() == 14) currentLabel = ui->trainObject_14;
            if (train->idx() == 15) currentLabel = ui->trainObject_15;
            if (train->idx() == 16) currentLabel = ui->trainObject_16;


            for (auto &edge : map_.get()->graph().edges()) {
                if (train->lineIdx() == edge.idx()) {
                    Vertex v1 = edge.vertex1();
                    Vertex v2 = edge.vertex2();

                    X1 = v1.position().x() * scale;
                    Y1 = v1.position().y() * scale;
                    X2 = v2.position().x() * scale;
                    Y2 = v2.position().y() * scale;

                    partLength = sqrt((pow(X1 - X2, 2) + pow(Y1 - Y2, 2))) / edge.length();
                    position = train->position();

                    if (Y1 == Y2) {
                        trainX = X1 + partLength * position;
                        trainY = Y1;
                    } else {
                        trainX = X1;
                        trainY = Y1 + partLength * position;
                    }
                }
            }

            QRect r1(trainX + W/2 - trainSize / 2, trainY + H/2 - trainSize / 2, trainSize, trainSize);

            if (train->playerIdx() != player_->idx()) currentLabel->setStyleSheet({enemyStyleSheet});
            else currentLabel->setStyleSheet({playerStyleSheet});

            currentLabel->setGeometry(r1);
            currentLabel->setText(QString::number(train->level()));

            if (isMap) currentLabel->show();
        }
    }

    if (!isMap) return;

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
//            painter.drawText(posX1 + (posX2 - posX1) / 2, posY1 + (posY2 - posY1) / 2, QString::number(edge.idx())); // edge idx
//            painter.drawText(posX1 + (posX2 - posX1) / 2, posY1 + 15 + (posY2 - posY1) / 2, QString::number(edge.length())); // edge length
//            painter.drawText(posX1 - 20 + 20 + (posX2 - posX1) / 2, posY1 + 10 + (posY2 - posY1) / 2, QString::number(edge.vertex1().idx())); // edge vertex 1
//            painter.drawText(posX1 + 20 - 20 + (posX2 - posX1) / 2, posY1 - 10 + (posY2 - posY1) / 2, QString::number(edge.vertex2().idx())); // edge vertex 2
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
        for (auto &town : map_->towns()) {
            QRectF boundingRect(metrics.boundingRect(town.name()));
//            QRectF boundingRect2(metrics.boundingRect(town.vertex().post().name()));
            boundingRect.setTopLeft(boundingRect.topLeft() - QPointF(textRectMargin, textRectMargin));
            boundingRect.setSize(boundingRect.size() + QSize(textRectMargin, textRectMargin));

            painter.save();
            painter.translate(town.vertex().position().toPointF() * scale + QPointF(circleSize / 2, -circleSize / 2) - boundingRect.bottomLeft());
            painter.drawRect(boundingRect);
            painter.drawText(0, 0, town.vertex().post().name());

            painter.restore();
        }

        for (auto &market : map_->markets()) {
            QRectF boundingRect(metrics.boundingRect(market.name()));
            QRectF boundingRect2(metrics.boundingRect(QString::number(market.product())));
            boundingRect.setTopLeft(boundingRect.topLeft() - QPointF(textRectMargin, textRectMargin));
            boundingRect.setSize(boundingRect.size() + QSize(textRectMargin, textRectMargin));
            boundingRect2.setTopLeft(boundingRect2.topLeft() - QPointF(textRectMargin, textRectMargin));
            boundingRect2.setSize(boundingRect2.size() + QSize(textRectMargin, textRectMargin));

            painter.save();
            painter.translate(market.vertex().position().toPointF() * scale + QPointF(circleSize / 2, -circleSize / 2) - boundingRect.bottomLeft());
            painter.drawRect(boundingRect);
            painter.drawText(0, 0, market.vertex().post().name());

            painter.translate(market.vertex().position().toPointF() * scale + QPointF(circleSize / 2, -circleSize / 2) - boundingRect2.bottomLeft());
            painter.drawRect(boundingRect2);
            painter.drawText(0, 30, QString::number(market.product()));

            painter.restore();
        }

        for (auto &storage : map_->storages()) {
            QRectF boundingRect(metrics.boundingRect(storage.name()));
            QRectF boundingRect2(metrics.boundingRect(storage.armor()));
            boundingRect.setTopLeft(boundingRect.topLeft() - QPointF(textRectMargin, textRectMargin));
            boundingRect.setSize(boundingRect.size() + QSize(textRectMargin, textRectMargin));

            painter.save();
            painter.translate(storage.vertex().position().toPointF() * scale + QPointF(circleSize / 2, -circleSize / 2) - boundingRect.bottomLeft());
            painter.drawRect(boundingRect);
            painter.drawText(0, 0, storage.vertex().post().name());

            painter.restore();
        }

//        for (Vertex &vertex : graph.vertices()) {
//            if (!vertex.isPostIdxNull()) {
//                QRectF boundingRect(metrics.boundingRect(vertex.post().name()));
//                boundingRect.setTopLeft(boundingRect.topLeft() - QPointF(textRectMargin, textRectMargin));
//                boundingRect.setSize(boundingRect.size() + QSize(textRectMargin, textRectMargin));

//                painter.save();
//                painter.translate(vertex.position().toPointF() * scale + QPointF(circleSize / 2, -circleSize / 2) - boundingRect.bottomLeft());
//                painter.drawRect(boundingRect);
//                painter.drawText(0, 0, vertex.post().name());

//                painter.restore();
//            }
//        }
    }
}
