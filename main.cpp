#include "gui/mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <iostream>
#include "src/Graph.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    try {
        QFile file("small_graph.json");

        if (!file.open(QIODevice::ReadOnly))
            throw std::runtime_error("Couldn't open save file.");

        Graph g(QJsonDocument::fromJson(file.readAll()).object());
        g.print();
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    return a.exec();
}
