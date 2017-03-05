#ifndef MESSAGEBROKER_H
#define MESSAGEBROKER_H

#include <QObject>
#include <QMap>

#include "modules/module.h"

class DataBroker : public QObject
{
    Q_OBJECT
public:
    explicit DataBroker(QObject *parent = 0);

    ~DataBroker();

    void loadModules();
    void loadConfiguration();

signals:

public slots:

private:

    int dataRate = 0;
    QMap <QString, Module*> modules;

};

#endif // MessageBroker_H
