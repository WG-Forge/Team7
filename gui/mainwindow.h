#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Map.h"
#include "Game.h"
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private slots:
        void on_startButton_clicked();
        void on_logoutButton_clicked();

        void onPlayerChanged(Player player);
        void onMapChanged(std::shared_ptr<Map> map, Player player);

    signals:
        void init(const QString &username);
        void disconnect();

    private:
        Ui::MainWindow *ui;

        QThread *thread;
};

#endif // MAINWINDOW_H
