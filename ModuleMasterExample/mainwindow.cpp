#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../protocol/control.pb.h"

#define DATA_PORT 6005
#define COMMAND_PORT 6006

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    comm = new Communication(DATA_PORT, COMMAND_PORT);

    connect(comm, &Communication::masterStateChanged,
            [this](const TcpControlConnection::MasterControlStateType& state) {
        qDebug() << "[Master State]" << state;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_resetButton_clicked()
{
    Broker::ControlCommand command;
    *command.add_desitination() = QString("all").toStdString();
    command.set_command(TcpControlConnection::CMD_RESET.toStdString());
    command.set_reply_required(true);
    qDebug() << QString(command.DebugString().c_str());
    comm->sendControlCommand(&command);
}

void MainWindow::on_readyButton_clicked()
{
    Broker::ControlCommand command;
    *command.add_desitination() = QString("all").toStdString();
    command.set_command(TcpControlConnection::CMD_READY.toStdString());
    command.set_reply_required(true);
    qDebug() << QString(command.DebugString().c_str());
    comm->sendControlCommand(&command);
}

void MainWindow::on_startButton_clicked()
{
    Broker::ControlCommand command;
    *command.add_desitination() = QString("all").toStdString();
    command.set_command(TcpControlConnection::CMD_START.toStdString());
    command.set_reply_required(true);
    qDebug() << QString(command.DebugString().c_str());
    comm->sendControlCommand(&command);
}

void MainWindow::on_pauseButton_clicked()
{
    Broker::ControlCommand command;
    *command.add_desitination() = QString("all").toStdString();
    command.set_command(TcpControlConnection::CMD_PAUSE.toStdString());
    command.set_reply_required(true);
    qDebug() << QString(command.DebugString().c_str());
    comm->sendControlCommand(&command);
}

void MainWindow::on_resumeButton_clicked()
{
    Broker::ControlCommand command;
    *command.add_desitination() = QString("all").toStdString();
    command.set_command(TcpControlConnection::CMD_RESUME.toStdString());
    command.set_reply_required(true);
    qDebug() << QString(command.DebugString().c_str());
    comm->sendControlCommand(&command);
}

void MainWindow::on_disconnectButton_clicked()
{

}

void MainWindow::on_connectButton_clicked()
{

}
