#include "udpdataconnection.h"
#include "../protocol/data.pb.h"

#include <QUdpSocket>


UdpDataConnection::UdpDataConnection(QObject *parent) : AbstractDataConnection(parent)
{
    socket = new QUdpSocket();
    connect(socket, &QUdpSocket::readyRead, this, &UdpDataConnection::handleDatagram);
}

UdpDataConnection::UdpDataConnection(const QString &ipAddress,
                                     quint16 portNum, QObject *parent) :
    AbstractDataConnection(parent)
{
    setIp(ipAddress);
    setPort(portNum);
}

UdpDataConnection::~UdpDataConnection()
{
    delete socket;
}

void UdpDataConnection::deInitConnection()
{
    socket->abort();
}

bool UdpDataConnection::initConnection(quint16 portNum)
{
    setPort(portNum);
    socket->bind(portNum);
    return true;
}

bool UdpDataConnection::provideDataPublished(Broker::DataCollection *dataCollection)
{

    int bytesLen = dataCollection->ByteSize();

    Broker::DataCollection header;
    header.set_provider_name(dataCollection->provider_name());
    header.set_timestamp(dataCollection->timestamp());

    int payloadLen = bytesLen - header.ByteSize();

    int index = 0;

    Broker::DataCollection packet;

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
//              qDebug() << "[UdpDataConnection::provideDataConsumed] index"
//                       << index << packet.ByteSize() << dataElement.ByteSize();
            }
            else {
//              qDebug() << "======= BREAK ========";
                break;
            }
        }

        packet.SerializeToArray(static_buffer_out, MAX_DATAGRAM_SIZE);
        ((QUdpSocket*)socket)->writeDatagram(static_buffer_out, packet.ByteSize(), QHostAddress(brokerIp), brokerPort);
        payloadLen -= (packet.ByteSize() - headerLen);
    }

    return true;
}

bool UdpDataConnection::receiveDataConsumed(Broker::DataCollection *dataCollection)
{
    bool ret = dataCollection->ParseFromArray(static_buffer_in, len);
    bufferReady = false;
    return ret;
}

void UdpDataConnection::initConnection(const QString &ipAddress, quint16 portNum)
{
    initConnection(portNum);
    setIp(ipAddress);
}

void UdpDataConnection::setIp(const QString &value)
{
    AbstractDataConnection::setIp(value);
    setIsReady(true);
}

void UdpDataConnection::handleDatagram()
{
    QUdpSocket *udpsocket = (QUdpSocket*)socket;

    QHostAddress brokerAddress;

    while(udpsocket->hasPendingDatagrams()) {

        /*
         * TODO: brokerIp and brokerPort must be passed elsewhere.
         * Pass in the controlConnection.
         */
        len = udpsocket->readDatagram(static_buffer_in, MAX_BUFFER_SIZE, &brokerAddress, &brokerPort);

        bool ignoreDatagram = false;
        if(!getIsReady()) {
            ignoreDatagram = true;
        }

        setBrokerIp(brokerAddress.toString());
        setIsReady(true);

        qDebug() << "[HANDLE DATAGRAM] ignore?" << ignoreDatagram;

        if(len == 0) { continue; }

        if(ignoreDatagram) { // Ignores the first datagram !!!
            qDebug() << "[HANDLE DATAGRAM] Received Empty Datagram";
            continue;
        }

        if(bufferReady) {
            emit overrunIdentified();
        }

        bufferReady = true;
        emit receivedDataConsumed();
    }
}

QString UdpDataConnection::getBrokerIp() const
{
    return brokerIp;
}

void UdpDataConnection::setBrokerIp(const QString &value)
{
    brokerIp = value;
}

quint16 UdpDataConnection::getBrokerPort() const
{
    return brokerPort;
}

void UdpDataConnection::setBrokerPort(const quint16 &value)
{
    brokerPort = value;
}
