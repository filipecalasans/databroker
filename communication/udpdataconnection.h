#ifndef UDPDATACONNECTION_H
#define UDPDATACONNECTION_H

#include "abstractdataconnection.h"

class UdpDataConnection : public AbstractDataConnection
{

public:

    UdpDataConnection(QObject *parent = 0);
    UdpDataConnection(const QString& ipAddress, quint16 portNum, QObject *parent = 0);

    ~UdpDataConnection();

    virtual void deInitConnection();
    virtual bool initConnection(QString ipAddress, quint16 portNum);
    virtual bool receiveDataPublished(Broker::DataCollection *dataCollection);
    virtual bool provideDataConsumed(Broker::DataCollection *dataCollection);

private slots:

    void handleDatagram();

private:

    static const int MAX_DATAGRAM_SIZE = 1000;
};

#endif // UDPDATACONNECTION_H
