#include "databroker.h"

#include <QDir>
#include <QSettings>
#include <QDebug>
#include <QDateTime>
#include <QJsonValueRef>
#include <QJsonArray>

#include <QJsonDocument>
#include <QJsonObject>

DataBroker::DataBroker(QObject *parent) : QObject(parent)
{
    /* TODO: Change this */

    //QString workingDirectoryPath = QDir::current().absolutePath() + "/config";


    if(!loadConfiguration()) {
        exit(1);
        return;
    }

    workingDirectoryPath = "/home/filipe/Documents/Workspace/portfolio/simulation_framework/build/config";

    QDir workingDir(workingDirectoryPath);
    if(!QDir::setCurrent(workingDirectoryPath)) {
        qDebug() << workingDir << "does not exist.";
        return;
    }

    startTimer(dataRate);
}

DataBroker::~DataBroker()
{
    qDeleteAll(modules.values());
    modules.clear();
}

void DataBroker::loadModules(const QJsonArray& moduleArray)
{
    for(QJsonValue ref : moduleArray) {
        if(ref.isString()) {
            QString modulePath = workingDirectoryPath + "/" + ref.toString();
            Module *m = new Module(modulePath);
            modules.insert(m->getConfiguration()->getId(), m);
            connect(m, &Module::processCommandReceived, this, &DataBroker::routeCommandReceived);
        }
    }
}

bool DataBroker::loadConfiguration()
{
    QJsonParseError error;

    QFile file("config.json");
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
        return false;
    }

    QJsonDocument configuration = QJsonDocument::fromJson(file.readAll(), &error);

    if(!configuration.isObject()) {
        qDebug() << error.errorString();
        return false;
    }

    QJsonObject configurationObj = configuration.object();

    dataRate = configurationObj.value("data_rate").toInt(0);
    autoStart = configurationObj.value("autoStart").toBool(true);

    QJsonArray masterModules = configurationObj.value("master_modules").toArray();
    for(QJsonValue master : masterModules) {
        if(master.isString()) {
            QString masterId = master.toString();
            if(!masterId.isEmpty()) {
                masterModules << master.toString();
            }
        }
    }

    QJsonArray modulesJsonArray = configurationObj.value("modules").toArray();
    loadModules(modulesJsonArray);

    return true;
}

QList<Module *> DataBroker::getModules()
{
    return modules.values();
}

void DataBroker::routeCommand(Module *sourceModule, Broker::ControlCommand *command)
{
    Q_UNUSED(command)

    if(command->desitination_size()) {
        routeCommandUsingPacketContent(sourceModule, command);
    }
    else {
        routeCommandUsingMap(sourceModule, command);
    }
}

void DataBroker::routeCommandUsingMap(Module *sourceModule, Broker::ControlCommand *command)
{
    qDebug() << "[ROUTING CMD USING MAP]";
    forwardCommandUsingRouteMap(sourceModule, command);
}

void DataBroker::routeCommandUsingPacketContent(
        Module *sourceModule, Broker::ControlCommand *command)
{
    if(command->desitination_size() == 1) {
        QString destination = QString(command->desitination(0).c_str());
        if(destination == "all"){
            forwardCommandToAllModule(sourceModule, command);
            return;
        }
    }

    forwardCommandToDestinationsInPacket(command);
}

void DataBroker::forwardCommandToAllModule(
        Module *sourceModule, Broker::ControlCommand *command)
{
    for(Module *module : modules) {
        if(sourceModule != module) {
            qDebug() << "[TRY ROUT COMMAND] dest:" << module->getConfiguration()->getId();
            if(!module->sendControlCommand(command)) {
                qDebug() << "[CANT ROUT COMMAND] dest:" << module->getConfiguration()->getId();
            }
            qDebug() << "==================================";
        }
    }
}

void DataBroker::forwardCommandToDestinationsInPacket(
        Broker::ControlCommand *command)
{
    /* Routing command using to the destinations listed in the packet */
    for(int i=0; i<command->desitination_size(); i++) {
        QString destination = QString(command->desitination(i).c_str());
        qDebug() << "[ROUTING CMD] Dest:" << destination;
        qDebug() << "[ROUTING CMD] Cmd:" << QString(command->DebugString().c_str());
        if(modules.contains(destination)) {
            Module *module = modules.value(destination);
            if(!module->sendControlCommand(command)) {
                 qDebug() << "[CANT SEND COMMAND] From: " << module->getConfiguration()->getId()
                          << "To: " << destination;
            }
        }
        else {
            qDebug() << "[ROUTING CMD] Can`t find the module:" << destination;
        }
        qDebug() << "===============================================================";
    }
}

void DataBroker::forwardCommandUsingRouteMap(Module *sourceModule,
        Broker::ControlCommand *command)
{
    QString commandName = QString(command->command().c_str());

    for(Module *destination : modules.values()) {
        if(destination != sourceModule) {
            QString moduleId = sourceModule->getConfiguration()->getId();
            const ModuleConfiguration *configuration = destination->getConfiguration();
            const QStringList *consumed = configuration->getCommandsConsumedByModule(moduleId);
            if(consumed) {
                if(consumed->contains(commandName)) {
                    destination->sendControlCommand(command);
                }
            }
        }
    }
}

void DataBroker::routeCommandReceived()
{
    Module *module = qobject_cast<Module*>(sender());

    qDebug() << "[DataBroker::routeCommandReceived()]";

    if(module) {
        Broker::ControlCommand command;
        module->getCommunication()->getControlConnection()->receiveControlCommand(&command);
        routeCommand(module, &command);
    }
}

void DataBroker::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    for(Module *module : modules) {
        //buildDataPacketAndSend(module);
    }
}

void DataBroker::buildDataPacketAndSend(Module *module)
{
    const QMap<QString, QStringList *> *map = module->getConfiguration()->getDataConsumedRoutes();
    Broker::DataCollection dataPacket;

    for(QString fromModule : map->keys()) {
        QStringList *dataList = map->value(fromModule);
        Module *module = modules.value(fromModule, nullptr);
        if(dataList->size() && module) {
            appendDataList(module, dataList, &dataPacket);
        }
    }
    dataPacket.set_provider_name(module->getConfiguration()->getId().toStdString());
    dataPacket.set_timestamp(QDateTime::currentMSecsSinceEpoch());
    module->sendDataPacket(&dataPacket);
}

void DataBroker::appendDataList(Module *fromModule, QStringList *dataList,
                                Broker::DataCollection *packet)
{
    for(QString dataId : *dataList) {
        QVariant dataVariant = fromModule->getData(dataId);
        if(dataVariant.isValid()) {
            Broker::Data *data = packet->add_data_provided();
            data->set_data_id(dataId.toStdString());
            Module::fromVariantToProtoDataType(dataVariant, data);
        }
    }
}


