#include "lockin7270.h"
#include <QtDebug>
#include <stdlib.h>
#include <QtWidgets>
#include <QtNetwork/QtNetwork>
#include <QtCore>


Lockin7270::Lockin7270()
{

}

Lockin7270::~Lockin7270()
{
    socket->close();
}


void Lockin7270::start(QString ip_string, int this_port) {
    ip_address_string = ip_string;
    port = this_port;
    socket = new QTcpSocket(0);
    host_address.setAddress(ip_address_string);
}

void Lockin7270::send_command(QString command) {
    socket->connectToHost(host_address,port,QTcpSocket::ReadWrite);
    socket->waitForConnected(20);
    QByteArray buffer;
    buffer.append(command);
    socket->write(buffer);
    socket->flush();
}

QString Lockin7270::read_response()
{
    QString response = "Error: no lines";
    socket->waitForReadyRead(20);
    while (socket->canReadLine())
    {
        QByteArray ba = socket->readLine();
        if(strcmp(ba.constData(), "\0")==0)
        {
            socket->disconnectFromHost();
            break;
        }
        response.clear();
        response.append(ba);
    }
    socket->disconnectFromHost();
    return response;
}

void Lockin7270::test() {
//    send_command("ID\0");
//    QString response = read_response();
//    int i=0;
}

double Lockin7270::get_x1(){
    send_command("X1.\0");
    QString response = read_response();
    response.chop(1);
    double result = response.toDouble();
    return result;
}

double Lockin7270::get_x2(){
    send_command("X2.\0");
    QString response = read_response();
    response.chop(1);
    double result = response.toDouble();
    return result;
}

void Lockin7270::socketError(QAbstractSocket::SocketError )
{
    qDebug()<<"error" ;
}

