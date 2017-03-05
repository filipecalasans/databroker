#ifndef TCPCONTROLCONNECTION_H
#define TCPCONTROLCONNECTION_H

#include "abstractcontrolconnection.h"

#include <QDataStream>
#include <QTcpServer>

class TcpControlConnection : public AbstractControlConnection
{
public:

    TcpControlConnection(QObject *parent = 0);
    TcpControlConnection(quint16 portNum, QObject *parent = 0);

    virtual ~TcpControlConnection();

    virtual void deInitConnection();
    virtual bool initConnection(quint16 portNum);

    virtual bool receiveControlCommand(Broker::ControlCommand *cmd);
    virtual bool sendControlCommand(Broker::ControlCommand *cmd);

private:

    QDataStream *stream = nullptr;
    QTcpServer *server = nullptr;

private slots:

    void readData();
    void handleClientStateChanged(QAbstractSocket::SocketState state);

};

#endif // TcpControlConnection_H
