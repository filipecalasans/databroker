#include "abstractcontrolconnection.h"

AbstractControlConnection::AbstractControlConnection(QObject *parent) : QObject(parent)
{
    static_buffer_in = new char[MAX_BUFFER_SIZE];
    static_buffer_out  = new char[MAX_BUFFER_SIZE];
}

AbstractControlConnection::AbstractControlConnection(quint16 port,
                                               QObject *parent) :
    AbstractControlConnection(parent)
{
    setPort(port);
}

AbstractControlConnection::~AbstractControlConnection()
{
    if(socket) {
        delete socket;
    }
    delete []static_buffer_in;
    delete []static_buffer_out;
}

/*
bool AbstractControlConnection::getIsReady() const
{
    return isReady;
}

void AbstractControlConnection::setIsReady(bool value)
{
    if(value != isReady) {
        isReady = value;
        connectionReadyChanged(isReady);
    }
}*/

QString AbstractControlConnection::getIp() const
{
    return ip;
}

void AbstractControlConnection::setIp(const QString &value)
{
    ip = value;
}

quint16 AbstractControlConnection::getPort() const
{
    return port;
}

void AbstractControlConnection::setPort(const quint16 &value)
{
    port = value;
}
