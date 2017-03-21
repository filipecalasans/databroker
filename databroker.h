#ifndef MESSAGEBROKER_H
#define MESSAGEBROKER_H

#include <QObject>
#include <QMap>
#include <QJsonArray>

#include "modules/module.h"

class DataBroker : public QObject
{
    Q_OBJECT
public:
    explicit DataBroker(QObject *parent = 0);

    ~DataBroker();

    void loadModules(const QJsonArray& moduleArray);
    bool loadConfiguration();

    QList<Module*> getModules();    
    void timerEvent(QTimerEvent *event);

signals:

public slots:

    void routeCommand(Module *sourceModule, Broker::ControlCommand *command);
    void routeCommandReceived();

protected:

    void routeCommandUsingPacketContent(Module *sourceModule, Broker::ControlCommand *command);
    void routeCommandUsingMap(Module *sourceModule, Broker::ControlCommand *command);
    void forwardCommandToAllModule(Module *sourceModule, Broker::ControlCommand *command);
    void forwardCommandToDestinationsInPacket(Broker::ControlCommand *command);
    void forwardCommandUsingRouteMap(Module *sourceModule, Broker::ControlCommand *command);

    void buildDataPacketAndSend(Module *module);
    void appendDataList(Module *fromModule, QStringList *dataList, Broker::DataCollection *packet);

private:

    bool autoStart = true;
    int dataRate = 0;

    QStringList masterModules;
    QMap <QString, Module*> modules;
    QString workingDirectoryPath;

};

#endif // MessageBroker_H
