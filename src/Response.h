#ifndef RESPONSE_H
#define RESPONSE_H
#include <QJsonObject>
#include "ResultEnum.h"
class Response
{
public:
    Response();
    Response(QJsonObject response);
private:
    QJsonObject response;
};

#endif // RESPONSE_H
