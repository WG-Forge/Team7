#ifndef STORAGE_H
#define STORAGE_H

#include "Post.h"

class Storage: public Post
{
public:
    explicit Storage() = default;
    explicit Storage(const QJsonObject& market);
    ~Storage() = default;

    int takeArmor(int capacity) {
        int amount = 0;
        if (armor_ < capacity) {
            amount = armor_;
            armor_ = 0;
        } else {
            amount = capacity;
            armor_ -= capacity;
        }

        return amount;
    };

    int armor();
    int armorCapacity();
    int replenishment();

    void update(const QJsonObject& data);

private:
    int armor_;
    int armorCapacity_;
    int replenishment_;

};

#endif // STORAGE_H
