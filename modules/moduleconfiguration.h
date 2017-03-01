#ifndef MODULE_CONFIGURATION_H
#define MODULE_CONFIGURATION_H

#include <QObject>
#include <QMap>
#include <QAbstractSocket>

#include "datadescriptor.h"

class ModuleConfiguration : public QObject
{
    Q_OBJECT
public:
    explicit ModuleConfiguration(QObject *parent = 0);

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

    QAbstractSocket::SocketType getDataSocketType() const;
    void setDataSocketType(const QAbstractSocket::SocketType &value);

    operator QString() const {

        QString descriptors;
        for(auto it = data_published.begin(); it!=data_published.end(); it++) {
            descriptors += (*it).operator QString();
            descriptors += "\n";
        }

        QString consumed;
        for(QString source : data_consumed.keys()) {
            consumed += QString("From %1: ").arg(source);
            for(QString dataId : *data_consumed.value(source)) {
                consumed +=  QString("%1, ").arg(dataId);
            }
            consumed += "\n";
        }

        QString out = QString("Module: %1\n"
                       "Description: %2\n"
                       "Is mandatory ? %3\n"
                       "Locally provided ? %4\n"
                       "IP: %5\n"
                       "PORT CONTROL: %6\n"
                       "PORT DATA: %7\n"
                       "Socket Type: %8\n"
                       "Data Published: %9\n"
                       "Data Consumed: %10")
                .arg(name)
                .arg(description)
                .arg(mandatory)
                .arg(locallyProvided)
                .arg(ip)
                .arg(portControl)
                .arg(portData)
                .arg((socketDataType == QAbstractSocket::TcpSocket) ? "TCP" : "UDP")
                .arg(descriptors)
                .arg(consumed);

        return out;
    }

    quint16 getPortData() const;
    void setPortData(const quint16 &value);

    quint16 getPortControl() const;
    void setPortControl(const quint16 &value);

    QString getId() const;
    void setId(const QString &value);

    bool loadFromJsonFile(const QString& jsonPath);

private:

    QString id;
    QString name;
    QString description;

    QMap<QString, DataDescriptor> data_published;
    QMap<QString, QStringList*> data_consumed;

    bool mandatory = true;
    bool locallyProvided = true;

    /* Connection Configuration */
    QString ip;
    quint16 portData = 0;
    quint16 portControl = 0;

    QAbstractSocket::SocketType socketDataType = QAbstractSocket::UdpSocket;

};

#endif // MODULE_H
