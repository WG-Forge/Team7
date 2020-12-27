#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QWidget>
#include "Map.h"
#include "Player.h"

namespace Ui {
class GraphView;
}

class GraphView : public QWidget {
    Q_OBJECT
    public:
        explicit GraphView(QWidget *parent = nullptr);
        ~GraphView();

        void setMap(std::shared_ptr<Map> m, Player *player, bool map);

    protected:
        void paintEvent(QPaintEvent *event) override;

    private:
        std::shared_ptr<Map> map_ = nullptr;
        Player *player_;

        Ui::GraphView *ui;
};

#endif // GRAPHVIEW_H

