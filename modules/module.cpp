#include "module.h"

#include <QDebug>
#include <QDateTime>

Module::Module(const QString& configPath, QObject *parent) : QObject(parent)
{
    configuration = new ModuleConfiguration();
    configuration->loadFromJsonFile(configPath);

    communication = new Communication(configuration);
}

Module::~Module()
{
    delete configuration;
    if(communication) {
        delete communication;
    }
}

const ModuleConfiguration *Module::getConfiguration()
{
    return configuration;
}

Communication *Module::getCommunication() const
{
    return communication;
}

