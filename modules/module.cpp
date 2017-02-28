#include "module.h"

Module::Module(const QString& configPath, QObject *parent) : QObject(parent)
{
    configuration = new ModuleConfiguration();
    configuration->loadFromJsonFile(configPath);
}

Module::~Module()
{
    delete configuration;
    delete controlSocket;
    if(dataUdpSocket) { delete dataUdpSocket; }
    if(dataTcpSocket) { delete dataTcpSocket; }
}
