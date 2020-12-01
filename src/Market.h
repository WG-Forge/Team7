#ifndef MARKET_H
#define MARKET_H

#include "Post.h"

class Market: public Post
{
public:
    explicit Market() = default;
    explicit Market(const QJsonObject& market);
    ~Market() = default;
    
    int point_idx();
    int product();
    int product_capacity();
    int replenishment();

private:
    int idx_;
    QString name_;
    enum PostType type_;
    std::vector<Event*> events_;
    int point_idx_;
    int product_;
    int product_capacity_;
    int replenishment_;

};

#endif // MARKET_H
