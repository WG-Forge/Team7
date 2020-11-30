#ifndef RESPONSE_H
#define RESPONSE_H
#include <QJsonObject>
#include "Enums/ResultEnum.h"
class Response
{
public:
   explicit Response() = default;
   explicit Response(const QJsonObject &response);
private:
    QJsonObject response_;
};

#endif // RESPONSE_H
