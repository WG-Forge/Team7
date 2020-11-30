#ifndef SOCKETTEST_H
#define SOCKETTEST_H

#include "Request.h"
#include "Response.h"

#include <QObject>
#include <QTcpSocket>
#include <QtDebug>
#include <QTextCodec>
#include <iostream>
#include <iomanip>
#include <cstdint>

class SocketTest : public QObject
{
    Q_OBJECT
public:
    explicit SocketTest(QObject *parent = nullptr);

    void Connect();
    void Close();
    void sendData(Request request);
    QJsonObject getData();

    
 

private:
    QTcpSocket *socket;
    const QString SERVER_ADDR = "wgforge-srv.wargaming.net";
    const QString SERVER_IP = "92.223.2.79";
    const int SERVER_PORT = 443;
};

#endif // SOCKETTEST_H
