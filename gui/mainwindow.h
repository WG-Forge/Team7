#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Map.h"
#include "Game.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setMap(std::unique_ptr<Map> m);
    void setGame(Game &game) { game_ = &game; };
    Game& game() { return *game_; };

private slots:
        void on_pushButton_clicked();

        void on_pushButton_2_clicked();

private:
        Ui::MainWindow *ui;
        Game *game_;
};

#endif // MAINWINDOW_H
