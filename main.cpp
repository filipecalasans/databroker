#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QDir>

#include "modules/moduleconfiguration.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    /* This section load modules from .ini configuration files */

    QString workingDirectoryPath = "/home/filipe/Documents/Workspace/portfolio/"
                               "simulation_framework/broker/config";

    QDir workingDir(workingDirectoryPath);
    if(!QDir::setCurrent(workingDirectoryPath)) {
        qDebug() << workingDir << "does not exist.";
        QApplication::exit(0);
    }

    QSettings settings("config.ini", QSettings::IniFormat);

    settings.beginGroup("modules");

    for(auto moduleId : settings.allKeys()) {
        QString modulePath = settings.value(moduleId).toString();
        qDebug() << moduleId << "=" << modulePath;
        ModuleConfiguration config;
        config.loadFromJsonFile(moduleId, modulePath);
        qDebug() << config;
    }



    return a.exec();
}
