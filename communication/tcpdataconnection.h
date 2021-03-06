#ifndef TCPDATACONNECTION_H
#define TCPDATACONNECTION_H

#include "abstractdataconnection.h"

#include <QDataStream>

class TcpDataConnection : public AbstractDataConnection
{
    Q_OBJECT
public:

    explicit TcpDataConnection(QObject *parent = 0);
    explicit TcpDataConnection(const QString& ipAddress, quint16 portNum, QObject *parent = 0);

    virtual ~TcpDataConnection();

    virtual bool initConnection();
    virtual void deInitConnection();
    virtual bool initConnection(QString ipAddress, quint16 portNum);
    virtual bool receiveDataPublished(Broker::DataCollection *dataCollection);
    virtual bool provideDataConsumed(Broker::DataCollection *dataCollection);

private:

    QDataStream *stream = nullptr;
    QDataStream *streamOut = nullptr;
    QByteArray *dataOut = nullptr;

    quint64 count;

private slots:

    void readData();

};

#endif // TCPDATACONNECTION_H
