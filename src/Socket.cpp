#include "Socket.h"

Socket::Socket(QObject *parent) : QObject(parent){}

void Socket::Connect() {
    socket = new QTcpSocket(this);
    socket->connectToHost(SERVER_ADDR, SERVER_PORT);
    if(!socket->waitForConnected()){
        qDebug() << "Not Connected!";
    }
}

void Socket::Close() {
    socket->close();
}

void Socket::sendData(Request request)
{
    QByteArray sData;
    QJsonDocument doc(request.data());
    qint32 size = doc.toJson(QJsonDocument::Compact).size();
    int actionCode = static_cast<int>(request.action());
    for (int i = 0; i < 4; ++i)
    {
        sData.append((unsigned char)(actionCode & 0xFF));
        actionCode >>= 8;
    }

    for (int i = 0; i < 4; ++i)
    {
        sData.append((unsigned char)(size & 0xFF));
        size >>= 8;
    }

    sData.append(doc.toJson(QJsonDocument::Compact));
    socket->write(sData);
    socket->waitForBytesWritten(3000);
}

QJsonObject Socket::getData() {
    QByteArray response;
    QJsonDocument doc;

    do {
        socket->waitForReadyRead();
        response += socket->readAll();

        doc = QJsonDocument::fromJson(response.mid(8));
    }
    while (doc.isNull());

    return doc.object();
}
