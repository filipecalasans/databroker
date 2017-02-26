#ifndef MODULE_H
#define MODULE_H

#include <QObject>

class Module : public QObject
{
    Q_OBJECT
public:
    explicit Module(QObject *parent = 0);

private:

    QString name;
    QString description;
    QStringList providedData;
    QStringList subscribedData;
    bool mustBeConnectedToStart = true;

};

#endif // MODULE_H
