#ifndef STORAGE_H
#define STORAGE_H

#include "Post.h"

class Storage: public Post
{
public:
    explicit Storage() = default;
    explicit Storage(const QJsonObject& market);
    ~Storage() = default;


    int point_idx();
    int armor();
    int armor_capacity();
    int replenishment();

private:
    int idx_;
    QString name_;
    enum PostType type_;
    std::vector<Event*> events_;
    int point_idx_;
    int armor_;
    int armor_capacity_;
    int replenishment_;

};

#endif // STORAGE_H
