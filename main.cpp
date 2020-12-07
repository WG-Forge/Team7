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
    a.processEvents();

    SocketTest cTest;
    cTest.Connect();

    QJsonObject data;
    data["name"] = "Boris";
    cTest.sendData(Request(Action::LOGIN, data));
    cTest.getData();

    cTest.sendData(Request(Action::MAP, QJsonObject({{"layer", 0}})));
    QJsonObject layer_0 = cTest.getData();

    cTest.sendData(Request(Action::MAP, QJsonObject({{"layer", 1}})));
    QJsonObject layer_1 = cTest.getData();

    cTest.sendData(Request(Action::MAP, QJsonObject({{"layer", 10}})));
    QJsonObject layer_2 = cTest.getData();

    cTest.Close();

    std::unique_ptr<Map> m = std::make_unique<Map>(layer_0, layer_1, layer_2);
    w.setMap(std::move(m));
    w.update();

    return a.exec();
}
