#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QWidget>
#include <QVector2D>
#include "../src/Graph.h"

class GraphView : public QWidget {
    Q_OBJECT
    public:
        explicit GraphView(QWidget *parent = nullptr);

        void setGraph(Graph *graph);

    protected:
        void paintEvent(QPaintEvent *event) override;

    private:
        Graph *graph_ = nullptr;
};

#endif // GRAPHVIEW_H
