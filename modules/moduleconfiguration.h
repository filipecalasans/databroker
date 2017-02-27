#ifndef MODULE_CONFIGURATION_H
#define MODULE_CONFIGURATION_H

#include <QObject>
#include <QMap>

#include "datadescriptor.h"

class ModuleConfiguration : public QObject
{
    Q_OBJECT
public:
    explicit ModuleConfiguration(QObject *parent = 0);

    enum DataSocketType {
        TCP_SOCKET,
        UDP_SOCKET,
    };

    QString getName() const;
    void setName(const QString &value);

    QString getDescription() const;
    void setDescription(const QString &value);

    bool getMandatory() const;
    void setMandatory(bool value);

    bool getLocallyProvided() const;
    void setLocallyProvided(bool value);

    QString getIp() const;
    void setIp(const QString &value);

    int getDataSocketType() const;
    void setDataSocketType(const DataSocketType &value);

    operator QString() const {
        return QString("Module: %1\n"
                       "Description: %2\n"
                       "Is mandatory ? %3\n"
                       "Locally provided ? %4\n"
                       "IP: %5\n"
                       "PORT CONTROL: %6\n"
                       "PORT DATA: %7\n"
                       "Socket Type: %8")
                .arg(name)
                .arg(description)
                .arg(mandatory)
                .arg(locallyProvided)
                .arg(ip)
                .arg(portControl)
                .arg(portData)
                .arg(socketDataType == UDP_SOCKET ? "UDP" : "TCP");
    }

    quint16 getPortData() const;
    void setPortData(const quint16 &value);

    quint16 getPortControl() const;
    void setPortControl(const quint16 &value);

    QString getId() const;
    void setId(const QString &value);

    void loadFromJsonFile(const QString &moduleId, QString jsonPath);

private:

    QString id;
    QString name;
    QString description;
    QMap<QString, DataDescriptor> expected_data, data_registered;

    bool mandatory = true;
    bool locallyProvided = true;

    /* Connection Data */
    QString ip;
    quint16 portData = 0;
    quint16 portControl = 0;

    DataSocketType socketDataType = UDP_SOCKET;

};

#endif // MODULE_H
