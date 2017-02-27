#include "module.h"

ModuleConfiguration::ModuleConfiguration(QObject *parent) : QObject(parent)
{

}

QString ModuleConfiguration::getName() const
{
    return name;
}

void ModuleConfiguration::setName(const QString &value)
{
    name = value;
}

QString ModuleConfiguration::getDescription() const
{
    return description;
}

void ModuleConfiguration::setDescription(const QString &value)
{
    description = value;
}

bool ModuleConfiguration::getMandatory() const
{
    return mandatory;
}

void ModuleConfiguration::setMandatory(bool value)
{
    mandatory = value;
}


bool ModuleConfiguration::getLocallyProvided() const
{
    return locallyProvided;
}

void ModuleConfiguration::setLocallyProvided(bool value)
{
    locallyProvided = value;
}

QString ModuleConfiguration::getIp() const
{
    return ip;
}

void ModuleConfiguration::setIp(const QString &value)
{
    ip = value;
}

int ModuleConfiguration::getDataSocketType() const
{
    return socketDataType;
}

void ModuleConfiguration::setDataSocketType(const DataSocketType &value)
{
    socketDataType = value;
}

quint16 ModuleConfiguration::getPortData() const
{
    return portData;
}

void ModuleConfiguration::setPortData(const quint16 &value)
{
    portData = value;
}

quint16 ModuleConfiguration::getPortControl() const
{
    return portControl;
}

void ModuleConfiguration::setPortControl(const quint16 &value)
{
    portControl = value;
}
