#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include "windows.h"
#include <QButtonGroup>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->graphview->hide();
    ui->postScreen->hide();

    gamesList = ui->gamesList;
    ui->postInfo->hide();

    gamesBoxes.append(ui->gameBox);
    gamesBoxes.append(ui->gameBox_2);
    gamesBoxes.append(ui->gameBox_3);
    gamesBoxes.append(ui->gameBox_4);
    gamesBoxes.append(ui->gameBox_5);
    gamesBoxes.append(ui->gameBox_6);
    gamesBoxes.append(ui->gameBox_7);
    gamesBoxes.append(ui->gameBox_8);

    ui->gameBox->hide();
    ui->gameBox_2->hide();
    ui->gameBox_3->hide();
    ui->gameBox_4->hide();
    ui->gameBox_5->hide();
    ui->gameBox_6->hide();
    ui->gameBox_7->hide();
    ui->gameBox_8->hide();

    Game *game = new Game();

    thread = new QThread;
    game->moveToThread(thread);
    connect(this, SIGNAL(init()), game, SLOT(init()));
    connect(game, SIGNAL(showMap()), this, SLOT(onShowMap()));
    connect(this, SIGNAL(disconnect()), game, SLOT(disconnect()));
    connect(this, SIGNAL(updateGames()), game, SLOT(updateGames()));
//    connect(game, SIGNAL(playerChanged(Player)), this, SLOT(onPlayerChanged(Player)));
    connect(game, SIGNAL(playerChanged(Player *, bool)), this, SLOT(onPlayerChanged(Player *, bool)));
    connect(game, SIGNAL(getGames(const QJsonObject &)), this, SLOT(onGetGames(const QJsonObject &)));
    connect(this, SIGNAL(connectToGame(const QString &, const QString &, const QString &, const int &, const int &)),
            game, SLOT(connectToGame(const QString &, const QString &, const QString &, const int &, const int &)));
    connect(game, SIGNAL(mapChanged(std::shared_ptr<Map>, Player *, bool)), this, SLOT(onMapChanged(std::shared_ptr<Map>, Player *, bool)));
    connect(game, SIGNAL(gameEnd(const int)), this, SLOT(onGameEnd(const int)));

    thread->start();

    emit init();

//    QTimer *timer = new QTimer(this);

//    connect(timer, SIGNAL(timeout()), this, SLOT(on_startButton_clicked()));
}

MainWindow::~MainWindow() {
    if (thread) {
        thread->terminate();
        thread->wait();
        delete thread;
    }

    delete ui;
}

void MainWindow::onShowMap() {
    ui->graphview->show();
    ui->startMenu->hide();
}

void MainWindow::on_logoutButton_clicked() {
    emit disconnect();

    ui->startMenu->show();
    ui->graphview->hide();
    ui->gamesWidget->hide();
    ui->gamesList->layout()->deleteLater();

    emit init();
    emit updateGames();
}

void MainWindow::onPlayerChanged(Player *player, bool isReady) {
    qDebug() << "PLAYER UPDATE";
    for (auto &train : player->trains()) {
    }
    ui->userName->setText(player->name());
    ui->userTown->setText("City: " + player->town().name());
    ui->userPopulation->setText("Population: " +
                                QString::number(player->town().population()) +
                                " / " +
                                QString::number(player->town().populationCapacity()));
    ui->userProducts->setText("Products: " +
                              QString::number(player->town().product()) +
                              " / " +
                              QString::number(player->town().productCapacity()));

    ui->userRating->setText("Rating: " + QString::number(player->rating()));
    ui->userArmor->setText("Armor: " + QString::number(player->town().armor()));
    ui->userLevel->setText("Level: " + QString::number(player->town().level()));
    QString maxTicks = QString::number(player->maxTicks());
    if (player->maxTicks() == -1) {
        maxTicks = "UNLIMITED";
    }
    ui->currentGameTicks->setText("Ticks: " + QString::number(player->currentTick()) + " / " + maxTicks);

    int trainIndex = 0;
    for (auto &train : player->trains()) {
        if (trainIndex == 0) {
            ui->trainLevel_1->setText(QString::number(player->trains()[trainIndex]->level()));
            ui->trainGoods_1->setText(QString::number(player->trains()[trainIndex]->goods()));
        }
        if (trainIndex == 1) {
            ui->trainLevel_2->setText(QString::number(player->trains()[trainIndex]->level()));
            ui->trainGoods_2->setText(QString::number(player->trains()[trainIndex]->goods()));
        }
        if (trainIndex == 2) {
            ui->trainLevel_3->setText(QString::number(player->trains()[trainIndex]->level()));
            ui->trainGoods_3->setText(QString::number(player->trains()[trainIndex]->goods()));
        }
        if (trainIndex == 3) {
            ui->trainLevel_4->setText(QString::number(player->trains()[trainIndex]->level()));
            ui->trainGoods_4->setText(QString::number(player->trains()[trainIndex]->goods()));
        }
        trainIndex++;
    }
}

