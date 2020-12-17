#include "gui/mainwindow.h"
#include "src/Graph.h"
#include "src/Socket.h"
#include "src/Post.h"
#include "src/Posts/Market.h"
#include "src/Posts/Storage.h"
#include "src/Posts/Town.h"
#include "src/Map.h"
#include "src/Enums/EventType.h"
#include "src/Game.h"

#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

//    Game *game = new Game();

    MainWindow w;
//    w.setWindowState(Qt::WindowMaximized);
//    w.setGame(game);
    w.show();
    a.processEvents();

//    std::unique_ptr<Map> m = std::make_unique<Map>(layer_0, layer_1, layer_2);
//    w.setMap(std::move(m));
//    w.update();
    return a.exec();
}
