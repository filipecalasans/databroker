#include "tcpcontrolconnection.h"

#include <QTcpSocket>
#include <QDataStream>

#include <QTcpServer>
#include <QDebug>


const QString TcpControlConnection::CMD_RESET = QString("RESET");
const QString TcpControlConnection::CMD_READY = QString("READY");
const QString TcpControlConnection::CMD_START = QString("START");
const QString TcpControlConnection::CMD_PAUSE = QString("PAUSE");
const QString TcpControlConnection::CMD_RESUME = QString("RESUME");

const QString TcpControlConnection::REPLY_IDLE = QString("IDLE");
const QString TcpControlConnection::REPLY_READY = QString("READY");
const QString TcpControlConnection::REPLY_RUNNING = QString("RUNNING");
const QString TcpControlConnection::REPLY_PAUSE = QString("PAUSE");


TcpControlConnection::TcpControlConnection(QObject *parent) : AbstractControlConnection(parent)
{
    server = new QTcpServer();
    stream = new QDataStream();

    stream->setVersion(QDataStream::Qt_5_7);

    connect(server, &QTcpServer::newConnection, [this]() {
        if(server->hasPendingConnections() && socket == nullptr) {
            socket = server->nextPendingConnection();

            qDebug() << "[New Client]" << socket->localAddress();

            stream->setDevice(socket);
            connect(socket, &QTcpSocket::stateChanged, this, &TcpControlConnection::handleClientStateChanged);
            connect(socket, &QTcpSocket::readyRead, this, &TcpControlConnection::readData);
            setState(ControlStateType::STATE_IDLE);
        }
        else {
            QTcpSocket *discard = server->nextPendingConnection();
            discard->abort();
            discard->deleteLater();

            qDebug() << "[Dropping Client] I am already attending a client";
        }
    });

}

TcpControlConnection::TcpControlConnection(quint16 portNum, QObject *parent) :
    TcpControlConnection(parent)
{
    setPort(portNum);
}

TcpControlConnection::~TcpControlConnection()
{
    delete stream;
    delete server;
}

void TcpControlConnection::deInitConnection()
{
    if(socket) {
        socket->abort();
    }
    server->close();
    bufferReady = false;
    stream->resetStatus();
    stream->setDevice(0);
}

bool TcpControlConnection::initConnection(quint16 portNum)
{
    deInitConnection();
    setPort(portNum);
    return server->listen(QHostAddress::Any, portNum);;
}

bool TcpControlConnection::receiveControlCommand(Broker::ControlCommand *cmd)
{
    bool ret = cmd->ParseFromArray(static_buffer_in, len);
    bufferReady = false;
    return ret;

}

bool TcpControlConnection::sendControlCommand(Broker::ControlCommand *cmd)
{
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
            qDebug() << "[TcpControlConnection::provideDataPublished] len =" << writtenBytes-integerSize;
            return true;
        }

        qDebug() << "[TcpControlConnection::provideDataPublished] Error len != len_sent";
        return false;
    }

    qDebug() << "[TcpControlConnection::provideDataPublished] error, len > MAX_BUFFER_SIZE";
    return false;

}

bool TcpControlConnection::notifyMyState()
{
    Broker::ControlCommand stateNotification;

    switch(state) {

        case ControlStateType::STATE_IDLE:
            stateNotification.set_command(REPLY_IDLE.toStdString());
        break;

        case ControlStateType::STATE_READY:
            stateNotification.set_command(REPLY_READY.toStdString());
        break;

        case ControlStateType::STATE_RUNNING:
            stateNotification.set_command(REPLY_RUNNING.toStdString());
        break;

        case ControlStateType::STATE_PAUSE:
            stateNotification.set_command(REPLY_PAUSE.toStdString());
        break;

        default:
            return false;
        break;
    }

    stateNotification.set_reply_required(false);
    return sendControlCommand(&stateNotification);
}

TcpControlConnection::ControlStateType TcpControlConnection::getState() const
{
    return state;
}

void TcpControlConnection::setState(const ControlStateType &value)
{
    if(value != state) {
        state = value;
        emit controlStateChanged(state);
    }
}

void TcpControlConnection::runStateTransition(Broker::ControlCommand *cmd)
{
    QString command(cmd->command().c_str());

    ControlStateType nxtState = getState();

    switch(state) {
        case STATE_DISCONNECTED:
            /* DO nothing. Only way to get away of this state
             * is after broking connecting to this module */
        break;

        case ControlStateType::STATE_IDLE:
            if(command == CMD_PAUSE) {
                /* Do nothing */
            } else if(command == CMD_READY) {
                nxtState = ControlStateType::STATE_READY;
            } else if(command == CMD_RESET) {
                /* Do Nothing */
            } else if(command == CMD_RESUME) {
                /* Do nothing */
            } else if(command == CMD_START) {
                /* Do nothing */
            }
        break;
        case ControlStateType::STATE_READY:
            if(command == CMD_PAUSE) {
                /* Do nothing */
            } else if(command == CMD_READY) {
                /* DO nothing */
            } else if(command == CMD_RESET) {
                nxtState = ControlStateType::STATE_IDLE;
            } else if(command == CMD_RESUME) {
                /* Do nothing */
            } else if(command == CMD_START) {
                nxtState = ControlStateType::STATE_RUNNING;
            }
        break;
        case ControlStateType::STATE_RUNNING:
            if(command == CMD_PAUSE) {
                nxtState = ControlStateType::STATE_PAUSE;
            } else if(command == CMD_READY) {
                /* Do Nothing */
            } else if(command == CMD_RESET) {
                nxtState = ControlStateType::STATE_IDLE;
            } else if(command == CMD_RESUME) {
                /* Do Nothing */
            } else if(command == CMD_START) {

            }
        break;
        case ControlStateType::STATE_PAUSE:
            if(command == CMD_PAUSE) {
                /* Do Nothing */
            } else if(command == CMD_READY) {
                /* Do nothing */
            } else if(command == CMD_RESET) {
                nxtState = ControlStateType::STATE_IDLE;
            } else if(command == CMD_RESUME) {
                nxtState = ControlStateType::STATE_RUNNING;
            } else if(command == CMD_START) {
                /* DO nothing */
            }
        break;
    }

    setState(nxtState);
}

bool TcpControlConnection::isDefaultControlCommand(Broker::ControlCommand *cmd)
{
    if(cmd->command() == CMD_PAUSE.toStdString()) { return true; }
    if(cmd->command() == CMD_READY.toStdString()) { return true; }
    if(cmd->command() == CMD_RESET.toStdString()) { return true; }
    if(cmd->command() == CMD_RESUME.toStdString()) { return true; }
    if(cmd->command() == CMD_START.toStdString()) { return true; }

    return false;
}

void TcpControlConnection::readData()
{
    while(1) {

        bool emitOverrun = false;

        stream->startTransaction();

        quint32 packet_len;

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
            }
            else {
                bufferReady = true;
                emit receivedControlCommand();
            }
        }
    }
}

void TcpControlConnection::handleClientStateChanged(QAbstractSocket::SocketState state)
{
    if(state == QAbstractSocket::UnconnectedState) {
        if(socket) {
            qDebug() << "[Client gone]" << socket->localAddress();
            socket->disconnect();
            socket->deleteLater();
            socket = nullptr;
        }
        setState(ControlStateType::STATE_DISCONNECTED);
    }
    else if(state == QAbstractSocket::ConnectedState) {
        setState(ControlStateType::STATE_IDLE);
    }
    else {
        setState(ControlStateType::STATE_DISCONNECTED);
    }
}
