#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QWidget>
#include "Graph.h"

namespace Ui {
class GraphView;
}

class GraphView : public QWidget {
    Q_OBJECT
    public:
        explicit GraphView(QWidget *parent = nullptr);
        ~GraphView();

        void setGraph(std::unique_ptr<Graph> graph);

    protected:
        void paintEvent(QPaintEvent *event) override;

    private:
        std::unique_ptr<Graph> graph_ = nullptr;

        Ui::GraphView *ui;
};

#endif // GRAPHVIEW_H

