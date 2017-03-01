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

    /*
     * Prepare Socket connections, it assumes the
     * module information have already been loaded from JSON descriptor.
     *
     */
    void initControlConnection();
    void initDataConnection();

signals:

public slots:

private:

    ModuleConfiguration *configuration = nullptr;

    /* Add here Data and Control Connection */

};

#endif // MODULE_H
