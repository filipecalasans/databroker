#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "protocol/data.pb.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Broker::Data data_sim_model;

}

MainWindow::~MainWindow()
{
    delete ui;
}
