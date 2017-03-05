#ifndef TCPCONTROLCONNECTION_H
#define TCPCONTROLCONNECTION_H

#include "abstractcontrolconnection.h"

#include <QDataStream>
#include <QTcpServer>

class TcpControlConnection : public AbstractControlConnection
{
    Q_OBJECT
public:

    enum ControlStateType {
        STATE_DISCONNECTED = 0,
        STATE_IDLE = 1,
        STATE_READY = 2,
        STATE_RUNNING = 3,
        STATE_PAUSE = 4
    };
    Q_ENUM(ControlStateType)

    static const QString CMD_RESET;
    static const QString CMD_READY;
    static const QString CMD_START;
    static const QString CMD_PAUSE;
    static const QString CMD_RESUME;

    static const QString REPLY_IDLE;
    static const QString REPLY_READY;
    static const QString REPLY_RUNNING;
    static const QString REPLY_PAUSE;

    TcpControlConnection(QObject *parent = 0);
    TcpControlConnection(quint16 portNum, QObject *parent = 0);

    virtual ~TcpControlConnection();

    virtual void deInitConnection();
    virtual bool initConnection(quint16 portNum);

    virtual bool receiveControlCommand(Broker::ControlCommand *cmd);
    virtual bool sendControlCommand(Broker::ControlCommand *cmd);

    bool notifyMyState();

    ControlStateType getState() const;

protected:

    void setState(const ControlStateType &value);
    void runStateTransition(Broker::ControlCommand *cmd);
    bool isDefaultControlCommand(Broker::ControlCommand *cmd);

private:

    ControlStateType state = STATE_DISCONNECTED;
    QDataStream *stream = nullptr;
    QTcpServer *server = nullptr;

private slots:

    void readData();
    void handleClientStateChanged(QAbstractSocket::SocketState state);

signals:

    void controlStateChanged(const ControlStateType& state);

};

#endif // TcpControlConnection_H
