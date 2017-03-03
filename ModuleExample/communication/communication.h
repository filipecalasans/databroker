#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QObject>
#include <QDebug>

#include "tcpdataconnection.h"
#include "udpdataconnection.h"

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

};

#endif // COMMUNICATION_H
