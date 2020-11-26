#include "SocketTest.h"

#include <iostream>
#include <iomanip>
#include <cstdint>

SocketTest::SocketTest(QObject *parent) : QObject(parent)
{

}

void SocketTest::Connect() {
    socket = new QTcpSocket(this);
    socket->connectToHost(SERVER_ADDR, SERVER_PORT);
    if(!socket->waitForConnected()){
        qDebug() << "Not Connected!";
    }
}

void SocketTest::Close() {
    socket->close();
}

void SocketTest::sendData(Request request)
{
    QByteArray sData;
    QJsonDocument doc(request.getData());
    qint32 size = doc.toJson(QJsonDocument::Compact).size();
    int actionCode = request.getAction();
    for (int i = 0; i < 4; ++i)
    {
//        qDebug() << (unsigned char)(code & 0xFF);
        sData.append((unsigned char)(actionCode & 0xFF));
        actionCode >>= 8;
    }

    // конверт размера
    for (int i = 0; i < 4; ++i)
    {
//        qDebug() << (unsigned char)(size & 0xFF);
        sData.append((unsigned char)(size & 0xFF));
        size >>= 8;
    }

    sData.append(doc.toJson(QJsonDocument::Compact));
    socket->write(sData);
    socket->waitForBytesWritten(3000);
    //qDebug() << "sent: " << socket->waitForBytesWritten();
}

QJsonObject SocketTest::getData() {
    if(socket->waitForReadyRead(5000)){
        QByteArray response(socket->readAll());
        QString DataAsString = QTextCodec::codecForMib(106)->toUnicode(response);
        int k = DataAsString.indexOf('{');
        DataAsString.remove(0,k);
        QJsonDocument doc = QJsonDocument::fromJson(DataAsString.toUtf8());
        return doc.object();
    }
    else{
       qDebug() << "No data!\n";
    }
}
