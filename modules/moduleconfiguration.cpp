#include "moduleconfiguration.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDialog>

#include "datadescriptor.h"

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

QAbstractSocket::SocketType ModuleConfiguration::getDataSocketType() const
{
    return socketDataType;
}

void ModuleConfiguration::setDataSocketType(const QAbstractSocket::SocketType &value)
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

bool ModuleConfiguration::loadFromJsonFile(const QString& jsonPath)
{
    QJsonParseError error;

    QFile file(jsonPath);
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
        return false;
    }

    QJsonDocument module = QJsonDocument::fromJson(file.readAll(), &error);

    if(!module.isObject()) {
        qDebug() << error.errorString();
        return false;
    }

    QJsonObject moduleObj = module.object();
    id = moduleObj.value("id").toString();
    name = moduleObj.value("name").toString();
    description = moduleObj.value("description").toString();
    ip = moduleObj.value("ip").toString();
    portData = moduleObj.value("port_data").toInt();
    portControl = moduleObj.value("port_control").toInt();
    socketDataType = moduleObj.value("socket_type").toString() == "tcp" ? QAbstractSocket::TcpSocket : QAbstractSocket::UdpSocket;
    mandatory - moduleObj.value("mandatory").toBool(true);

    QJsonArray dataPublished = moduleObj.value("data_published").toArray();
    if(dataPublished.size()) {
        for(QJsonValue ref : dataPublished) {
            locallyProvided =true;
            if(ref.isObject()) {
                QJsonObject data = ref.toObject();
                QString dataId = data.value("id").toString();
                QString dataName = data.value("name").toString();
                QString unit = data.value("unit").toString();
                QString description = data.value("description").toString();
                DataDescriptor descriptor(dataId, dataName, description, unit);
                data_published.insert(dataId, descriptor);
            }
        }
    }

    QJsonArray dataConsumed = moduleObj.value("data_consumed").toArray();
    if(dataConsumed.size() > 0) {
        for(QJsonValue ref : dataConsumed) {
            locallyProvided =true;
            if(ref.isObject()) {
                QJsonObject routeObj = ref.toObject();
                QString source = routeObj.value("source").toString();
                QString dataid = routeObj.value("id").toString();
                if(!data_consumed.contains(source)) {
                    data_consumed.insert(source, new QStringList());
                }
                if(!((QStringList*)data_consumed.value(source))->contains(dataid)) {
                    ((QStringList*)data_consumed.value(source))->append(dataid);
                }
            }
        }
    }

    QJsonArray commandConsummed = moduleObj.value("commands_consumed").toArray();
    if(commandConsummed.size() > 0) {
        for(QJsonValue ref : commandConsummed) {
            if(ref.isObject()) {
                QJsonObject routeObj = ref.toObject();
                QString source = routeObj.value("source").toString();
                QString commandName = routeObj.value("command").toString();
                if(!commands_consumed.contains(source)) {
                    commands_consumed.insert(source, new QStringList());
                }
                if(!((QStringList*)commands_consumed.value(source))->contains(commandName)) {
                    ((QStringList*)commands_consumed.value(source))->append(commandName);
                }
            }
        }
    }

    return true;
}

const QMap<QString, DataDescriptor> *ModuleConfiguration::getDataPublishedMap() const
{
    return &data_published;
}

const QMap<QString, QStringList *> *ModuleConfiguration::getDataConsumedRoutes() const
{
    return &data_consumed;
}

const QMap<QString, QStringList *> *ModuleConfiguration::getCommandsConsumedRoutes() const
{
    return &commands_consumed;
}

const QStringList *ModuleConfiguration::getCommandsConsumedByModule(const QString &moduleId) const
{
    return commands_consumed.value(moduleId, nullptr);
}

const QStringList *ModuleConfiguration::getDataConsumedByModule(const QString &moduleId) const
{
    return data_consumed.value(moduleId, nullptr);
}

QString ModuleConfiguration::getId() const
{
    return id;
}

void ModuleConfiguration::setId(const QString &value)
{
    id = value;
}

