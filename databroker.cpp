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

    workingDirectoryPath = "/home/filipe/Documents/Workspace/portfolio/simulation_framework/build/config";

    QDir workingDir(workingDirectoryPath);
    if(!QDir::setCurrent(workingDirectoryPath)) {
        qDebug() << workingDir << "does not exist.";
        return;
    }

    if(!loadConfiguration()) {
        exit(1);
        return;
    }

    initTimeoutTimers();

    if(autoStart) {
        autostartPlay();
    }

    QObject::startTimer(dataRate);
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

bool DataBroker::areAllMandatoryModulesInState(TcpControlConnection::ControlStateType state)
{
    for(Module *module : modules) {
        if(module->getConfiguration()->getMandatory()){
            if(module->getCommunication()->getControlConnection()->getState() != state) {
                return false;
            }
        }
    }
    return true;
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
        if(allModulesRunning) {
            buildDataPacketAndSend(module);
        }
    }
}

void DataBroker::buildDataPacketAndSend(Module *module)
{
    const QMap<QString, QStringList *> *map = module->getConfiguration()->getDataConsumedRoutes();
    Broker::DataCollection dataPacket;

    for(QString fromModule : map->keys()) {
        QStringList *dataList = map->value(fromModule);
        Module *moduleDataSource = modules.value(fromModule, nullptr);
        if(dataList->size() && moduleDataSource) {
            appendDataList(moduleDataSource, dataList, &dataPacket);
        }
    }
    dataPacket.set_provider_name("");
    //dataPacket.set_provider_name(module->getConfiguration()->getId().toStdString());
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

void DataBroker::initTimeoutTimers()
{
    readyTimer.setInterval(readyTimeout);
    startTimer.setInterval(startTimeout);
    retryTimer.setInterval(retryTimeout);
    connectTimer.setInterval(connectTimeout);

    readyTimer.setSingleShot(true);
    startTimer.setSingleShot(true);
    retryTimer.setSingleShot(true);
    connectTimer.setSingleShot(true);

    connect(&connectTimer, &QTimer::timeout, [this](){
        if(areAllMandatoryModulesInState(TcpControlConnection::STATE_IDLE)) {
            if(autoStart) {
                readyModules();
            }
        }
        else {
            if(autoStart) {
                retryTimer.start();
            }
            else {
                /*TODO: SEND FEEDBACK TO MASTERS */
            }
        }
    });


    connect(&readyTimer, &QTimer::timeout, [this](){
        if(areAllMandatoryModulesInState(TcpControlConnection::STATE_READY)) {
            if(autoStart) {
                startModules();
            }
            else {
                /* TODO: SEND FEEDBACK TO MASTERS */
            }
        }
        else {
            resetModules();
            if(autoStart) {
                retryTimer.start();
            }
            else {
                /*TODO: SEND FEEDBACK TO MASTERS */
            }
        }
    });

    connect(&startTimer, &QTimer::timeout, [this](){
        allModulesRunning = areAllMandatoryModulesInState(TcpControlConnection::STATE_RUNNING);
        if(!allModulesRunning) {
            if(autoStart) {
                retryTimer.start();
            }
            else {
                /* TODO: Send feedback to masters */
            }
        }
    });

    connect(&retryTimer, &QTimer::timeout, [this](){
        if(autoStart) {
            qDebug() << "RETRY AUTOSTART";
            resetModules();
        }
        else {
            /*TODO: SEND FEEDBACK TO MASTERS */
        }
    });
}

void DataBroker::disconnectModules()
{
    for(Module *m : getModules()) {
        m->getCommunication()->getControlConnection()->deInitConnection();
    }

    allModulesRunning = false;
}

void DataBroker::connectModules()
{
    for(Module *m : getModules()) {
        m->getCommunication()->getControlConnection()->initConnection();
    }

    connectTimer.start();
}

void DataBroker::readyModules()
{
    for(Module *m : getModules()) {
        m->getCommunication()->getControlConnection()->sendDefaultReadyCommand();
    }

    readyTimer.start();
}

void DataBroker::startModules()
{
    for(Module *m : getModules()) {
        m->getCommunication()->getControlConnection()->sendDefaultStartCommand();
    }

    startTimer.start();
}

void DataBroker::resetModules()
{
    for(Module *m : getModules()) {
        m->getCommunication()->getControlConnection()->sendDefaultResetCommand();
    }
}

void DataBroker::autostartPlay()
{
    connectModules();
}

void DataBroker::pauseModules()
{
    for(Module *m : getModules()) {
        m->getCommunication()->getControlConnection()->sendDefaultPauseCommand();
    }

    allModulesRunning = false;
}

void DataBroker::resumeModules()
{
    for(Module *m : getModules()) {
        m->getCommunication()->getControlConnection()->sendDefaultResumeCommand();
    }

    startTimer.start();
}



