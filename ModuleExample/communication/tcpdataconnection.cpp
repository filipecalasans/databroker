#include "tcpdataconnection.h"

#include <QTcpSocket>
#include <QDataStream>

#include <QTcpServer>
#include <QDebug>

TcpDataConnection::TcpDataConnection(QObject *parent) : AbstractDataConnection(parent)
{
    server = new QTcpServer();
    stream = new QDataStream();

    stream->setVersion(QDataStream::Qt_5_7);

    connect(server, &QTcpServer::newConnection, [this]() {
        if(server->hasPendingConnections() && socket == nullptr) {
            socket = server->nextPendingConnection();

            qDebug() << "[New Client]" << socket->localAddress();

            stream->setDevice(socket);
            connect(socket, &QTcpSocket::stateChanged, this, &TcpDataConnection::handleClientStateChanged);
            connect(socket, &QTcpSocket::readyRead, this, &TcpDataConnection::readData);
            setIsReady(true);
        }
        else {
            QTcpSocket *discard = server->nextPendingConnection();
            discard->abort();
            discard->deleteLater();

            qDebug() << "[Dropping Client] I am already attending a client";
        }
    });
}

TcpDataConnection::TcpDataConnection(quint16 portNum, QObject *parent) :
    TcpDataConnection(parent)
{
    setPort(portNum);
}

TcpDataConnection::~TcpDataConnection()
{
    delete stream;
    delete server;
}

void TcpDataConnection::deInitConnection()
{
    if(socket) {
        socket->abort();
    }
    server->close();
    bufferReady = false;
    stream->resetStatus();
    stream->setDevice(0);
}

bool TcpDataConnection::initConnection(quint16 portNum)
{
    deInitConnection();
    return server->listen(QHostAddress::Any, portNum);;
}

bool TcpDataConnection::receiveDataConsumed(Broker::DataCollection *dataCollection)
{
    bool ret = dataCollection->ParseFromArray(static_buffer_in, len);
    bufferReady = false;
    return ret;
}

bool TcpDataConnection::provideDataPublished(Broker::DataCollection *dataCollection)
{
    quint32 writeLen = dataCollection->ByteSize();

    QByteArray array;
    QDataStream integer(&array, QIODevice::ReadWrite);

    if(writeLen <= MAX_BUFFER_SIZE) {
        integer << writeLen;
        memcpy(static_buffer_out, array.data(), array.size());
        int integerSize = array.size();
        writeLen += integerSize;
        dataCollection->SerializeToArray(static_buffer_out+integerSize, MAX_BUFFER_SIZE);
        int writtenBytes = socket->write(static_buffer_out, writeLen);
        if(writeLen == (quint64)writtenBytes) {
            qDebug() << "[TcpDataConnection::provideDataPublished] len =" << writtenBytes-integerSize;
            return true;
        }

        qDebug() << "[TcpDataConnection::provideDataPublished] Error len != len_sent";
        return false;
    }

    qDebug() << "[TcpDataConnection::provideDataPublished] error, len > MAX_BUFFER_SIZE";
    return false;
}

void TcpDataConnection::readData()
{
    while(1) {

        bool emitOverrun = false;

        stream->startTransaction();

        quint32 packet_len;

        (*stream) >> packet_len;

        if(!stream->commitTransaction() || packet_len == 0) {
            return;
        }

        if(packet_len > MAX_BUFFER_SIZE) {
            stream->readRawData(static_buffer_in, MAX_BUFFER_SIZE);
            return;
        }

        int read_len = stream->readRawData(static_buffer_in, packet_len);

        qDebug() << "[TcpDataConnection::readData] Data received" << read_len;

        if((quint64)read_len == packet_len) {
            len = packet_len;
            if(bufferReady) {
                emitOverrun = true;
                qDebug() << "[TcpDataConnection::readData] Overrun ocurred";
            }

            if(emitOverrun) {
                emit overrunIdentified();
            }

            bufferReady = true;
            emit receivedDataConsumed();
        }
    }
}

void TcpDataConnection::handleClientStateChanged(QAbstractSocket::SocketState state)
{
    if(state == QAbstractSocket::UnconnectedState) {
        if(socket) {
            socket->disconnect();
            socket->deleteLater();
            socket = nullptr;
            setIsReady(false);
        }
    }
    else if(state == QAbstractSocket::ConnectedState) {
        setIsReady(true);
    }
    else {
        setIsReady(false);
    }
}
