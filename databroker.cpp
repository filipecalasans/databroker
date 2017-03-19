#include "databroker.h"

#include <QDir>
#include <QSettings>
#include <QDebug>

DataBroker::DataBroker(QObject *parent) : QObject(parent)
{
    QString workingDirectoryPath = "/home/filipe/Documents/Workspace/portfolio/simulation_framework/broker/config";

    QDir workingDir(workingDirectoryPath);
    if(!QDir::setCurrent(workingDirectoryPath)) {
        qDebug() << workingDir << "does not exist.";
        return;
    }

    loadConfiguration();
    loadModules();
}

DataBroker::~DataBroker()
{
    qDeleteAll(modules.values());
    modules.clear();
}

void DataBroker::loadModules()
{
    QSettings settings("config.ini", QSettings::IniFormat);

    settings.beginGroup("modules");

    for(auto moduleId : settings.allKeys()) {
        QString modulePath = settings.value(moduleId).toString();
        Module *m = new Module(modulePath);
        modules.insert(m->getConfiguration()->getId(), m);
        connect(m, &Module::processCommandReceived, this, &DataBroker::routeCommandReceived);
        qDebug() << moduleId << "=" << modulePath;
    }
}

void DataBroker::loadConfiguration()
{
    QSettings settings("config.ini", QSettings::IniFormat);

    settings.beginGroup("configuration");

    dataRate = settings.value("data_rate", 0).toInt();
}

QList<Module *> DataBroker::getModules()
{
    return modules.values();
}

void DataBroker::routeCommand(Module *sourceModule, Broker::ControlCommand *command)
{
    Q_UNUSED(command)

    if(command->desitination_size()) {
        routeCommandUsingPacketContent(command);
    }
    else {
        routeCommandUsingMap(sourceModule, command);
    }
}

void DataBroker::routeCommandUsingMap(Module *sourceModule, Broker::ControlCommand *command)
{
    forwardCommandUsingRouteMap(sourceModule, command);
}

void DataBroker::routeCommandUsingPacketContent(
        Broker::ControlCommand *command)
{
    if(command->desitination_size() == 1) {
        QString destination = QString(command->desitination(0).c_str());
        if(destination == "all"){
            forwardCommandToAllModule(command);
            return;
        }
    }

    forwardCommandToDestinationsInPacket(command);
}

void DataBroker::forwardCommandToAllModule(
        Broker::ControlCommand *command)
{
    for(Module *module : modules) {
        if(!module->sendControlCommand(command)) {
            qDebug() << "[CANT ROUT COMMAND]" << module->getConfiguration()->getName();
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
        qDebug() << "===============================================================";
        if(modules.contains(destination)) {
            Module *module = modules.value(destination);
            if(!module->sendControlCommand(command)) {
                 qDebug() << "[CANT ROUT COMMAND]" << module->getConfiguration()->getName();
            }
        }
    }
}

void DataBroker::forwardCommandUsingRouteMap(Module *sourceModule,
        Broker::ControlCommand *command)
{
    QString commandName = QString(command->command().c_str());

    for(Module *destination : modules.values()) {
        if(destination != sourceModule) {
            QString moduleName = sourceModule->getConfiguration()->getName();
            const ModuleConfiguration *configuration = destination->getConfiguration();
            const QStringList *consumed = configuration->getCommandsConsumedByModule(moduleName);
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
