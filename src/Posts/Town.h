#ifndef TOWN_H
#define TOWN_H

#include "Post.h"

class Town: public Post
{
public:
    explicit Town() = default;
    explicit Town(const QJsonObject& town);
    ~Town() = default;

    int armor();
    int armor_capacity();
    int level();
    int next_level_price();
    QString player_idx();
    int population();
    int population_capacity();
    int product();
    int product_capacity();
    int train_cooldown();

    void setProduct(int amount) { product_ += amount; };

private:
    int armor_;
    int armor_capacity_;
    int level_;
    int next_level_price_;
    QString player_idx_;
    int population_;
    int population_capacity_;
    int product_;
    int product_capacity_;
    int train_cooldown_;

};

#endif // TOWN_H
