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

void MainWindow::setMap(std::shared_ptr<Map> m) {
    ui->graphview->setMap(m, game_->player());
}
#include "src/Thread.h"

void MainWindow::on_startButton_clicked()
{
    if (ui->userNameForm->toPlainText() != "") {
        ui->startMenu->hide();
        ui->graphview->show();

        game_ = new Game();

        QString userName = ui->userNameForm->toPlainText();

        game_->connectToServer();

        game_->login(userName);

        game_->getMap();

        game_->makeMap();

        Thread *thread = new Thread(this, game_->map());
        thread->start();

//        this->setMap(game_->map());
        this->update();

        ui->userGameInfo->setText("User Info");
        ui->userGameInfo->append("Name: " + game_->player().name());
//        ui->userGameInfo->append("City: " + game_->player().town().name());
        ui->userNameForm->clear();

//        qDebug() << game_->map()->graph().idx();
//        qDebug() << game_->map()->trains()[0].waysLength();
//        qDebug() << game_->map()->graph().idx().at(game_->player().town().point_idx());
        game_->gameCycle();
    }
}

void MainWindow::on_logoutButton_clicked()
{
    game_->disconnect();
    ui->startMenu->show();
    ui->graphview->hide();
}
