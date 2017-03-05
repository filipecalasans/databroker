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
    for(Module *m : broker->getModules()) {
        m->getCommunication()->getControlConnection()->sendDefaultResetCommand();
    }
}

void MainWindow::on_readyButton_clicked()
{
    for(Module *m : broker->getModules()) {
        m->getCommunication()->getControlConnection()->sendDefaultReadyCommand();
    }
}

void MainWindow::on_startButton_clicked()
{
    for(Module *m : broker->getModules()) {
        m->getCommunication()->getControlConnection()->sendDefaultStartCommand();
    }
}

void MainWindow::on_pauseButton_clicked()
{
    for(Module *m : broker->getModules()) {
        m->getCommunication()->getControlConnection()->sendDefaultPauseCommand();
    }
}

void MainWindow::on_resumeButton_clicked()
{
    for(Module *m : broker->getModules()) {
        m->getCommunication()->getControlConnection()->sendDefaultResumeCommand();
    }
}

void MainWindow::on_disconnectButton_clicked()
{
    for(Module *m : broker->getModules()) {
        m->getCommunication()->getControlConnection()->deInitConnection();
    }
}

void MainWindow::on_connectButton_clicked()
{
    for(Module *m : broker->getModules()) {
        m->getCommunication()->getControlConnection()->initConnection();
    }
}
