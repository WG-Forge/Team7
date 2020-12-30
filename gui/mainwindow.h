#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Map.h"
#include "Game.h"
#include <QThread>
#include <QGroupBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private slots:
//        void onGameConnection();

        void onPlayerChanged(Player *player, bool isReady);
        void onMapChanged(std::shared_ptr<Map> map, Player *player, bool ggg, std::vector<QString> *, std::vector<int> *);
        void onGetGames(const QJsonObject &gamesData);
        void onShowMap();
        void onGameEnd(const int);

//        void on_startButton_clicked();
        void on_logoutButton_clicked();
        void on_joinButton_36_clicked();
        void on_joinButton_35_clicked();
        void on_joinButton_34_clicked();
        void on_joinButton_33_clicked();
        void on_joinButton_32_clicked();
        void on_joinButton_31_clicked();
        void on_joinButton_30_clicked();
        void on_joinButton_clicked();

        void on_updateServers_clicked();
        void on_hostButton_clicked();

        void on_exitFInal_clicked();

signals:
        void init();
        void disconnect();
        void updateGames();
        void connectToGame(const QString &, const QString &, const QString &, const int &, const int &);
//        void gameConnected();

    private:
        Ui::MainWindow *ui;

        QThread *thread = nullptr;
        QWidget *gamesList;
        QVector<QGroupBox *> gamesBoxes;
};

#endif // MAINWINDOW_H
