#include "gui/mainwindow.h"
#include "src/Graph.h"
#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <iostream>
#include "src/SocketTest.h"
#include "src/Post.h"
#include "src/Posts/Market.h"
#include "src/Posts/Storage.h"
#include "src/Posts/Town.h"
#include "src/Map.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowState(Qt::WindowMaximized);
    w.show();

    SocketTest cTest;
    cTest.Connect();
    QJsonObject data;
    data["name"] = "Boris";
    cTest.sendData(Request(Action::LOGIN,data));
    cTest.getData();
    QJsonObject data1;
    data1["layer"] = 0;
    cTest.sendData(Request(Action::MAP,data1));
    QJsonObject response = cTest.getData();

    try {
//        QFile file("../QtProjects/tests/big_graph.json");

//        if (!file.open(QIODevice::ReadOnly))
//            throw std::runtime_error("Couldn't open save file.");

       // std::unique_ptr<Graph> g = std::make_unique<Graph>(QJsonDocument::fromJson(file.readAll()).object());
        std::unique_ptr<Graph> g = std::make_unique<Graph>(response);
        g->calcCoords(16.0f / 9);
        w.setGraph(std::move(g));
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    QJsonObject layer1;
    layer1["layer"] = 1;
    cTest.sendData(Request(Action::MAP,layer1));
    response = cTest.getData();
    Map map(response);
    cTest.Close();

    return a.exec();
}
