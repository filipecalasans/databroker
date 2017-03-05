#include "communication.h"

#include "../broker/protocol/data.pb.h"
#include "../broker/protocol/control.pb.h"

#include <QDateTime>

Communication::Communication(QObject *parent) : QObject(parent)
{
#ifdef TCP_CONNECTION

    dataConnection = new TcpDataConnection(6001);

    connect(dataConnection, &AbstractDataConnection::receivedDataConsumed, [this](){
        Broker::DataCollection packet;
        dataConnection->receiveDataConsumed(&packet);
        qDebug() << "[CONSUMED] Size:" << packet.ByteSize();
        qDebug() << "[CONSUMED]" << QString::fromStdString(packet.DebugString());
    });

    if(!dataConnection->initConnection(6001)) {
        qDebug() << "Can not init connection";
    }
    else {
        qDebug() << "Listening port 60001";
    }
#else

    dataConnection = new UdpDataConnection();

    connect(dataConnection, &AbstractDataConnection::receivedDataConsumed, [this](){
        Broker::DataCollection packet;
        dataConnection->receiveDataConsumed(&packet);
        qDebug() << "[CONSUMED] Size:" << packet.ByteSize();
        qDebug() << "[CONSUMED]" << QString::fromStdString(packet.DebugString());
        qDebug() << "============================================================================";
    });

    if(!dataConnection->initConnection(6001)) {
        qDebug() << "Can not init connection";
    }
    else {
        qDebug() << "Listening Data Server on port" << dataConnection->getPort();
    }
#endif

    controlConnection = new TcpControlConnection();

    connect(controlConnection, &TcpControlConnection::controlStateChanged,
            [this](TcpControlConnection::ControlStateType state) {

        qDebug() << QString("[STATE] %1").arg(state);
        controlConnection->notifyMyState();
    });

    connect(controlConnection, &AbstractControlConnection::receivedControlCommand, [this](){
        Broker::ControlCommand packet;
        controlConnection->receiveControlCommand(&packet);
        qDebug() << "[CONSUMED] Size:" << packet.ByteSize();
        qDebug() << "[CONSUMED]" << QString::fromStdString(packet.DebugString());
        qDebug() << "============================================================================";
    });

    if(!controlConnection->initConnection(6000)) {
        qDebug() << "Can not init connection";
    }
    else {
        qDebug() << "Listening Control Server on port" << controlConnection->getPort();
    }

}
