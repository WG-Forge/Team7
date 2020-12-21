#include "Socket.h"

Socket::Socket(QTcpSocket *parent) : QTcpSocket(parent){}

void Socket::connect() {
    socket = new QTcpSocket(this);
    socket->connectToHost(SERVER_ADDR, SERVER_PORT);
    if(!socket->waitForConnected()){
        qDebug() << "Not Connected!";
    } else qDebug() << "Connected!";
}

void Socket::close() {
    qDebug() << "Closed";
    socket->close();
}

void Socket::sendData(Request request)
{
    QByteArray sData;
    QJsonDocument doc(request.data());
    qint32 size = doc.toJson(QJsonDocument::Compact).size();
    int defSize = size;
    if (defSize == 2) {
        defSize = 0;
        size = 0;
    }

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

    if (defSize != 0)  {
        sData.append(doc.toJson(QJsonDocument::Compact));
    }
    qDebug() << "Sdata:" << sData;
    socket->write(sData);
    socket->waitForBytesWritten();
}

QJsonObject Socket::getData() {
    QByteArray response;
    QJsonDocument doc;

    do {
        socket->waitForReadyRead();
        response += socket->readAll();

        if (response.size() == 4) continue;
        if (response.mid(8).size() == 0) break;

        doc = QJsonDocument::fromJson(response.mid(8));
    }
    while (doc.isNull());

    return doc.object();
}
