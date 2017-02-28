#ifndef DATADESCRIPTOR_H
#define DATADESCRIPTOR_H

#include <QObject>

class DataDescriptor : public QObject
{
    Q_OBJECT
public:
    explicit DataDescriptor(QObject *parent = 0);
    explicit DataDescriptor(QString id, QString name, QString desc, QString unit, QObject *parent = 0);
    explicit DataDescriptor(const DataDescriptor& other, QObject *parent = 0);
    explicit DataDescriptor(DataDescriptor& other, QObject *parent = 0);

    QString getDataName() const;
    void setDataName(const QString &value);

    QString getDescription() const;
    void setDescription(const QString &value);

    QString getUnit() const;
    void setUnit(const QString &value);

    operator QString() const {
        return QString("DATA: %1, Unit: %2\nDescription %3")
                .arg(dataName)
                .arg(unit)
                .arg(description);
    }

    DataDescriptor& operator =(const DataDescriptor& other) {
        if(this != &other) {
            this->dataId = other.getDataId();
            this->dataName = other.getDataName();
            this->unit = other.getUnit();
            this->description = other.getDescription();
        }
        return (*this);
    }


    QString getKey() const;

    QString getDataId() const;
    void setDataId(const QString &value);

private:

    QString dataId;
    QString dataName;
    QString description;
    QString unit;

};

#endif // DATADESCRIPTOR_H
