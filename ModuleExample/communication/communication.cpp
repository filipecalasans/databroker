#include "communication.h"

#include "../broker/protocol/data.pb.h"
#include "../broker/protocol/control.pb.h"

#include <QDateTime>
#include <QTimerEvent>

Communication::Communication(quint16 dataPort, quint16 controlPort, QObject *parent) :
    QObject(parent),
    dataPort(dataPort),
    controlPort(controlPort)
{
    initControlConnection();
    initDataConnection();

//    startTimer(500);
}

bool Communication::initDataConnection()
{
#ifdef TCP_CONNECTION

    dataConnection = new TcpDataConnection(dataPort);

    connect(dataConnection, &AbstractDataConnection::receivedDataConsumed, [this](){
        Broker::DataCollection packet;
        dataConnection->receiveDataConsumed(&packet);
        qDebug() << "[CONSUMED] Size:" << packet.ByteSize();
        qDebug() << "[CONSUMED]" << QString::fromStdString(packet.DebugString());
    });

    if(!dataConnection->initConnection(dataPort)) {
        qDebug() << "Can not init connection";
        return false;
    }
    else {
        qDebug() << "Listening port 6001";
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

    if(!dataConnection->initConnection(dataPort)) {
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

    connect(controlConnection, &TcpControlConnection::masterStateChanged,
            this, &Communication::masterStateChanged);

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

    if(!controlConnection->initConnection(controlPort)) {
        qDebug() << "Can not init control connection port:" << controlPort;
        return false;
    }
    else {
        qDebug() << "Listening Control Server on port" << controlConnection->getPort();
    }

    return true;
}

QString Communication::getModuleName() const
{
    return moduleName;
}

void Communication::setModuleName(const QString &value)
{
    moduleName = value;
}

void Communication::timerEvent(QTimerEvent *event)
{
    event->accept();

    static int count = 0;

    if(controlConnection->getState() == TcpControlConnection::STATE_RUNNING) {
        Broker::DataCollection data;

#ifdef VISION
        data.set_provider_name(getModuleName().toStdString());
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
#else

        data.set_provider_name(getModuleName().toStdString());
        data.set_timestamp(QDateTime::currentDateTime().toMSecsSinceEpoch());
        for(int i=0; i<2; i++) {
            Broker::Data *p = data.add_data_provided();
            p->set_data_id(QString("v%1").arg(i).toStdString());
            p->set_data_type(Broker::DATA_DOUBLE);
            p->set_data_double(qrand()/1.76);
        }

        if(dataConnection->getIsReady()) {
            dataConnection->provideDataPublished(&data);
        }
#endif




#ifdef VISION
        if(((count++) % 2) == 0) {
            Broker::ControlCommand command;
            command.set_reply_required(false);
            command.set_command("VISION CMD");
            Broker::ControlCommandArguments *arg = command.add_args();
            arg->set_data_type(Broker::CTRL_STRING);
            arg->set_data_string(QString("CAN'T FIND ROBOTS").toStdString());
            //(*command.add_desitination()) = "radio";
            //(*command.add_desitination()) = "ai";
            //(*command.add_desitination()) = "all";
            controlConnection->sendControlCommand(&command);
        }
#else
        if(((count++) % 2) == 0) {
            Broker::ControlCommand command;
            command.set_reply_required(false);
            command.set_command("RADIO CMD");
            Broker::ControlCommandArguments *arg = command.add_args();
            arg->set_data_type(Broker::CTRL_STRING);
            arg->set_data_string(QString("CAN'T FIND ROBOTS").toStdString());
            //(*command.add_desitination()) = "vision";
            //(*command.add_desitination()) = "ai";
            //(*command.add_desitination()) = "all";
            controlConnection->sendControlCommand(&command);
        }
#endif

    }
}

bool Communication::sendControlCommand(Broker::ControlCommand *command)
{
    if(controlConnection->getState() != TcpControlConnection::STATE_DISCONNECTED) {
        return controlConnection->sendControlCommand(command);
    }
    return false;
}

