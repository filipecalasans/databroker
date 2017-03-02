#include "module.h"

#include <QDebug>
#include <QDateTime>

Module::Module(const QString& configPath, QObject *parent) : QObject(parent)
{
    configuration = new ModuleConfiguration();
    configuration->loadFromJsonFile(configPath);

    initDataConnection();

    startTimer(10);
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
    if(dataConnection) {
        if(dataConnection->getIsReady()) {
            Broker::DataCollection packet;

            packet.set_provider_name("radio");

            Broker::Data *px0, *py0;

            quint64 timestamp = QDateTime::currentMSecsSinceEpoch();

            px0 = packet.add_data_provided();
            px0->set_data_name("vx0");
            px0->set_timestamp(timestamp);
            px0->set_data_type(Broker::DOUBLE);
            px0->set_data_double((qrand()%100)/ 1.7);

            py0 = packet.add_data_provided();
            py0->set_data_name("vy0");
            py0->set_timestamp(timestamp);
            py0->set_data_type(Broker::DOUBLE);
            py0->set_data_double((qrand()%100)/ 1.7);

            dataConnection->provideDataConsumed(&packet);

            qDebug() << "[CONSUMED] Size:" << packet.ByteSize();
            qDebug() << "[CONSUMED]" << QString::fromStdString(packet.DebugString());
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

            qDebug() << "[PUBLISHED]" << QString::fromStdString(data.DebugString());
        });
    }
    else {
        return;
    }

    if(!dataConnection->initConnection()) {
        qDebug() << "[Module::initDataConnection()] Data Connection initialized";
        return;
    }
}
