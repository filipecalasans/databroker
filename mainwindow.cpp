#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    broker = new DataBroker();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_resetButton_clicked()
{
    broker->resetModules();
}

void MainWindow::on_readyButton_clicked()
{
    broker->readyModules();
}

void MainWindow::on_startButton_clicked()
{
    broker->startModules();
}

void MainWindow::on_pauseButton_clicked()
{
    broker->pauseModules();
}

void MainWindow::on_resumeButton_clicked()
{
    broker->resumeModules();
}

void MainWindow::on_disconnectButton_clicked()
{
    broker->disconnectModules();
}

void MainWindow::on_connectButton_clicked()
{
    broker->connectModules();
}
