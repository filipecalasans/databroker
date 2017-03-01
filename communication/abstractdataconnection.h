#ifndef ABSTRACTDATACONNECTION_H
#define ABSTRACTDATACONNECTION_H

#include <QObject>
#include <QAbstractSocket>
#include <QMutex>

#include "../protocol/data.pb.h"

class AbstractDataConnection : public QObject
{
    Q_OBJECT
public:

    static const int MAX_BUFFER_SIZE = 2048;

    explicit AbstractDataConnection(QObject *parent = 0);
    explicit AbstractDataConnection(const QString& ip, quint16 port, QObject *parent = 0);

    virtual ~AbstractDataConnection();

    /* Define the interface for this class */
    bool initConnection() {
        deInitConnection();
        return initConnection(ip, port);
    }

    virtual void deInitConnection() = 0;
    virtual bool initConnection(QString ipAddress, quint16 portNum) = 0;
    virtual bool receiveDataPublished(Broker::DataCollection *dataCollection) = 0;
    virtual bool provideDataConsumed(Broker::DataCollection *dataCollection) = 0;

    bool getIsReady() const;
    void setIsReady(bool value);

    QString getIp() const;
    void setIp(const QString &value);

    quint16 getPort() const;
    void setPort(const quint16 &value);

signals:

    void receivedDataPublished();
    void overrunIdentified();
    void connectionReadyChanged(bool ready);

protected:

    QString ip;
    quint16 port = 0;
    QAbstractSocket *socket = nullptr;
    bool isReady = false;

    bool bufferReady = false;

    char *static_buffer_in=nullptr, *static_buffer_out=nullptr;
    quint32 len = 0;

};

#endif // ABSTRACTDATACONNECTION_H
