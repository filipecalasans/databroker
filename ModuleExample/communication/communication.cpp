#include "communication.h"

#include "../broker/protocol/data.pb.h"
#include "../broker/protocol/control.pb.h"

#include <QDateTime>
#include <QTimerEvent>

Communication::Communication(QObject *parent) : QObject(parent)
{
    initControlConnection();
    initDataConnection();

    startTimer(500);
}

bool Communication::initDataConnection()
{
#ifdef TCP_CONNECTION

    dataConnection = new TcpDataConnection(6001);

    connect(dataConnection, &AbstractDataConnection::receivedDataConsumed, [this](){
        Broker::DataCollection packet;
        dataConnection->receiveDataConsumed(&packet);
        qDebug() << "[CONSUMED] Size:" << packet.ByteSize();
        qDebug() << "[CONSUMED]" << QString::fromStdString(packet.DebugString());
    });

    if(!dataConnection->initConnection(6001)) {
        qDebug() << "Can not init connection";
        return false;
    }
    else {
        qDebug() << "Listening port 60001";
    }
#else

    dataConnection = new UdpDataConnection();

    connect(dataConnection, &AbstractDataConnection::receivedDataConsumed, [this](){
        Broker::DataCollection packet;
        dataConnection->receiveDataConsumed(&packet);
        qDebug() << "[RCVD CONSUMED] Size:" << packet.ByteSize();
        qDebug() << "[RCVD CONSUMED]" << QString::fromStdString(packet.DebugString());
        qDebug() << "============================================================================";
    });

    if(!dataConnection->initConnection(6001)) {
        qDebug() << "Can not init connection";
        return false;
    }
    else {
        qDebug() << "Listening Data Server on port" << dataConnection->getPort();
    }

#endif
    return true;
}

bool Communication::initControlConnection()
{
    controlConnection = new TcpControlConnection();

    connect(controlConnection, &TcpControlConnection::controlStateChanged,
            [this](TcpControlConnection::ControlStateType state) {

        qDebug() << "[STATE]" << state;
        controlConnection->notifyMyState();
    });

    connect(controlConnection, &AbstractControlConnection::receivedControlCommand, [this](){
        Broker::ControlCommand packet;
        controlConnection->receiveControlCommand(&packet);

        qDebug() << "[RCVD CONTROL] Size:" << packet.ByteSize();
        qDebug() << "[RCVD CONTROL]" << QString::fromStdString(packet.DebugString());
        qDebug() << "============================================================================";
    });

    if(!controlConnection->initConnection(6000)) {
        qDebug() << "Can not init control connection";
        return false;
    }
    else {
        qDebug() << "Listening Control Server on port" << controlConnection->getPort();
    }

    return true;
}

void Communication::timerEvent(QTimerEvent *event)
{
    event->accept();

    static int count = 0;

    if(controlConnection->getState() == TcpControlConnection::STATE_RUNNING) {
        Broker::DataCollection data;

        data.set_provider_name("vision");
        data.set_timestamp(QDateTime::currentDateTime().toMSecsSinceEpoch());
        for(int i=0; i<5; i++) {
            Broker::Data *p = data.add_data_provided();
            p->set_data_id(QString("p%1").arg(i).toStdString());
            p->set_data_type(Broker::DATA_DOUBLE);
            p->set_data_double(qrand()/1.76);
        }

        if(dataConnection->getIsReady()) {
            dataConnection->provideDataPublished(&data);
        }

        if(((count++) % 2) == 0) {
            Broker::ControlCommand command;
            command.set_reply_required(false);
            command.set_command("ERROR");
            Broker::ControlCommandArguments *arg = command.add_args();
            arg->set_data_type(Broker::CTRL_STRING);
            arg->set_data_string(QString("CAN'T FIND ROBOTS").toStdString());
            (*command.add_desitination()) = "radio";
            (*command.add_desitination()) = "ai";
            (*command.add_desitination()) = "manager";
            controlConnection->sendControlCommand(&command);
        }
    }

}
