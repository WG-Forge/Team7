#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QtDebug>
#include "Game.h"
#include "../gui/mainwindow.h"
#include "Map.h"

class Thread : public QThread {
    public:
        Thread() = default;
        Thread(MainWindow *window, std::shared_ptr<Map> map) {
            window_ = window; map_ = map;
        };


        void run() {
            qDebug() << "thread started";

        }
    private:
        MainWindow *window_;
        std::shared_ptr<Map> map_;
};

#endif // THREAD_H
