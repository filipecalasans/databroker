#include "tcpdataconnection.h"

#include <QTcpSocket>
#include <QDataStream>

TcpDataConnection::TcpDataConnection(QObject *parent) : AbstractDataConnection(parent)
{
    socket = new QTcpSocket();
    stream = new QDataStream(socket);

    stream->setVersion(QDataStream::Qt_5_4);

    connect(socket, &QTcpSocket::stateChanged, [this](QAbstractSocket::SocketState socketState) {
        if(socketState == QAbstractSocket::ConnectedState) {
            setIsReady(true);
        }
        else {
            setIsReady(false);
        }
    });

    connect(socket, &QTcpSocket::readyRead, this, &TcpDataConnection::readData);
}

TcpDataConnection::TcpDataConnection(
        const QString& ipAddress, quint16 portNum, QObject *parent) :
    TcpDataConnection(parent)
{
    setIp(ipAddress);
    setPort(portNum);
}

void TcpDataConnection::deInitConnection()
{
    socket->abort();
    bufferReady = false;
    stream->resetStatus();
}

bool TcpDataConnection::initConnection(QString ipAddress, quint16 portNum)
{
    socket->connectToHost(ipAddress, portNum);
    return true;
}

void TcpDataConnection::receiveDataPublished(Broker::DataCollection *dataCollection)
{
    dataCollection->ParseFromArray(static_buffer, len);
}

void TcpDataConnection::provideDataConsumed(Broker::DataCollection *dataCollection)
{
    dataCollection->SerializeToArray()
    stream->writeRawData()
}

bool TcpDataConnection::identifiedPacketOverrun()
{
    return false;
}

void TcpDataConnection::readData()
{
    bool emitOverrun = false;

    stream->startTransaction();

    len = stream->readRawData(static_buffer, MAX_BUFFER_SIZE);

    if(!stream->commitTransaction()) {
        return;
    }

    if(bufferReady) {
        emitOverrun = true;
    }

    bufferReady = true;

    if(emitOverrun) {
        emit overrunIdentified();
    }

    emit receivedDataPublished();
}
