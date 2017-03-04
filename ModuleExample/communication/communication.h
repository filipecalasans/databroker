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
    explicit Communication(QObject *parent = 0);

    void timerEvent(QTimerEvent *event);

signals:

public slots:

private:

#ifdef TCP_CONNECTION
    TcpDataConnection *dataConnection = nullptr;
#else
    UdpDataConnection *dataConnection = nullptr;
#endif

    TcpControlConnection *controlConnection = nullptr;
};

#endif // COMMUNICATION_H
