#ifndef REQUEST_H
#define REQUEST_H
#include "Enums/ActionEnum.h"
#include <QJsonObject>
#include <QJsonDocument>

class Request
{
public:
    explicit Request() = default;
    explicit Request(Action actionCode,const QJsonObject& data);
    explicit Request(Action actionCode,const char data);

    Action& action();
    size_t& size();
    QJsonObject& data();
private:
    enum Action actionCode_;
    size_t dataLength_;
    QJsonObject data_;
    char dataBlank_;
};

#endif // REQUEST_H
