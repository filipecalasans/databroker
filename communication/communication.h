#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QObject>
#include "../protocol/data.pb.h"

#include "abstractdataconnection.h"
#include "tcpcontrolconnection.h"

#include "modules/moduleconfiguration.h"

class Communication : public QObject
{
    Q_OBJECT
public:

    explicit Communication(const ModuleConfiguration *config, QObject *parent = 0);
    ~Communication();

    void timerEvent(QTimerEvent *event);

    void sendDataConsumed(const QVariantMap &data);

    TcpControlConnection *getControlConnection() const;

    AbstractDataConnection *getDataConnection() const;

signals:

    void receivedDataPublished();
    void receivedCommand();

public slots:  

protected:

    void initDataConnection(const ModuleConfiguration *configuration);
    void initControlConnection(const ModuleConfiguration *configuration);

private:

    /* Add here Data and Control Connection */
    AbstractDataConnection *dataConnection = nullptr;
    TcpControlConnection *controlConnection = nullptr;

    const ModuleConfiguration *configuration = nullptr;

};

#endif // COMMUNICATION_H
