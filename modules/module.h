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

    Communication *getCommunication() const;

    static QVariant fromProtoDataTypeToVariant(Broker::Data *data);
    static void fromVariantToProtoDataType(const QVariant &dataVariant, Broker::Data *data);

protected:

    void initLiveDataMapFromConfiguration();

signals:    

public slots:

    void processDataPublishedReceived();

private:

    ModuleConfiguration *configuration = nullptr;
    Communication *communication = nullptr;

    QVariantMap currentData;

    quint64 lastTimeStamp = 0;
};

#endif // MODULE_H
