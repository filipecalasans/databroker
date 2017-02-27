#include "module.h"

Module::Module(const QString& moduleId, const QString& iniPath, QObject *parent) : QObject(parent)
{
    configuration = new ModuleConfiguration();

    configuration->loadFromJsonFile(moduleId, iniPath);
}

Module::~Module()
{
    delete configuration;
    delete controlSocket;
}
