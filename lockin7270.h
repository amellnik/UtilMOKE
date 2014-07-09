#ifndef LOCKIN7270_H
#define LOCKIN7270_H
#include <stdlib.h>
#include <QtWidgets>
#include <QtNetwork/QtNetwork>
#include <QtCore>


class Lockin7270
{
public:
    QString  ip_address_string;
    QHostAddress host_address;
    Lockin7270();
    ~Lockin7270();
    void test();
    void start(QString ip_string, int this_port);
    double get_x1();
    double get_x2();
    QTcpSocket *socket;
    int port;

private:
    QString read_response();
    void send_command(QString command);
    void socketError(QAbstractSocket::SocketError);

};

#endif // LOCKIN7270_H
