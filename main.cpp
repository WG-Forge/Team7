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
    w.setWindowState(Qt::WindowMaximized);
    w.show();

    try {
        QFile file("../Circumflex/tests/big_graph.json");

        if (!file.open(QIODevice::ReadOnly))
            throw std::runtime_error("Couldn't open save file.");

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        Graph *g = new Graph(QJsonDocument::fromJson(file.readAll()).object());
        g->calcCoords(16.0f / 9);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [ms]" << std::endl;
        w.setGraph(g);
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    return a.exec();
}
