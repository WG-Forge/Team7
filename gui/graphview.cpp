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

    ui->town_1->hide();
    ui->town_2->hide();
    ui->town_3->hide();
    ui->town_4->hide();

    ui->market_1->hide();
    ui->market_2->hide();
    ui->market_3->hide();
    ui->market_4->hide();

    ui->storage_1->hide();
    ui->storage_2->hide();
    ui->storage_3->hide();
    ui->storage_4->hide();
}

GraphView::~GraphView() {
    delete ui;
}

void GraphView::setMap(std::shared_ptr<Map> m, Player *player, bool ggg) {
    isMap = ggg;
    qDebug() << "MAP UPDATE";
    map_ = m;
    player_ = player;
}

void GraphView::paintEvent(QPaintEvent *event) {
    ui->loadingLabel->setVisible(map_ == nullptr);
    QString enemyTrainStyleSheet = "border-radius: 8px; background-color: red; color: black; font-weight: 600; text-align: center; border: 1px solid black;";
    QString playerTrainStyleSheet = "border-radius: 8px; background-color: lightgreen; color: black; font-weight: 600; text-align: center; border: 1px solid black;";
    QString enemyTownStyleSheet = "background-color: pink; color: black; font-weight: 600; text-align: center; border: 1px solid black;";
    QString playerTownStyleSheet = "background-color: lightyellow; color: black; font-weight: 600; text-align: center; border: 1px solid black;";
    QString marketStyleSheet = "background-color: white; color: black; font-weight: 600; text-align: center; border: 1px solid black;";

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

        painter.translate(W / 2, H / 2);

        for (auto &train : map_.get()->trains()) {
            QLabel *currentLabel;

            if (train.idx() == 1) currentLabel = ui->trainObject_1;
            if (train.idx() == 2) currentLabel = ui->trainObject_2;
            if (train.idx() == 3) currentLabel = ui->trainObject_3;
            if (train.idx() == 4) currentLabel = ui->trainObject_4;
//            if (train.idx() == 5) currentLabel = ui->trainObject_5;
//            if (train.idx() == 6) currentLabel = ui->trainObject_6;
//            if (train.idx() == 7) currentLabel = ui->trainObject_7;
//            if (train.idx() == 8) currentLabel = ui->trainObject_8;
//            if (train.idx() == 9) currentLabel = ui->trainObject_9;
//            if (train.idx() == 10) currentLabel = ui->trainObject_10;
//            if (train.idx() == 11) currentLabel = ui->trainObject_11;
//            if (train.idx() == 12) currentLabel = ui->trainObject_12;
//            if (train.idx() == 13) currentLabel = ui->trainObject_13;
//            if (train.idx() == 14) currentLabel = ui->trainObject_14;
//            if (train.idx() == 15) currentLabel = ui->trainObject_15;
//            if (train.idx() == 16) currentLabel = ui->trainObject_16;


            for (auto &edge : map_.get()->graph().edges()) {
                if (train.lineIdx() == edge.idx()) {
                    Vertex v1 = edge.vertex1();
                    Vertex v2 = edge.vertex2();

                    X1 = v1.position().x() * scale;
                    Y1 = v1.position().y() * scale;
                    X2 = v2.position().x() * scale;
                    Y2 = v2.position().y() * scale;

                    partLength = sqrt((pow(X1 - X2, 2) + pow(Y1 - Y2, 2))) / edge.length();
                    position = train.position();

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

            if (train.playerIdx() != player_->idx()) currentLabel->setStyleSheet({enemyTrainStyleSheet});
            else currentLabel->setStyleSheet({playerTrainStyleSheet});

            currentLabel->setGeometry(r1);
            currentLabel->setText(QString::number(train.level()));

            if (!isMap) currentLabel->show();
        }
    }

    int townIndex = 0;
    for (auto &town : map_->towns()) {
        QPainter painter(this);
        Graph& graph = map_->graph();
        QFontMetrics metrics(ui->town_1->font());
        float W = painter.device()->width();
        float H = painter.device()->height();
        float scale = std::min(W / 16 * 9, H) * 0.95;
        double circleSize = 6;
        double textRectMargin = 2;
        double userCircleSize = 10;
        float posX = town.vertex().position().x(),
                posY = town.vertex().position().y();

        painter.translate(W / 2, H / 2);

        QLabel *currentTown;
        if (townIndex == 0) currentTown = ui->town_1;
        if (townIndex == 1) currentTown = ui->town_2;
        if (townIndex == 2) currentTown = ui->town_3;
        if (townIndex == 3) currentTown = ui->town_4;

        if (town.playerIdx() != player_->idx()) currentTown->setStyleSheet({enemyTownStyleSheet});
        else currentTown->setStyleSheet({playerTownStyleSheet});

        QRect boundingRect(metrics.boundingRect(QString(town.name() + " | " + QString::number(town.population()))));
        boundingRect.setTopLeft(boundingRect.topLeft() - QPoint(textRectMargin, textRectMargin));
        boundingRect.setRight(boundingRect.right() + 3 * textRectMargin);
        boundingRect.setSize(boundingRect.size() + QSize(textRectMargin, textRectMargin));

        painter.save();
        if (townIndex == 0) boundingRect.moveTo(posX * scale + W/2, posY * scale + H/2 - boundingRect.height() - circleSize);
        if (townIndex == 1) boundingRect.moveTo(posX * scale + W/2 + circleSize / 2, posY * scale + H/2 - boundingRect.height() - circleSize);
        if (townIndex == 2) boundingRect.moveTo(posX * scale + W/2, posY * scale + H/2 + circleSize);
        if (townIndex == 3) boundingRect.moveTo(posX * scale + W/2 + circleSize / 2, posY * scale + H/2 + circleSize);

        currentTown->setText(QString(town.name() + " | " + QString::number(town.population())));
        currentTown->setGeometry(boundingRect);
        if (!isMap) currentTown->show();
        painter.restore();
        townIndex++;
    }

    int marketIndex = 0;
    for (auto &market : map_->markets()) {
        QPainter painter(this);
        Graph& graph = map_->graph();
        QFontMetrics metrics(ui->market_1->font());
        float W = painter.device()->width();
        float H = painter.device()->height();
        float scale = std::min(W / 16 * 9, H) * 0.95;
        double circleSize = 6;
        double textRectMargin = 2;
        double userCircleSize = 10;
        float posX = market.vertex().position().x(),
                posY = market.vertex().position().y();

        painter.translate(W / 2, H / 2);

        QLabel *currentMarket;
        if (marketIndex == 0) currentMarket = ui->market_1;
        if (marketIndex == 1) currentMarket = ui->market_2;
        if (marketIndex == 2) currentMarket = ui->market_3;
        if (marketIndex == 3) currentMarket = ui->market_4;

        currentMarket->setStyleSheet({marketStyleSheet});

        QRect boundingRect(metrics.boundingRect(QString(market.name() + " | " + QString::number(market.product()))));

        boundingRect.setTopLeft(boundingRect.topLeft() - QPoint(textRectMargin, textRectMargin));
        boundingRect.setRight(boundingRect.right() + 3 * textRectMargin);
        boundingRect.setSize(boundingRect.size() + QSize(textRectMargin, textRectMargin));

        painter.save();

        if (marketIndex == 0) boundingRect.moveTo(posX * scale + W/2 - boundingRect.width(), posY * scale + H/2 - boundingRect.height() - circleSize / 2);
        if (marketIndex == 1) boundingRect.moveTo(posX * scale + W/2 + circleSize / 2, posY * scale + H/2 - boundingRect.height() - circleSize / 2);
        if (marketIndex == 2) boundingRect.moveTo(posX * scale + W/2 - boundingRect.width(), posY * scale + H/2 + circleSize / 2);
        if (marketIndex == 3) boundingRect.moveTo(posX * scale + W/2 + circleSize / 2, posY * scale + H/2 + circleSize / 2);

        currentMarket->setText(QString(market.name() + " | " + QString::number(market.product())));
        currentMarket->setGeometry(boundingRect);
        if (!isMap) currentMarket->show();
        painter.restore();
        marketIndex++;
    }

    int storageIndex = 0;
    for (auto &storage : map_->storages()) {
        QPainter painter(this);
        Graph& graph = map_->graph();
        QFontMetrics metrics(ui->market_1->font());
        float W = painter.device()->width();
        float H = painter.device()->height();
        float scale = std::min(W / 16 * 9, H) * 0.95;
        double circleSize = 6;
        double textRectMargin = 2;
        double userCircleSize = 10;
        float posX = storage.vertex().position().x(),
                posY = storage.vertex().position().y();

        painter.translate(W / 2, H / 2);

        QLabel *currentStorage;
        if (storageIndex == 0) currentStorage = ui->storage_1;
        if (storageIndex == 1) currentStorage = ui->storage_2;
        if (storageIndex == 2) currentStorage = ui->storage_3;
        if (storageIndex == 3) currentStorage = ui->storage_4;

        currentStorage->setStyleSheet({marketStyleSheet});

        QRect boundingRect(metrics.boundingRect(QString(storage.name() + " | " + QString::number(storage.armor()))));

        boundingRect.setTopLeft(boundingRect.topLeft() - QPoint(textRectMargin, textRectMargin));
        boundingRect.setRight(boundingRect.right() + 3 * textRectMargin);
        boundingRect.setSize(boundingRect.size() + QSize(textRectMargin, textRectMargin));

        painter.save();

        if (storageIndex == 0) boundingRect.moveTo(posX * scale + W/2 - boundingRect.width(), posY * scale + H/2 - boundingRect.height() - circleSize / 2);
        if (storageIndex == 1) boundingRect.moveTo(posX * scale + W/2 + circleSize / 2, posY * scale + H/2 - boundingRect.height() - circleSize / 2);
        if (storageIndex == 2) boundingRect.moveTo(posX * scale + W/2 - boundingRect.width(), posY * scale + H/2 + circleSize / 2);
        if (storageIndex == 3) boundingRect.moveTo(posX * scale + W/2 + circleSize / 2, posY * scale + H/2 + circleSize / 2);

        currentStorage->setText(QString(storage.name() + " | " + QString::number(storage.armor())));
        currentStorage->setGeometry(boundingRect);
        if (!isMap) currentStorage->show();
        painter.restore();
        storageIndex++;
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
        }

        for (auto &post : map_->posts()) {
                float posX = post.vertex().position().x() * scale;
                float posY = post.vertex().position().y() * scale;
                enum PostType type = post.type();

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

                if (post.playerIdx() == player_->idx()) {
                    painter.setBrush(Qt::yellow);
                    painter.drawEllipse(posX - userCircleSize / 2, posY - userCircleSize / 2, userCircleSize, userCircleSize);
                    continue;
                }

                painter.drawEllipse(posX - circleSize / 2, posY - circleSize / 2, circleSize, circleSize);
        }
    }
}
