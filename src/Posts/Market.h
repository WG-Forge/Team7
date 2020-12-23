#ifndef MARKET_H
#define MARKET_H

#include "Post.h"

class Market: public Post
{
public:
    explicit Market() = default;
    explicit Market(const QJsonObject& market);
    ~Market() = default;

    int takeProduct(int capacity) {
        int amount = 0;
        if (product_ < capacity) {
            amount = product_;
            product_ = 0;
        } else {
            amount = capacity;
            product_ -= capacity;
        }

        return amount;
    };

    int product();
    int productCapacity();
    int replenishment();

    void update(const QJsonObject& data);
private:
    int product_;
    int productCapacity_;
    int replenishment_;

};

#endif // MARKET_H
