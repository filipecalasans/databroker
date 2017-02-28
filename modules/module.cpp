#include "module.h"

Module::Module(const QString& iniPath, QObject *parent) : QObject(parent)
{
    configuration = new ModuleConfiguration();
    configuration->loadFromJsonFile(iniPath);
}

Module::~Module()
{
    delete configuration;
    delete controlSocket;
}
