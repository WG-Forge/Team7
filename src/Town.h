#ifndef TOWN_H
#define TOWN_H
#include "Post.h"

class Town: public Post
{
public:
   explicit Town() = default;
   explicit Town(const QJsonObject& town);

    int armor();
    int armor_capacity();
    int level();
    int next_level_price();
    QString player_idx();
    int point_idx();
    int population();
    int population_capacity();
    int product();
    int product_capacity();
    int train_cooldown();

private:
    int armor_;
    int armor_capacity_;
    std::vector<Event> events_;
    int idx_;
    int level_;
    QString name_;
    int next_level_price_;
    QString player_idx_;
    int point_idx_;
    int population_;
    int population_capacity_;
    int product_;
    int product_capacity_;
    int train_cooldown_;
    enum PostType type_;

};

#endif // TOWN_H
