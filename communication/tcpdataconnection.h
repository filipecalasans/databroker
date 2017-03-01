#ifndef TCPDATACONNECTION_H
#define TCPDATACONNECTION_H

#include "abstractdataconnection.h"

#include <QDataStream>

class TcpDataConnection : public AbstractDataConnection
{
public:

    explicit TcpDataConnection(QObject *parent = 0);
    explicit TcpDataConnection(const QString& ipAddress, quint16 portNum, QObject *parent = 0);

    virtual ~TcpDataConnection();

    virtual void deInitConnection();
    virtual bool initConnection(QString ipAddress, quint16 portNum);
    virtual bool receiveDataPublished(Broker::DataCollection *dataCollection);
    virtual bool provideDataConsumed(Broker::DataCollection *dataCollection);

private:

    QDataStream *stream = nullptr;
    quint64 count;

private slots:

    void readData();

};

#endif // TCPDATACONNECTION_H
