#include "module.h"


#include <QDebug>

Module::Module(const QString& configPath, QObject *parent) : QObject(parent)
{
    configuration = new ModuleConfiguration();
    configuration->loadFromJsonFile(configPath);

    initDataConnection();
}

Module::~Module()
{
    delete configuration;
    if(dataConnection) {
        delete dataConnection;
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
