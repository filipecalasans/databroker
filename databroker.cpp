#include "databroker.h"

#include <QDir>
#include <QSettings>
#include <QDebug>

DataBroker::DataBroker(QObject *parent) : QObject(parent)
{
    loadModules();
}

DataBroker::~DataBroker()
{
    qDeleteAll(modules.values());
    modules.clear();
}

void DataBroker::loadModules()
{
    /* TODO: change this */

    QString workingDirectoryPath = "/home/filipe/Documents/Workspace/portfolio/simulation_framework/broker/config";

    QDir workingDir(workingDirectoryPath);
    if(!QDir::setCurrent(workingDirectoryPath)) {
        qDebug() << workingDir << "does not exist.";
        return;
    }

    QSettings settings("config.ini", QSettings::IniFormat);

    settings.beginGroup("modules");

    for(auto moduleId : settings.allKeys()) {
        QString modulePath = settings.value(moduleId).toString();
        Module *m = new Module(modulePath);
        modules.insert(m->getConfiguration()->getId(), m);
        qDebug() << moduleId << "=" << modulePath;
    }
}
