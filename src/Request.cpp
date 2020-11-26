#include "Request.h"

Request::Request()
{

}
Request::Request(Action actionCode, QJsonObject data){
    QJsonDocument doc(data);
    this->actionCode = actionCode;
    this->data = data;
    this->dataLength = doc.toJson(QJsonDocument::Compact).size();
}
Action Request::getAction(){
    return this->actionCode;
}
size_t Request::getSize(){
    return this->dataLength;
}
QJsonObject Request::getData(){
    return this->data;
}
