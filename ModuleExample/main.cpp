#include <QCoreApplication>
#include <QDebug>

#include "communication/communication.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Communication *communication = new Communication();

    Q_UNUSED(communication)

    return a.exec();
}
