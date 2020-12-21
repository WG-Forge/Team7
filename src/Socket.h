#ifndef SOCKET_H
#define SOCKET_H

#include "Request.h"
#include "Response.h"

#include <QObject>
#include <QTcpSocket>
#include <QtDebug>
#include <QTextCodec>
#include <iostream>
#include <iomanip>
#include <cstdint>

class Socket : public QTcpSocket
{
    Q_OBJECT
public:
//    explicit Socket(QObject *parent = nullptr);
    explicit Socket(QTcpSocket *parent = nullptr);

    void connect();
    void close();
    void sendData(Request request);
    QJsonObject getData();

private:
    QTcpSocket *socket;
    const QString SERVER_ADDR = "wgforge-srv.wargaming.net";
    const QString SERVER_IP = "92.223.2.79";
    const int SERVER_PORT = 443;
};

#endif // SOCKET_H
