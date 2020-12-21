#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include "windows.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->graphview->hide();
    ui->postInfo->hide();
//    QTimer *timer = new QTimer(this);

//    connect(timer, SIGNAL(timeout()), this, SLOT(on_startButton_clicked()));
}

MainWindow::~MainWindow() {
    delete ui;
}

//void MainWindow::setMap(std::shared_ptr<Map> m) {
//    ui->graphview->setMap(m, game_->player());
//}
#include "src/Thread.h"

void MainWindow::on_startButton_clicked() {
    if (ui->userNameForm->toPlainText() != "") {
        ui->startMenu->hide();
        ui->graphview->show();

        Game *game = new Game();

        QString userName = ui->userNameForm->toPlainText();

        this->update();

        thread = new QThread;
        game->moveToThread(thread);
        connect(this, SIGNAL(disconnect()), game, SLOT(disconnect()));
        connect(this, SIGNAL(init(const QString&)), game, SLOT(init(const QString&)));
        connect(game, SIGNAL(playerChanged(Player)), this, SLOT(onPlayerChanged(Player)));
        connect(game, SIGNAL(mapChanged(std::shared_ptr<Map>, Player)), this, SLOT(onMapChanged(std::shared_ptr<Map>, Player)));
        thread->start();

        emit init(userName);
    }
}

void MainWindow::on_logoutButton_clicked() {
    emit disconnect();

    ui->startMenu->show();
    ui->graphview->hide();
}

void MainWindow::onPlayerChanged(Player player) {
    ui->userName->setText("Name: " + player.name());
    ui->userTown->setText("City: " + player.town().name());
    ui->userPopulation->setText("Population: " +
                                QString::number(player.town().population()) +
                                " / " +
                                QString::number(player.town().populationCapacity()));
    ui->userProducts->setText("Products: " +
                              QString::number(player.town().product()) +
                              " / " +
                              QString::number(player.town().productCapacity()));

    ui->userSomething->setText("Rating: " + QString::number(player.rating()));
}

void MainWindow::onMapChanged(std::shared_ptr<Map> map, Player player) {
    ui->graphview->setMap(map, player);
    update();
    qDebug() << "Received map from Game thread";
}
