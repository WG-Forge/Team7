#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->pushButton_2->hide();
//    connect(this, &MainWindow::on)
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setMap(std::unique_ptr<Map> m) {
    ui->graphview->setMap(std::move(m));
}

void MainWindow::on_pushButton_clicked()
{
    game_->connectToServer();

    game_->login();

    game_->getMap();

    game_->drawMap();

//    this->setMap(game_->map());
}

void MainWindow::on_pushButton_2_clicked()
{
    game_->disconnect();
}
