#ifndef REQUEST_H
#define REQUEST_H
#include "ActionEnum.h"
#include <QJsonObject>
#include <QJsonDocument>

class Request
{
public:
    Request();
    Request(Action actionCode, QJsonObject data);
    Action getAction();
    size_t getSize();
    QJsonObject getData();
private:
    Action actionCode;
    size_t dataLength;
    QJsonObject data;
};

#endif // REQUEST_H
