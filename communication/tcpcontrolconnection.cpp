#include "tcpcontrolconnection.h"

#include <QTcpSocket>
#include <QDataStream>
#include <QDebug>

const QString TcpControlConnection::CMD_RESET = QString("RESET");
const QString TcpControlConnection::CMD_READY = QString("READY");
const QString TcpControlConnection::CMD_START = QString("START");
const QString TcpControlConnection::CMD_PAUSE = QString("PAUSE");
const QString TcpControlConnection::CMD_RESUME = QString("RESUME");

const QString TcpControlConnection::REPLY_IDLE = QString("REPLY IDLE");
const QString TcpControlConnection::REPLY_READY = QString("REPLY READY");
const QString TcpControlConnection::REPLY_RUNNING = QString("REPLY RUNNING");
const QString TcpControlConnection::REPLY_PAUSE = QString("REPLY PAUSE");

const QString TcpControlConnection::ALL_IDLE = QString("ALL IDLE");
const QString TcpControlConnection::ALL_READY = QString("ALL READY");
const QString TcpControlConnection::ALL_RUNNING = QString("ALL RUNNING");
const QString TcpControlConnection::ALL_PAUSE = QString("ALL PAUSE");

const QString TcpControlConnection::FAIL_ALL_IDLE = QString("FAIL ALL IDLE");
const QString TcpControlConnection::FAIL_ALL_READY = QString("FAIL ALL READY");
const QString TcpControlConnection::FAIL_ALL_RUNNING = QString("FAIL ALL RUNNING");
const QString TcpControlConnection::FAIL_ALL_PAUSE = QString("FAIL ALL PAUSE");

TcpControlConnection::TcpControlConnection(bool masterModule, QObject *parent) :
    AbstractControlConnection(parent),
    masterModule(masterModule)
{
    socket = new QTcpSocket();
    stream = new QDataStream(socket);

    dataOut = new QByteArray();
    streamOut = new QDataStream(dataOut, QIODevice::ReadWrite);

    stream->setVersion(QDataStream::Qt_5_7);
    streamOut->setVersion(QDataStream::Qt_5_7);

    connect(socket, &QTcpSocket::stateChanged, [this](QAbstractSocket::SocketState socketState) {
        if(socketState == QAbstractSocket::ConnectedState) {
            setState(ControlStateType::STATE_IDLE);
        }
        else {
            setState(ControlStateType::STATE_DISCONNECTED);
        }
    });

    connect(socket, &QTcpSocket::readyRead, this, &TcpControlConnection::readData);
}

TcpControlConnection::TcpControlConnection(bool masterModule,
        const QString& ipAddress, quint16 portNum, QObject *parent) :
    TcpControlConnection(masterModule, parent)
{
    setIp(ipAddress);
    setPort(portNum);
}

TcpControlConnection::~TcpControlConnection()
{
    if(socket) {
        delete socket;
    }
    delete stream;
    delete streamOut;
}

bool TcpControlConnection::initConnection()
{
    return AbstractControlConnection::initConnection();
}

void TcpControlConnection::deInitConnection()
{
    if(socket) {
        socket->abort();
    }

    bufferReady = false;

    if(stream) {
        stream->resetStatus();
    }
}

bool TcpControlConnection::initConnection(QString ipAddress, quint16 portNum)
{
    socket->connectToHost(ipAddress, portNum);
    return true;
}

bool TcpControlConnection::receiveControlCommand(Broker::ControlCommand *cmd)
{
    bool ret = cmd->ParseFromArray(static_buffer_in, len);
    bufferReady = false;
    return ret;
}

bool TcpControlConnection::sendControlCommand(Broker::ControlCommand *cmd)
{
    if(getState() == STATE_DISCONNECTED) {
        return false;
    }

    quint32 writeLen = cmd->ByteSize();

    QByteArray array;
    QDataStream integer(&array, QIODevice::ReadWrite);

    if(writeLen <= MAX_BUFFER_SIZE) {
        integer << writeLen;
        memcpy(static_buffer_out, array.data(), array.size());
        int integerSize = array.size();
        writeLen += integerSize;
        cmd->SerializeToArray(static_buffer_out+integerSize, MAX_BUFFER_SIZE);
        int writtenBytes = socket->write(static_buffer_out, writeLen);
        if(writeLen == (quint64)writtenBytes) {
            qDebug() << "[TcpControlConnection::provideDataConsumed] len =" << writtenBytes-integerSize;
            return true;
        }

        qDebug() << "[TcpControlConnection::provideDataConsumed] Error len != len_sent";
        return false;
    }

    qDebug() << "[TcpControlConnection::provideDataConsumed] error, len > MAX_BUFFER_SIZE";
    return false;
}

bool TcpControlConnection::sendDefaultResetCommand()
{
    Broker::ControlCommand cmd;
    cmd.set_command(CMD_RESET.toStdString().c_str());
    cmd.set_reply_required(true);
    return sendControlCommand(&cmd);
}

bool TcpControlConnection::sendDefaultReadyCommand()
{
    Broker::ControlCommand cmd;
    cmd.set_command(CMD_READY.toStdString().c_str());
    cmd.set_reply_required(true);
    return sendControlCommand(&cmd);
}

