#ifndef TOWN_H
#define TOWN_H

#include "Post.h"

class Town: public Post
{
public:
    explicit Town() = default;
    explicit Town(const QJsonObject& town);
    ~Town() = default;

    int setPopulation(int population) {
        population_ = population;
        return population_;
    }
    int setGoods(int goods) {
        product_ = goods;
        return product_;
    }

    int armor();
    int armorCapacity();
    int level();
    int nextLevelPrice();
    int population();
    int populationCapacity();
    int product();
    int productCapacity();
    int trainCooldown();
    QString playerIdx();

    void addProduct(int amount) {
        product_ += amount;
        if (product_ < 0) product_ = 0;
    };

private:
    int armor_;
    int armorCapacity_;
    int level_;
    int nextLevelPrice_;
    QString playerIdx_;
    int population_;
    int populationCapacity_;
    int product_;
    int productCapacity_;
    int trainCooldown_;

};

#endif // TOWN_H
