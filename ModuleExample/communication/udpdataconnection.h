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
    virtual bool initConnection(quint16 portNum);
    virtual bool provideDataPublished(Broker::DataCollection *dataCollection);
    virtual bool receiveDataConsumed(Broker::DataCollection *dataCollection);

    void initConnection(const QString& ipAddress, quint16 portNum);
    virtual void setIp(const QString &value);

    quint16 getBrokerPort() const;
    void setBrokerPort(const quint16 &value);

    QString getBrokerIp() const;
    void setBrokerIp(const QString &value);

private slots:

    void handleDatagram();

private:

    quint16 brokerPort;
    QString brokerIp;

    static const int MAX_DATAGRAM_SIZE = 1000;
};

#endif // UDPDATACONNECTION_H
