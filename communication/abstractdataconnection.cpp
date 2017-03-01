#include "abstractdataconnection.h"

AbstractDataConnection::AbstractDataConnection(QObject *parent) : QObject(parent)
{
    static_buffer_in = new char[MAX_BUFFER_SIZE];
    static_buffer_out  = new char[MAX_BUFFER_SIZE];
}

AbstractDataConnection::AbstractDataConnection(const QString& ip,
                                               quint16 port,
                                               QObject *parent) :
    AbstractDataConnection(parent)
{
    setIp(ip);
    setPort(port);
}

AbstractDataConnection::~AbstractDataConnection()
{
    if(socket) {
        delete socket;
    }

    delete []static_buffer_in;
    delete []static_buffer_out;
}

bool AbstractDataConnection::getIsReady() const
{
    return isReady;
}

void AbstractDataConnection::setIsReady(bool value)
{
    if(value != isReady) {
        isReady = value;
        connectionReadyChanged(isReady);
    }
}

QString AbstractDataConnection::getIp() const
{
    return ip;
}

quint16 AbstractDataConnection::getPort() const
{
    return port;
}

void AbstractDataConnection::setPort(const quint16 &value)
{
    port = value;
}

void AbstractDataConnection::setIp(const QString &value)
{
    ip = value;
}
