#include "abstractrlconnection.h"

AbstractControlConnection::AbstractControlConnection(QObject *parent) : QObject(parent)
{
    static_buffer_in = new char[MAX_BUFFER_SIZE];
    static_buffer_out  = new char[MAX_BUFFER_SIZE];
}

AbstractControlConnection::AbstractControlConnection(const QString& ip,
                                               quint16 port,
                                               QObject *parent) :
    AbstractControlConnection(parent)
{
    setIp(ip);
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

bool AbstractControlConnection::initConnection() {
    deInitConnection();
    return initConnection(ip, port);
}

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
}

QString AbstractControlConnection::getIp() const
{
    return ip;
}

quint16 AbstractControlConnection::getPort() const
{
    return port;
}

void AbstractControlConnection::setPort(const quint16 &value)
{
    port = value;
}

void AbstractControlConnection::setIp(const QString &value)
{
    ip = value;
}
