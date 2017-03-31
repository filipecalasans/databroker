#include "udpdataconnection.h"
#include "protocol/data.pb.h"

#include <QUdpSocket>

#include <QDateTime>

UdpDataConnection::UdpDataConnection(QObject *parent) : AbstractDataConnection(parent)
{
    socket = new QUdpSocket();
    connect(socket, &QUdpSocket::readyRead, this, &UdpDataConnection::handleDatagram);
}

UdpDataConnection::UdpDataConnection(const QString &ipAddress,
                                     quint16 portNum, QObject *parent) :
    UdpDataConnection(parent)
{
    setIp(ipAddress);
    setPort(portNum);
}

UdpDataConnection::~UdpDataConnection()
{
    delete socket;
}

bool UdpDataConnection::initConnection()
{
    return AbstractDataConnection::initConnection();
}

void UdpDataConnection::deInitConnection()
{
    socket->abort();
}

bool UdpDataConnection::initConnection(QString ipAddress, quint16 portNum)
{
    setPort(portNum);
    setIp(ipAddress);
    bool ret = socket->bind(0);
    ret = ret && sendFirstDatagram();
    setIsReady(ret);
    return ret;
}

bool UdpDataConnection::receiveDataPublished(Broker::DataCollection *dataCollection)
{
    bool ret = dataCollection->ParseFromArray(static_buffer_in, len);
    bufferReady = false;
    return ret;
}

bool UdpDataConnection::provideDataConsumed(Broker::DataCollection *dataCollection)
{
    int bytesLen = dataCollection->ByteSize();

    Broker::DataCollection header;
    header.set_provider_name(dataCollection->provider_name());
    header.set_timestamp(dataCollection->timestamp());

    int payloadLen = bytesLen - header.ByteSize();

    int index = 0;

    Broker::DataCollection packet;

    if(payloadLen == 0) {
        bool ret = packet.SerializeToArray(static_buffer_out, MAX_DATAGRAM_SIZE);
        qDebug() << "provideDataConsumed" << QString(packet.DebugString().c_str());
        ((QUdpSocket*)socket)->writeDatagram(static_buffer_out, packet.ByteSize(), QHostAddress(ip), port);
        return ret;
    }

    while (payloadLen > 0 && index < dataCollection->data_provided_size()) {

        packet.Clear();
        packet.set_provider_name(dataCollection->provider_name());
        packet.set_timestamp(dataCollection->timestamp());

        int headerLen = packet.ByteSize();

        while(index < dataCollection->data_provided_size()) {
            Broker::Data dataElement = dataCollection->data_provided().Get(index);
            if(packet.ByteSize() + dataElement.ByteSize() < MAX_DATAGRAM_SIZE) {
                Broker::Data *newData = packet.add_data_provided();
                *newData = dataElement;
                index++;
//                qDebug() << "[UdpDataConnection::provideDataConsumed] index"
//                         << index << packet.ByteSize() << dataElement.ByteSize();
            }
            else {
//                qDebug() << "======= BREAK ========";
                break;
            }
        }

        packet.SerializeToArray(static_buffer_out, MAX_DATAGRAM_SIZE);
//        qDebug() << QString(packet.DebugString().c_str());
        ((QUdpSocket*)socket)->writeDatagram(static_buffer_out, packet.ByteSize(), QHostAddress(ip), port);
        payloadLen -= (packet.ByteSize() - headerLen);
    }

    return true;
}

void UdpDataConnection::handleDatagram()
{
    QUdpSocket *udpsocket = (QUdpSocket*)socket;

    qDebug() << "[UdpDataConnection::provideDataConsumed] received datagram";

    while(udpsocket->hasPendingDatagrams()) {

        len = udpsocket->readDatagram(static_buffer_in, MAX_BUFFER_SIZE);

        qDebug() << "[UdpDataConnection::provideDataConsumed] read datagram, len=" << len;

        if(bufferReady) {
            emit overrunIdentified();
        }

        bufferReady = true;
        emit receivedDataPublished();
    }
}


bool UdpDataConnection::sendFirstDatagram()
{
    qDebug() << "[sendFirstDatagram] Send 1st packet";
    Broker::DataCollection dummyPacket;
    dummyPacket.set_provider_name("broker");
    dummyPacket.set_timestamp(QDateTime::currentDateTime().toMSecsSinceEpoch());
    return provideDataConsumed(&dummyPacket);
}
