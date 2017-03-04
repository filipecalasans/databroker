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
        qDebug() << "Listening port" << dataConnection->getPort();
    }
#endif

    controlConnection = new TcpControlConnection();

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
        qDebug() << "Listening port" << dataConnection->getPort();
    }

    //startTimer(500);
}

void Communication::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    if(dataConnection->getIsReady()){

        Broker::DataCollection packet;

        packet.set_provider_name("vision");
        packet.set_timestamp(QDateTime::currentMSecsSinceEpoch());

        Broker::Data *px0, *py0;

        px0 = packet.add_data_provided();
        px0->set_data_id("px0");
        px0->set_data_type(Broker::DATA_DOUBLE);
        px0->set_data_double((qrand()%100)/ 1.7);

        py0 = packet.add_data_provided();
        py0->set_data_id("py0");
        py0->set_data_type(Broker::DATA_DOUBLE);
        py0->set_data_double((qrand()%100)/ 1.7);

        dataConnection->provideDataPublished(&packet);

        qDebug() << "[PUBLISHED] Size:" << packet.ByteSize();
        qDebug() << "[PUBLISHED]" << QString::fromStdString(packet.DebugString());
    }
}

