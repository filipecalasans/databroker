#ifndef TCPCONTROLCONNECTION_H
#define TCPCONTROLCONNECTION_H

#include "abstractrlconnection.h"

#include <QDataStream>

class TcpControlConnection : public AbstractControlConnection
{
public:

    explicit TcpControlConnection(QObject *parent = 0);
    explicit TcpControlConnection(const QString& ipAddress, quint16 portNum, QObject *parent = 0);

    virtual ~TcpControlConnection();

    virtual void deInitConnection();
    virtual bool initConnection(QString ipAddress, quint16 portNum);
    virtual bool receiveControlCommand(Broker::ControlCommand *cmd);
    virtual bool sendControlCommand(Broker::ControlCommand *cmd);

private:

    QDataStream *stream = nullptr;
    QDataStream *streamOut = nullptr;
    QByteArray *dataOut = nullptr;

    quint64 count;

private slots:

    void readData();

};

#endif // TcpControlConnection_H
