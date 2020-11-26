#include "gui/mainwindow.h"
#include "src/Graph.h"
#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <iostream>
#include "src/SocketTest.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowState(Qt::WindowMaximized);
//    w.show();

    try {
        QFile file("../Circumflex/tests/big_graph.json");

        if (!file.open(QIODevice::ReadOnly))
            throw std::runtime_error("Couldn't open save file.");

        std::unique_ptr<Graph> g = std::make_unique<Graph>(QJsonDocument::fromJson(file.readAll()).object());
        g->calcCoords(16.0f / 9);
        w.setGraph(std::move(g));
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    SocketTest cTest;
    cTest.Connect();

    cTest.sendData("{\"name\":\"Boris\"}", 1);
    cTest.getData();

    cTest.sendData("{\"layer\":0}", 10);
    cTest.getData();

    cTest.Close();

    return a.exec();
}
