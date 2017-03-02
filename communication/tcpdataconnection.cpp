#include "tcpdataconnection.h"

#include <QTcpSocket>
#include <QDataStream>
#include <QDebug>

TcpDataConnection::TcpDataConnection(QObject *parent) : AbstractDataConnection(parent)
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
    delete streamOut;
}

void TcpDataConnection::deInitConnection()
{
    if(socket) {
        socket->abort();
    }

    bufferReady = false;

    if(stream) {
        stream->resetStatus();
    }
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
            qDebug() << "[TcpDataConnection::provideDataConsumed] len =" << writtenBytes-integerSize;
            return true;
        }

        qDebug() << "[TcpDataConnection::provideDataConsumed] Error len != len_sent";
        return false;
    }

    qDebug() << "[TcpDataConnection::provideDataConsumed] error, len > MAX_BUFFER_SIZE";
    return false;
}

void TcpDataConnection::readData()
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
            emit receivedDataPublished();
        }

    }
}
