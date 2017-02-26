#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "protocol/data.pb.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Broker::DataCollection provided_data;
    Broker::DataCollection subscribed_data;
}

MainWindow::~MainWindow()
{
    delete ui;
}
