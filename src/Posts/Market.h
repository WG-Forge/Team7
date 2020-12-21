#ifndef MARKET_H
#define MARKET_H

#include "Post.h"

class Market: public Post
{
public:
    explicit Market() = default;
    explicit Market(const QJsonObject& market);
    ~Market() = default;

    int product();
    int product_capacity();
    int replenishment();

private:
    int product_;
    int product_capacity_;
    int replenishment_;

};

#endif // MARKET_H
