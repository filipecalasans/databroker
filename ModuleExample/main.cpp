#include <QCoreApplication>
#include <QDebug>

#include "communication/communication.h"

#ifdef VISION
#define PROVIDER_NAME "vision"
#define DATA_PORT 6001
#define COMMAND_PORT 6000
#else

#define PROVIDER_NAME "radio"
#define DATA_PORT 6002
#define COMMAND_PORT 6003
#endif

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Communication *communication = new Communication(DATA_PORT, COMMAND_PORT);
    communication->setModuleName(PROVIDER_NAME);

    Q_UNUSED(communication)

    return a.exec();
}