bool TcpControlConnection::sendDefaultStartCommand()
{
    Broker::ControlCommand cmd;
    cmd.set_command(CMD_START.toStdString().c_str());
    cmd.set_reply_required(true);
    return sendControlCommand(&cmd);
}

bool TcpControlConnection::sendDefaultPauseCommand()
{
    Broker::ControlCommand cmd;
    cmd.set_command(CMD_PAUSE.toStdString().c_str());
    cmd.set_reply_required(true);
    return sendControlCommand(&cmd);
}

bool TcpControlConnection::sendDefaultAllIdleCommand()
{
    Broker::ControlCommand cmd;
    cmd.set_command(ALL_IDLE.toStdString().c_str());
    cmd.set_reply_required(true);
    return sendControlCommand(&cmd);
}


bool TcpControlConnection::sendDefaultAllReadyCommand()
{
    Broker::ControlCommand cmd;
    cmd.set_command(ALL_READY.toStdString().c_str());
    cmd.set_reply_required(true);
    return sendControlCommand(&cmd);
}

bool TcpControlConnection::sendDefaultAllPausedCommand()
{
    Broker::ControlCommand cmd;
    cmd.set_command(ALL_PAUSE.toStdString().c_str());
    cmd.set_reply_required(true);
    return sendControlCommand(&cmd);
}

bool TcpControlConnection::sendDefaultAllRunningCommand()
{
    Broker::ControlCommand cmd;
    cmd.set_command(ALL_RUNNING.toStdString().c_str());
    cmd.set_reply_required(true);
    return sendControlCommand(&cmd);
}


bool TcpControlConnection::sendDefaultFailAllIdleCommand()
{
    Broker::ControlCommand cmd;
    cmd.set_command(ALL_IDLE.toStdString().c_str());
    cmd.set_reply_required(false);
    return sendControlCommand(&cmd);
}


bool TcpControlConnection::sendDefaultFailAllReadyCommand()
{
    Broker::ControlCommand cmd;
    cmd.set_command(FAIL_ALL_READY.toStdString().c_str());
    cmd.set_reply_required(false);
    return sendControlCommand(&cmd);
}

bool TcpControlConnection::sendDefaultFailAllPausedCommand()
{
    Broker::ControlCommand cmd;
    cmd.set_command(FAIL_ALL_PAUSE.toStdString().c_str());
    cmd.set_reply_required(false);
    return sendControlCommand(&cmd);
}

bool TcpControlConnection::sendDefaultFailAllRunningCommand()
{
    Broker::ControlCommand cmd;
    cmd.set_command(FAIL_ALL_RUNNING.toStdString().c_str());
    cmd.set_reply_required(false);
    return sendControlCommand(&cmd);
}

TcpControlConnection::ControlStateType TcpControlConnection::getState() const
{
    return state;
}

void TcpControlConnection::setState(const ControlStateType &value)
{
    if(state != value) {
        state = value;
        emit controlStateChanged(state);
    }
}

void TcpControlConnection::runStateTransition(Broker::ControlCommand *cmd)
{
    QString command(cmd->command().c_str());

    ControlStateType nxtState = getState();

    if(command == REPLY_IDLE) {
        nxtState = STATE_IDLE;
    } else if(command == REPLY_RUNNING) {
        nxtState = STATE_RUNNING;
    } else if(command == REPLY_READY) {
        nxtState = STATE_READY;
    } else if(command == REPLY_PAUSE) {
        nxtState = STATE_PAUSE;
    }

    setState(nxtState);
}

bool TcpControlConnection::isDefaultControlCommand(Broker::ControlCommand *cmd)
{
    if(cmd->command() == REPLY_IDLE.toStdString()) { return true; }
    if(cmd->command() == REPLY_PAUSE.toStdString()) { return true; }
    if(cmd->command() == REPLY_READY.toStdString()) { return true; }
    if(cmd->command() == REPLY_RUNNING.toStdString()) { return true; }

    return false;
}

bool TcpControlConnection::sendDefaultResumeCommand()
{
    Broker::ControlCommand cmd;
    cmd.set_command(CMD_RESUME.toStdString().c_str());
    cmd.set_reply_required(true);
    return  sendControlCommand(&cmd);
}

void TcpControlConnection::readData()
{
    while(1) {

        bool emitOverrun = false;

        stream->startTransaction();

        quint32 packet_len = 0;

        (*stream) >> packet_len;

        if(!stream->commitTransaction() || packet_len == 0) {
            return;
        }

        if(packet_len > MAX_BUFFER_SIZE) {
            stream->readRawData(static_buffer_in, MAX_BUFFER_SIZE);
            return;
        }

        int read_len = stream->readRawData(static_buffer_in, packet_len);

        qDebug() << "[TcpControlConnection::readData] Data received" << read_len;

        if((quint64)read_len == packet_len) {
            len = packet_len;
            if(bufferReady) {
                emitOverrun = true;
                qDebug() << "[TcpControlConnection::readData] Overrun ocurred";
            }

            if(emitOverrun) {
                emit overrunIdentified();
            }

            Broker::ControlCommand cmd;
            if(receiveControlCommand(&cmd)) {
                if(isDefaultControlCommand(&cmd)) {
                    runStateTransition(&cmd);
                }
                else {
                    bufferReady = true;
                    emit receivedControlCommand();
                }
            }
        }
    }
}