void MainWindow::onMapChanged(std::shared_ptr<Map> map, Player *player, bool ggg) {
    ui->graphview->setMap(map, player, ggg);
//    update();
    qDebug() << "Received map from Game thread";
}

void MainWindow::onGetGames(const QJsonObject &gamesData) {
    ui->gameBox->hide();
    ui->gameBox_2->hide();
    ui->gameBox_3->hide();
    ui->gameBox_4->hide();
    ui->gameBox_5->hide();
    ui->gameBox_6->hide();
    ui->gameBox_7->hide();
    ui->gameBox_8->hide();

    qDebug() << gamesData["games"].toArray();
    QJsonArray games = gamesData["games"].toArray();

    if (games.size() == 0) return;
    if (games.size() >= 1) {
        ui->gameName_36->setText(games[0].toObject()["name"].toString());
        ui->gamePlayers_36->setText(QString::number(games[0].toObject()["num_players"].toInt()));
        ui->gameTicks_36->setText(QString::number(games[0].toObject()["num_turns"].toInt()));
        ui->gameStatement_36->setText(QString::number(games[0].toObject()["state"].toInt()));

        ui->gameBox_8->show();
    }
    if (games.size() >= 2) {
        ui->gameName_35->setText(games[1].toObject()["name"].toString());
        ui->gamePlayers_35->setText(QString::number(games[1].toObject()["num_players"].toInt()));
        ui->gameTicks_35->setText(QString::number(games[1].toObject()["num_turns"].toInt()));
        ui->gameStatement_35->setText(QString::number(games[1].toObject()["state"].toInt()));

        ui->gameBox_7->show();
    }
    if (games.size() >= 3) {
        ui->gameName_34->setText(games[2].toObject()["name"].toString());
        ui->gamePlayers_34->setText(QString::number(games[2].toObject()["num_players"].toInt()));
        ui->gameTicks_34->setText(QString::number(games[2].toObject()["num_turns"].toInt()));
        ui->gameStatement_34->setText(QString::number(games[2].toObject()["state"].toInt()));
        ui->gameBox_6->show();
    }
    if (games.size() >= 4) {
        ui->gameName_33->setText(games[3].toObject()["name"].toString());
        ui->gamePlayers_33->setText(QString::number(games[3].toObject()["num_players"].toInt()));
        ui->gameTicks_33->setText(QString::number(games[3].toObject()["num_turns"].toInt()));
        ui->gameStatement_33->setText(QString::number(games[3].toObject()["state"].toInt()));
        ui->gameBox_5->show();
    }
    if (games.size() >= 5) {
        ui->gameName_32->setText(games[4].toObject()["name"].toString());
        ui->gamePlayers_32->setText(QString::number(games[4].toObject()["num_players"].toInt()));
        ui->gameTicks_32->setText(QString::number(games[4].toObject()["num_turns"].toInt()));
        ui->gameStatement_32->setText(QString::number(games[4].toObject()["state"].toInt()));
        ui->gameBox_4->show();
    }
    if (games.size() >= 6) {
        ui->gameName_31->setText(games[5].toObject()["name"].toString());
        ui->gamePlayers_31->setText(QString::number(games[5].toObject()["num_players"].toInt()));
        ui->gameTicks_31->setText(QString::number(games[5].toObject()["num_turns"].toInt()));
        ui->gameStatement_31->setText(QString::number(games[5].toObject()["state"].toInt()));
        ui->gameBox_3->show();
    }
    if (games.size() >= 7) {
        ui->gameName_30->setText(games[6].toObject()["name"].toString());
        ui->gamePlayers_30->setText(QString::number(games[6].toObject()["num_players"].toInt()));
        ui->gameTicks_30->setText(QString::number(games[6].toObject()["num_turns"].toInt()));
        ui->gameStatement_30->setText(QString::number(games[6].toObject()["state"].toInt()));
        ui->gameBox_2->show();
    }
    if (games.size() >= 8) {
        ui->gameName->setText(games[7].toObject()["name"].toString());
        ui->gamePlayers->setText(QString::number(games[7].toObject()["num_players"].toInt()));
        ui->gameTicks->setText(QString::number(games[7].toObject()["num_turns"].toInt()));
        ui->gameStatement->setText(QString::number(games[7].toObject()["state"].toInt()));
        ui->gameBox->show();
    }
}


// Я ХЗ КАК ЭТИ КНОПКИ ОБРАБАТЫВАТЬ, ПОЭТОМУ БАХНУЛ ХАРДКОДОМ  \\\ НУ Я РЕАЛЬНО ХЗ КАК ЭТИ ЛЕЙБЛЫ ПОЛУЧИТЬ ОТ КНОПКИ, ДА И ИХ ЕЩЁ И ФИГ ОБНОВИШЬ

void MainWindow::on_joinButton_36_clicked()
{
    QString userName = ui->nickname->toPlainText();
    QString password = ui->password->toPlainText();
    QString gameName = ui->gameName_36->text();
    int players = ui->gamePlayers_36->text().toInt();
    int ticks  = ui->gameTicks_36->text().toInt();
    int state = ui->gameStatement_36->text().toInt();

    if (state == 2 || state == 3) return;
    if (userName.size() < 2) return;
//    if (password.size()) return;

    emit connectToGame(userName, password, gameName, players, ticks);
}

