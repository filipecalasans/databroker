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
    QVariant getData(const QString& dataId) const;

    Communication *getCommunication() const;

    static QVariant fromProtoDataTypeToVariant(Broker::Data *data);
    static void fromVariantToProtoDataType(const QVariant &dataVariant, Broker::Data *data);

    bool sendControlCommand(Broker::ControlCommand *command);
    bool sendDataPacket(Broker::DataCollection *dataCollection);

    bool isMaster() const;

protected:

    void initLiveDataMapFromConfiguration();

signals:    

    void processCommandReceived();

public slots:

    void processDataPublishedReceived();

private:

    ModuleConfiguration *configuration = nullptr;
    Communication *communication = nullptr;

    QVariantMap currentData;

    quint64 lastTimeStamp = 0;

};

#endif // MODULE_H
