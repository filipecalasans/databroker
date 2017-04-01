#ifndef MESSAGEBROKER_H
#define MESSAGEBROKER_H

#include <QObject>
#include <QMap>
#include <QJsonArray>
#include <QTimer>

#include "modules/module.h"

class DataBroker : public QObject
{
    Q_OBJECT
public:
    explicit DataBroker(QObject *parent = 0);

    ~DataBroker();

    bool loadConfiguration();
    void loadModules(const QJsonArray& moduleArray);

    QList<Module*> getModules();    
    bool areAllMandatoryModulesInState(TcpControlConnection::ControlStateType state);

    void timerEvent(QTimerEvent *event);

    void disconnectModules();
    void connectModules();
    void readyModules();
    void startModules();
    void resetModules();
    void pauseModules();
    void resumeModules();

    void allIdleFeedback();
    void allReadyFeedback();
    void allPausedFeedback();
    void allRunningFeedback();

    void failAllIdleFeedback();
    void failAllReadyFeedback();
    void failAllPausedFeedback();
    void failAllRunningFeedback();

    void autostartPlay();

signals:


public slots:

    void routeCommand(Module *sourceModule, Broker::ControlCommand *command);
    void routeCommandReceived();

protected:

    void routeCommandUsingPacketContent(Module *sourceModule, Broker::ControlCommand *command);
    void routeCommandUsingMap(Module *sourceModule, Broker::ControlCommand *command);
    void routeCmdFromMaster(const QString &command);
    void forwardCommandToAllModules(Module *sourceModule, Broker::ControlCommand *command);
    void forwardCommandToDestinationsInPacket(Broker::ControlCommand *command);
    void forwardCommandUsingRouteMap(Module *sourceModule, Broker::ControlCommand *command);

    void buildDataPacketAndSend(Module *module);
    void appendDataList(Module *fromModule, QStringList *dataList, Broker::DataCollection *packet);

    void initTimeoutTimers();

private:

    bool allModulesRunning = false;
    bool autoStart = true;
    int dataRate = 0;

    QStringList masterModules;
    QMap <QString, Module*> modules;
    QString workingDirectoryPath;

    int connectTimeout = 2000, readyTimeout = 1000, startTimeout = 2000, retryTimeout = 5000, pauseTimeout = 100;
    QTimer connectTimer, readyTimer, startTimer, retryTimer, pauseTimer;
};

#endif // MessageBroker_H
