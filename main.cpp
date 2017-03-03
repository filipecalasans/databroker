#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

#include "databroker.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    DataBroker broker;

    return a.exec();
}
