#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QObject>
#include <QDebug>

#ifdef TCP_CONNECTION
#include "tcpdataconnection.h"
#else
#include "udpdataconnection.h"
#endif

#include "tcpcontrolconnection.h"

class Communication : public QObject
{
    Q_OBJECT
public:

    explicit Communication(quint16 dataPort, quint16 controlPort, QObject *parent = 0);

    void timerEvent(QTimerEvent *event);

    bool sendControlCommand(Broker::ControlCommand *command);

    QString getModuleName() const;
    void setModuleName(const QString &value);

protected:

    bool initDataConnection();
    bool initControlConnection();

signals:

    void masterStateChanged(const TcpControlConnection::MasterControlStateType &value);

public slots:

private:

    quint16 dataPort, controlPort;
    QString moduleName;

#ifdef TCP_CONNECTION
    TcpDataConnection *dataConnection = nullptr;
#else
    UdpDataConnection *dataConnection = nullptr;
#endif

    TcpControlConnection *controlConnection = nullptr;
};

#endif // COMMUNICATION_H
