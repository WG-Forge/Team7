#ifndef SOCKETTEST_H
#define SOCKETTEST_H

#include <QObject>
#include <QTcpSocket>
#include <QtDebug>

class SocketTest : public QObject
{
    Q_OBJECT
public:
    explicit SocketTest(QObject *parent = nullptr);

    void Connect();
    void Close();
    void sendData(QString data, qint32 actionCode);
    void getData();

    enum Action
    {
        LOGIN = 1,
        LOGOUT = 2,
        MOVE = 3,
        UPGRADE = 4,
        TURN = 5,
        PLAYER = 6,
        GAMES = 7,
        MAP = 10
    };
    enum Result
    {
        OKEY = 0,
        BAD_COMMAND = 1,
        RESOURCE_NOT_FOUND = 2,
        ACCESS_DENIED = 3,
        INAPPROPRIATE_GAME_STATE = 4,
        TIMEOUT = 5,
        INTERNAL_SERVER_ERROR = 500
    };

private:
    QTcpSocket *socket;
    const QString SERVER_ADDR = "wgforge-srv.wargaming.net";
    const QString SERVER_IP = "92.223.2.79";
    const int SERVER_PORT = 443;
};

#endif // SOCKETTEST_H
