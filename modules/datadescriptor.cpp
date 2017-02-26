#include "datadescriptor.h"

DataDescriptor::DataDescriptor(QObject *parent) : QObject(parent)
{

}

DataDescriptor::DataDescriptor(QString name, QString desc, QString unit, QObject *parent) :
    QObject(parent),
    dataName(name),
    description(desc),
    unit(unit)
{

}

QString DataDescriptor::getDataName() const
{
    return dataName;
}

void DataDescriptor::setDataName(const QString &value)
{
    dataName = value;
}

QString DataDescriptor::getDescription() const
{
    return description;
}

void DataDescriptor::setDescription(const QString &value)
{
    description = value;
}

QString DataDescriptor::getUnit() const
{
    return unit;
}

void DataDescriptor::setUnit(const QString &value)
{
    unit = value;
}




