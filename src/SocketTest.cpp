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
}

void SocketTest::Close() {
    socket->close();
}

void SocketTest::sendData(QString data, qint32 actionCode)
{
    QByteArray sData;
    qint32 size = data.size();

    int index = 0;

    //конверт action кода
    for (int i = 0; i < 4; ++i, ++index)
    {
//        qDebug() << (unsigned char)(code & 0xFF);

        sData.append((unsigned char)(actionCode & 0xFF));
        actionCode >>= 8;
    }

    // конверт размера
    for (int i = 0; i < 4; ++i, ++index)
    {
//        qDebug() << (unsigned char)(size & 0xFF);

        sData.append((unsigned char)(size & 0xFF));
        size >>= 8;
    }

    sData.append(data.toUtf8());

    if (socket->waitForConnected())
    {
        socket->write(sData);
        qDebug() << "sent: " << socket->waitForBytesWritten();
    }
    else
    {
        qDebug() << "Not Connected!";
    }
}

void SocketTest::getData() {
    // респонс код
    socket->waitForReadyRead();
    QString responseCode = socket->readAll();

//    qDebug() << "Response Code: " << socket->readAll();


    // дата
    socket->waitForReadyRead();
    QString responseData = socket->readAll();

//    qDebug() << "Response Data: " << socket->readAll();

}
