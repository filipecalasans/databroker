#include "tcpdataconnection.h"

#include <QTcpSocket>
#include <QDataStream>
#include <QDebug>

TcpDataConnection::TcpDataConnection(QObject *parent) : AbstractDataConnection(parent)
{
    socket = new QTcpSocket();
    stream = new QDataStream(socket);

    stream->setVersion(QDataStream::Qt_5_7);

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

TcpDataConnection::~TcpDataConnection()
{
    if(socket) {
        delete socket;
    }
    delete stream;
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

bool TcpDataConnection::receiveDataPublished(Broker::DataCollection *dataCollection)
{
    bool ret = dataCollection->ParseFromArray(static_buffer_in, len);
    bufferReady = false;
    return ret;
}

bool TcpDataConnection::provideDataConsumed(Broker::DataCollection *dataCollection)
{
    int len = dataCollection->ByteSize();

    if(len <= MAX_BUFFER_SIZE) {
        dataCollection->SerializeToArray(static_buffer_out, MAX_BUFFER_SIZE);
        if(stream->writeRawData(static_buffer_out, len) == len) {
            qDebug() << "[TcpDataConnection::provideDataConsumed] data sent, len:" << len;
            return true;
        }

        return false;
    }

    qDebug() << "[TcpDataConnection::provideDataConsumed] data sent, len:" << len;

    return false;
}

void TcpDataConnection::readData()
{
    while(1) {

        bool emitOverrun = false;

        stream->startTransaction();

        (*stream) >> len;

        if(!stream->commitTransaction()) {
            return;
        }

        if(len > MAX_BUFFER_SIZE) {
            stream->readRawData(static_buffer_in, len);
            return;
        }

        int read_len = stream->readRawData(static_buffer_in, len);

        qDebug() << "[TcpDataConnection::readData] Data received" << len;

        if(read_len == len) {

            if(bufferReady) {
                emitOverrun = true;
                qDebug() << "[TcpDataConnection::readData] Overrun ocurred";
            }


            if(emitOverrun) {
                bufferReady = false;
                emit overrunIdentified();
            }
            else {
                bufferReady = true;
                emit receivedDataPublished();
            }
        }
    }
}