void MainWindow::on_joinButton_35_clicked()
{
    QString userName = ui->nickname->toPlainText();
    QString password = ui->password->toPlainText();
    QString gameName = ui->gameName_35->text();
    int players = ui->gamePlayers_35->text().toInt();
    int ticks  = ui->gameTicks_35->text().toInt();
    int state = ui->gameStatement_35->text().toInt();

    if (state == 2 || state == 3) return;
    if (userName.size() < 2) return;
//    if (password.size()) return;

    emit connectToGame(userName, password, gameName, players, ticks);
}

void MainWindow::on_joinButton_34_clicked()
{
    QString userName = ui->nickname->toPlainText();
    QString password = ui->password->toPlainText();
    QString gameName = ui->gameName_34->text();
    int players = ui->gamePlayers_34->text().toInt();
    int ticks  = ui->gameTicks_34->text().toInt();
    int state = ui->gameStatement_34->text().toInt();

    if (state == 2 || state == 3) return;
    if (userName.size() < 2) return;
//    if (password.size()) return;

    emit connectToGame(userName, password, gameName, players, ticks);
}

void MainWindow::on_joinButton_33_clicked()
{
    QString userName = ui->nickname->toPlainText();
    QString password = ui->password->toPlainText();
    QString gameName = ui->gameName_33->text();
    int players = ui->gamePlayers_33->text().toInt();
    int ticks  = ui->gameTicks_33->text().toInt();
    int state = ui->gameStatement_33->text().toInt();

    if (state == 2 || state == 3) return;
    if (userName.size() < 2) return;
//    if (password.size()) return;

    emit connectToGame(userName, password, gameName, players, ticks);
}

void MainWindow::on_joinButton_32_clicked()
{
    QString userName = ui->nickname->toPlainText();
    QString password = ui->password->toPlainText();
    QString gameName = ui->gameName_32->text();
    int players = ui->gamePlayers_32->text().toInt();
    int ticks  = ui->gameTicks_32->text().toInt();
    int state = ui->gameStatement_32->text().toInt();

    if (state == 2 || state == 3) return;
    if (userName.size() < 2) return;
//    if (password.size()) return;

    emit connectToGame(userName, password, gameName, players, ticks);
}

void MainWindow::on_joinButton_31_clicked()
{
    QString userName = ui->nickname->toPlainText();
    QString password = ui->password->toPlainText();
    QString gameName = ui->gameName_31->text();
    int players = ui->gamePlayers_31->text().toInt();
    int ticks  = ui->gameTicks_31->text().toInt();
    int state = ui->gameStatement_31->text().toInt();

    if (state == 2 || state == 3) return;
    if (userName.size() < 2) return;
//    if (password.size()) return;

    emit connectToGame(userName, password, gameName, players, ticks);
}

void MainWindow::on_joinButton_30_clicked()
{
    QString userName = ui->nickname->toPlainText();
    QString password = ui->password->toPlainText();
    QString gameName = ui->gameName_30->text();
    int players = ui->gamePlayers_30->text().toInt();
    int ticks  = ui->gameTicks_30->text().toInt();
    int state = ui->gameStatement_30->text().toInt();

    if (state == 2 || state == 3) return;
    if (userName.size() < 2) return;
//    if (password.size()) return;

    emit connectToGame(userName, password, gameName, players, ticks);
}

void MainWindow::on_joinButton_clicked()
{
    QString userName = ui->nickname->toPlainText();
    QString password = ui->password->toPlainText();
    QString gameName = ui->gameName->text();
    int players = ui->gamePlayers->text().toInt();
    int ticks  = ui->gameTicks->text().toInt();
    int state = ui->gameStatement->text().toInt();

    if (state == 2 || state == 3) return;
    if (userName.size() < 2) return;
//    if (password.size()) return;

    emit connectToGame(userName, password, gameName, players, ticks);
}

void MainWindow::on_hostButton_clicked()
{
    QString userName = ui->nickname->toPlainText();
    QString password = ui->password->toPlainText();
    QString gameName = ui->hostGameName->text();
    int players = ui->hostPlayersAmout->text().toInt();
    int ticks  = ui->hostTicksValue->text().toInt();

    if (userName.size() < 2) return;
//    if (password.size()) return;

    emit connectToGame(userName, password, gameName, players, ticks);
}

void MainWindow::on_updateServers_clicked()
{
    qDebug() << "clicked";
    emit updateGames();
}

void MainWindow::onGameEnd(int rating) {
    thread->exit();
    this->disconnect();

    ui->graphview->hide();
    ui->postScreen->show();
    ui->postRating->setText("Финальный рейтинг: " + QString::number(rating));
}

void MainWindow::on_pushButton_clicked()
{
    ui->postScreen->hide();
    ui->startMenu->show();
    emit init();
    thread->start();
}
