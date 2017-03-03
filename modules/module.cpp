#include "module.h"

#include <QDebug>
#include <QDateTime>

Module::Module(const QString& configPath, QObject *parent) : QObject(parent)
{
    configuration = new ModuleConfiguration();
    configuration->loadFromJsonFile(configPath);

    initDataConnection();

    startTimer(500);
}

Module::~Module()
{
    delete configuration;
    if(dataConnection) {
        delete dataConnection;
    }
}

void Module::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    if(dataConnection) {
        if(dataConnection->getIsReady()) {
            Broker::DataCollection packet;

            packet.set_provider_name("radio");
            packet.set_timestamp(QDateTime::currentMSecsSinceEpoch());

            Broker::Data *px0, *py0;

//            px0 = packet.add_data_provided();
//            px0->set_data_name("vx0");
//            px0->set_data_type(Broker::DOUBLE);
//            px0->set_data_double((qrand()%100)/ 1.7);

            for(int i=0; i<100; i++) {
                py0 = packet.add_data_provided();
                py0->set_data_name(QString("vy%1").arg(i).toStdString());
                py0->set_data_type(Broker::DOUBLE);
                py0->set_data_double((qrand()%100)/ 1.7);
            }
            dataConnection->provideDataConsumed(&packet);

            qDebug() << "[CONSUMED] Size:" << packet.ByteSize();
            qDebug() << "[CONSUMED]" << QString::fromStdString(packet.DebugString());
            qDebug() << "[CONSUMED]" << "=================================================================";
        }
    }
}

void Module::initDataConnection()
{
    if(configuration->getDataSocketType() == QAbstractSocket::TcpSocket) {
        dataConnection = new TcpDataConnection(configuration->getIp(), configuration->getPortData());

        connect(dataConnection, &TcpDataConnection::receivedDataPublished, [this](){
            Broker::DataCollection data;
            dataConnection->receiveDataPublished(&data);

            qDebug() << "[PUBLISHED TCP]" << QString::fromStdString(data.DebugString());
        });
    }
    else {
        dataConnection = new UdpDataConnection(configuration->getIp(), configuration->getPortData());
        connect(dataConnection, &UdpDataConnection::receivedDataPublished, [this](){
            Broker::DataCollection data;
            dataConnection->receiveDataPublished(&data);

            qDebug() << "[PUBLISHED UDP]" << QString::fromStdString(data.DebugString());
        });
    }

    if(!dataConnection->initConnection()) {
        qDebug() << "[Module::initDataConnection()] Data Connection not initialized";
    }
}
