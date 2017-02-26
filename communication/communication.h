#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QObject>
#include "../protocol/data.pb.h"

class Communication : public QObject
{
    Q_OBJECT
public:
    explicit Communication(QObject *parent = 0);

signals:

public slots:

private:

    Broker::DataCollection *provided_data;
    Broker::DataCollection *subscribed_data;
    Broker::DataDescriptor *descriptor;

};

#endif // COMMUNICATION_H
