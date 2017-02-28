#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>

#include "moduleconfiguration.h"

class Module : public QObject
{
    Q_OBJECT
public:
    explicit Module(const QString& configPath, QObject *parent = 0);

    ~Module();

protected:




signals:

public slots:

private:

    ModuleConfiguration *configuration = nullptr;

    QTcpSocket *controlSocket = nullptr;
    QTcpSocket *dataTcpSocket = nullptr;
    QUdpSocket *dataUdpSocket = nullptr;

};

#endif // MODULE_H
