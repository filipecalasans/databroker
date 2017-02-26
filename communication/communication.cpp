#include "communication.h"

Communication::Communication(QObject *parent) : QObject(parent)
{
    provided_data = new Broker::DataCollection();
    subscribed_data = new Broker::DataCollection();
}
