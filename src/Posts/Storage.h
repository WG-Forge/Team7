#ifndef STORAGE_H
#define STORAGE_H

#include "Post.h"

class Storage: public Post
{
public:
    explicit Storage() = default;
    explicit Storage(const QJsonObject& market);
    ~Storage() = default;

    int armor();
    int armorCapacity();
    int replenishment();

private:
    int armor_;
    int armorCapacity_;
    int replenishment_;

};

#endif // STORAGE_H
