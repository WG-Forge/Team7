#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->graphview->hide();
    ui->textEdit->setText("User Info");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setMap(std::unique_ptr<Map> m) {
    ui->graphview->setMap(std::move(m), game_->player());
}

void MainWindow::on_startButton_clicked()
{
    if (ui->userNameForm->toPlainText() != "") {
        ui->startMenu->hide();
        ui->graphview->show();

        QString userName = ui->userNameForm->toPlainText();

        game_->connectToServer();

        game_->login(userName);

        game_->getMap();

        game_->drawMap();

        this->setMap(game_->map());
        this->update();

        ui->textEdit->setText("User Info");
        ui->textEdit->append("Name: " + game_->player().name());
        ui->textEdit->append("City: " + game_->player().town().name());
        ui->userNameForm->clear();
    }
}

void MainWindow::on_logoutButton_clicked()
{
    game_->disconnect();

    ui->startMenu->show();
    ui->graphview->hide();
}
