#ifndef TCPDATACONNECTION_H
#define TCPDATACONNECTION_H

#include "abstractdataconnection.h"

#include <QDataStream>
#include <QTcpServer>

class TcpDataConnection : public AbstractDataConnection
{
public:

    TcpDataConnection(QObject *parent = 0);
    TcpDataConnection(quint16 portNum, QObject *parent = 0);

    virtual ~TcpDataConnection();

    virtual void deInitConnection();
    virtual bool initConnection(quint16 portNum);
    virtual bool receiveDataConsumed(Broker::DataCollection *dataCollection);
    virtual bool provideDataPublished(Broker::DataCollection *dataCollection);

private:

    QDataStream *stream = nullptr;
    QTcpServer *server = nullptr;

private slots:

    void readData();
    void handleClientStateChanged(QAbstractSocket::SocketState state);

};

#endif // TCPDATACONNECTION_H
