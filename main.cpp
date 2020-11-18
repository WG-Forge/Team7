#include "gui/mainwindow.h"
#include "src/Graph.h"
#include "src/rand.h"
#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Rand::init();

    MainWindow w;
    w.show();

    try {
        QFile file("big_graph.json");

        if (!file.open(QIODevice::ReadOnly))
            throw std::runtime_error("Couldn't open save file.");

        Graph *g = new Graph(QJsonDocument::fromJson(file.readAll()).object());
        g->calcCoords();
        w.setGraph(g);
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    return a.exec();
}
