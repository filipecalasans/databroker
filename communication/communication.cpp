#include "communication.h"

#include <QDateTime>

#include "communication/tcpdataconnection.h"
#include "communication/udpdataconnection.h"

Communication::Communication(const ModuleConfiguration *config, QObject *parent) : QObject(parent),
    configuration(config)
{
    initDataConnection(configuration);
    initControlConnection(configuration);
    //startTimer(500);
}

Communication::~Communication()
{
    if(dataConnection) {
        delete dataConnection;
    }
}

void Communication::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    if(dataConnection) {
        if(dataConnection->getIsReady()) {
            Broker::DataCollection packet;

            packet.set_provider_name("radio");
            packet.set_timestamp(QDateTime::currentMSecsSinceEpoch());

            Broker::Data *py0;

            for(int i=0; i<3; i++) {
                py0 = packet.add_data_provided();
                py0->set_data_id(QString("vy%1").arg(i).toStdString());
                py0->set_data_type(Broker::DATA_DOUBLE);
                py0->set_data_double((qrand()%100)/ 1.7);
            }
            dataConnection->provideDataConsumed(&packet);

            qDebug() << "[CONSUMED] Size:" << packet.ByteSize();
            qDebug() << "[CONSUMED]" << QString::fromStdString(packet.DebugString());
            qDebug() << "[CONSUMED]" << "=================================================================";
        }
    }
}

void Communication::initDataConnection(const ModuleConfiguration *configuration)
{
    if(configuration->getDataSocketType() == QAbstractSocket::TcpSocket) {
        dataConnection = new TcpDataConnection(configuration->getIp(), configuration->getPortData());
    }
    else {
        dataConnection = new UdpDataConnection(configuration->getIp(), configuration->getPortData());
    }

    connect(dataConnection, &AbstractDataConnection::receivedDataPublished,
            this, &Communication::receivedDataPublished);

    if(!dataConnection->initConnection()) {
        qDebug() << "[Module::initDataConnection()] Data Connection not initialized";
    }
}

void Communication::initControlConnection(const ModuleConfiguration *configuration)
{
    controlConnection = new TcpControlConnection(
                            configuration->getIp(),
                            configuration->getPortControl());
}


