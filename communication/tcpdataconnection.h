#ifndef TCPDATACONNECTION_H
#define TCPDATACONNECTION_H

#include "abstractdataconnection.h"
#include <QDataStream>

class TcpDataConnection : public AbstractDataConnection
{
public:

    TcpDataConnection(QObject *parent = 0);
    TcpDataConnection(const QString& ipAddress, quint16 portNum, QObject *parent = 0);

    virtual void deInitConnection();
    virtual bool initConnection(QString ipAddress, quint16 portNum);
    virtual void receiveDataPublished(Broker::DataCollection *dataCollection);
    virtual void provideDataConsumed(Broker::DataCollection *dataCollection);
    virtual bool identifiedPacketOverrun();

private:

    QDataStream *stream = nullptr;

private slots:

    void readData();



};

#endif // TCPDATACONNECTION_H
