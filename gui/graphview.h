#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QWidget>
#include <QVector2D>
#include <memory>
#include "../src/Graph.h"
#include "../src/Post.h"
#include "../src/Enums/PostType.h";

class GraphView : public QWidget {
    Q_OBJECT
    public:
        explicit GraphView(QWidget *parent = nullptr);

        void setGraph(std::unique_ptr<Graph> graph);

    protected:
        void paintEvent(QPaintEvent *event) override;

    private:
        std::unique_ptr<Graph> graph_ = nullptr;
};

#endif // GRAPHVIEW_H
