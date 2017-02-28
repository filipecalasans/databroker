#include "datadescriptor.h"

DataDescriptor::DataDescriptor(QObject *parent) : QObject(parent)
{

}

DataDescriptor::DataDescriptor(QString id, QString name, QString desc, QString unit, QObject *parent) :
    QObject(parent),
    dataId(id),
    dataName(name),
    description(desc),
    unit(unit)
{

}

DataDescriptor::DataDescriptor(const DataDescriptor &other, QObject *parent) : QObject(parent)
{
    (*this) = other;
}

DataDescriptor::DataDescriptor(DataDescriptor &other, QObject *parent) : QObject(parent)
{
    (*this) = other;
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

QString DataDescriptor::getKey() const
{
    return dataId;
}

QString DataDescriptor::getDataId() const
{
    return dataId;
}

void DataDescriptor::setDataId(const QString &value)
{
    dataId = value;
}




