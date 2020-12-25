#include "Request.h"

Request::Request(enum Action actionCode,const QJsonObject& data){
    QJsonDocument doc(data);
    actionCode_ = actionCode;
    data_ = data;
    dataLength_ = doc.toJson(QJsonDocument::Compact).size();
}

//Request::Request(enum Action actionCode,const char data){
//    actionCode_ = actionCode;
//    dataBlank_ = data;
//    dataLength_ = 0;
//    qDebug() << dataBlank_;
//}

Action& Request::action(){
    return actionCode_;
}

size_t& Request::size(){
    return dataLength_;
}

QJsonObject& Request::data(){
    return data_;
}
