#include "tcpcontrolconnection.h"

#include <QTcpSocket>
#include <QDataStream>

#include <QTcpServer>
#include <QDebug>

TcpControlConnection::TcpControlConnection(QObject *parent) : AbstractControlConnection(parent)
{
    server = new QTcpServer();
    stream = new QDataStream();

    stream->setVersion(QDataStream::Qt_5_7);

    connect(server, &QTcpServer::newConnection, [this]() {
        if(server->hasPendingConnections() && socket == nullptr) {
            socket = server->nextPendingConnection();

            qDebug() << "[New Client]" << socket->localAddress();

            stream->setDevice(socket);
            connect(socket, &QTcpSocket::stateChanged, this, &TcpControlConnection::handleClientStateChanged);
            connect(socket, &QTcpSocket::readyRead, this, &TcpControlConnection::readData);
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

TcpControlConnection::TcpControlConnection(quint16 portNum, QObject *parent) :
    TcpControlConnection(parent)
{
    setPort(portNum);
}

TcpControlConnection::~TcpControlConnection()
{
    delete stream;
    delete server;
}

void TcpControlConnection::deInitConnection()
{
    if(socket) {
        socket->abort();
    }
    server->close();
    bufferReady = false;
    stream->resetStatus();
    stream->setDevice(0);
}

bool TcpControlConnection::initConnection(quint16 portNum)
{
    deInitConnection();
    return server->listen(QHostAddress::Any, portNum);;
}

bool TcpControlConnection::receiveControlCommand(Broker::ControlCommand *cmd)
{
    bool ret = cmd->ParseFromArray(static_buffer_in, len);
    bufferReady = false;
    return ret;

}

bool TcpControlConnection::sendControlCommand(Broker::ControlCommand *cmd)
{
    quint32 writeLen = cmd->ByteSize();

    QByteArray array;
    QDataStream integer(&array, QIODevice::ReadWrite);

    if(writeLen <= MAX_BUFFER_SIZE) {
        integer << writeLen;
        memcpy(static_buffer_out, array.data(), array.size());
        int integerSize = array.size();
        writeLen += integerSize;
        cmd->SerializeToArray(static_buffer_out+integerSize, MAX_BUFFER_SIZE);
        int writtenBytes = socket->write(static_buffer_out, writeLen);
        if(writeLen == (quint64)writtenBytes) {
            qDebug() << "[TcpControlConnection::provideDataPublished] len =" << writtenBytes-integerSize;
            return true;
        }

        qDebug() << "[TcpControlConnection::provideDataPublished] Error len != len_sent";
        return false;
    }

    qDebug() << "[TcpControlConnection::provideDataPublished] error, len > MAX_BUFFER_SIZE";
    return false;

}

void TcpControlConnection::readData()
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

        qDebug() << "[TcpControlConnection::readData] Data received" << read_len;

        if((quint64)read_len == packet_len) {
            len = packet_len;
            if(bufferReady) {
                emitOverrun = true;
                qDebug() << "[TcpControlConnection::readData] Overrun ocurred";
            }

            if(emitOverrun) {
                emit overrunIdentified();
            }

            bufferReady = true;
            emit receivedControlCommand();
        }
    }
}

void TcpControlConnection::handleClientStateChanged(QAbstractSocket::SocketState state)
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
