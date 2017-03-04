#include "tcpcontrolconnection.h"

#include <QTcpSocket>
#include <QDataStream>
#include <QDebug>

TcpControlConnection::TcpControlConnection(QObject *parent) : AbstractControlConnection(parent)
{
    socket = new QTcpSocket();
    stream = new QDataStream(socket);

    dataOut = new QByteArray();
    streamOut = new QDataStream(dataOut, QIODevice::ReadWrite);

    stream->setVersion(QDataStream::Qt_5_7);
    streamOut->setVersion(QDataStream::Qt_5_7);

    connect(socket, &QTcpSocket::stateChanged, [this](QAbstractSocket::SocketState socketState) {
        if(socketState == QAbstractSocket::ConnectedState) {
            setIsReady(true);
        }
        else {
            setIsReady(false);
        }
    });

    connect(socket, &QTcpSocket::readyRead, this, &TcpControlConnection::readData);
}

TcpControlConnection::TcpControlConnection(
        const QString& ipAddress, quint16 portNum, QObject *parent) :
    TcpControlConnection(parent)
{
    setIp(ipAddress);
    setPort(portNum);
}

TcpControlConnection::~TcpControlConnection()
{
    if(socket) {
        delete socket;
    }
    delete stream;
    delete streamOut;
}

void TcpControlConnection::deInitConnection()
{
    if(socket) {
        socket->abort();
    }

    bufferReady = false;

    if(stream) {
        stream->resetStatus();
    }
}

bool TcpControlConnection::initConnection(QString ipAddress, quint16 portNum)
{
    socket->connectToHost(ipAddress, portNum);
    return true;
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
            qDebug() << "[TcpControlConnection::provideDataConsumed] len =" << writtenBytes-integerSize;
            return true;
        }

        qDebug() << "[TcpControlConnection::provideDataConsumed] Error len != len_sent";
        return false;
    }

    qDebug() << "[TcpControlConnection::provideDataConsumed] error, len > MAX_BUFFER_SIZE";
    return false;
}

void TcpControlConnection::readData()
{
    while(1) {

        bool emitOverrun = false;

        stream->startTransaction();

        quint32 packet_len = 0;

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
            emit receivedControlCmd();
        }
    }
}
