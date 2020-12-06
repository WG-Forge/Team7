#include "gui/mainwindow.h"
#include "src/Graph.h"
#include "src/SocketTest.h"
#include "src/Post.h"
#include "src/Posts/Market.h"
#include "src/Posts/Storage.h"
#include "src/Posts/Town.h"
#include "src/Map.h"
#include "src/Enums/EventType.h"

#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowState(Qt::WindowMaximized);
    w.show();

    SocketTest cTest;
    cTest.Connect();

    QJsonObject data;
    data["name"] = "Boris";
    cTest.sendData(Request(Action::LOGIN, data));
    cTest.getData();

    QJsonObject data1;
    data1["layer"] = 0;
    cTest.sendData(Request(Action::MAP, data1));
    QJsonObject layer_0 = cTest.getData();

//    try {
////        QFile file("../QtProjects/tests/big_graph.json");

////        if (!file.open(QIODevice::ReadOnly))
////            throw std::runtime_error("Couldn't open save file.");

//       // std::unique_ptr<Graph> g = std::make_unique<Graph>(QJsonDocument::fromJson(file.readAll()).object());

//        std::unique_ptr<Graph> g = std::make_unique<Graph>(response);
//        g->calcCoords(16.0f / 9);
//        w.setGraph(std::move(g));
//    }
//    catch (std::exception &e) {
//        std::cout << e.what() << std::endl;
//    }

    QJsonObject data2;
    data2["layer"] = 1;
    cTest.sendData(Request(Action::MAP, data2));
    QJsonObject layer_1 = cTest.getData();

    Map map(layer_1);

    QJsonObject data3;
    data3["layer"] = 10;
    cTest.sendData(Request(Action::MAP, data3));
    QJsonObject layer_2 = cTest.getData();

    cTest.Close();

    std::unique_ptr<Graph> g = std::make_unique<Graph>(layer_0, map.posts());
//    g->setCoords(layer_2);
    g->calcCoords(16.0f / 9, layer_2);
    w.setGraph(std::move(g));

    return a.exec();
}
