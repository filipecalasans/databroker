#ifndef ABSTRACTCONTROLCONNECTION_H_
#define ABSTRACTCONTROLCONNECTION_H_

#include <QObject>
#include <QAbstractSocket>
#include <QMutex>

#include "../protocol/control.pb.h"

class AbstractControlConnection : public QObject
{
    Q_OBJECT
public:

    static const int MAX_BUFFER_SIZE = 2048;

    explicit AbstractControlConnection(QObject *parent = 0);
    explicit AbstractControlConnection(const QString& ip, quint16 port, QObject *parent = 0);

    virtual ~AbstractControlConnection();

    /* Define the interface for this class */
    virtual bool initConnection();

    virtual void deInitConnection() = 0;
    virtual bool initConnection(QString ipAddress, quint16 portNum) = 0;
    virtual bool sendControlCommand(Broker::ControlCommand *cmd) = 0;
    virtual bool receiveControlCommand(Broker::ControlCommand *cmd) = 0;

    QString getIp() const;
    void setIp(const QString &value);

    quint16 getPort() const;
    void setPort(const quint16 &value);

signals:

    void receivedControlCommand();
    void overrunIdentified();
    void connectionReadyChanged(bool ready);

protected:

    QString ip;
    quint16 port = 0;
    QAbstractSocket *socket = nullptr;

    bool bufferReady = false;

    char *static_buffer_in=nullptr, *static_buffer_out=nullptr;
    quint32 len = 0;

};

#endif // AbstractControlConnection_H
