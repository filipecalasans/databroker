#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>

#include "moduleconfiguration.h"
#include "communication/communication.h"

class Module : public QObject
{
    Q_OBJECT
public:
    explicit Module(const QString& configPath, QObject *parent = 0);

    ~Module();

    const ModuleConfiguration *getConfiguration();

protected:

signals:

public slots:

private:

    ModuleConfiguration *configuration = nullptr;
    Communication *communication = nullptr;

};

#endif // MODULE_H
