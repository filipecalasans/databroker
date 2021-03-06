#ifndef TCPCONTROLCONNECTION_H
#define TCPCONTROLCONNECTION_H

#include "abstractrlconnection.h"

#include <QDataStream>

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

    static const QString ALL_IDLE;
    static const QString ALL_READY;
    static const QString ALL_RUNNING;
    static const QString ALL_PAUSE;

    static const QString FAIL_ALL_IDLE;
    static const QString FAIL_ALL_READY;
    static const QString FAIL_ALL_RUNNING;
    static const QString FAIL_ALL_PAUSE;

    explicit TcpControlConnection(bool masterModule, QObject *parent = 0);
    explicit TcpControlConnection(bool masterModule, const QString& ipAddress,
                                  quint16 portNum, QObject *parent = 0);

    virtual ~TcpControlConnection();

    virtual bool initConnection();
    virtual void deInitConnection();
    virtual bool initConnection(QString ipAddress, quint16 portNum);
    virtual bool receiveControlCommand(Broker::ControlCommand *cmd);
    virtual bool sendControlCommand(Broker::ControlCommand *cmd);

    /* Send the default command with
     * no command arguments in the
     * packet.
     */
    bool sendDefaultResetCommand();
    bool sendDefaultReadyCommand();
    bool sendDefaultStartCommand();
    bool sendDefaultPauseCommand();
    bool sendDefaultResumeCommand();

    bool sendDefaultAllIdleCommand();
    bool sendDefaultAllReadyCommand();
    bool sendDefaultAllRunningCommand();
    bool sendDefaultAllPausedCommand();

    bool sendDefaultFailAllIdleCommand();
    bool sendDefaultFailAllReadyCommand();
    bool sendDefaultFailAllRunningCommand();
    bool sendDefaultFailAllPausedCommand();

    ControlStateType getState() const;

signals:

    void controlStateChanged(const ControlStateType &state);

protected:

    void setState(const ControlStateType &value);
    void runStateTransition(Broker::ControlCommand *cmd);
    bool isDefaultControlCommand(Broker::ControlCommand *cmd);

private:

    ControlStateType state = STATE_DISCONNECTED;

    QDataStream *stream = nullptr;
    QDataStream *streamOut = nullptr;
    QByteArray *dataOut = nullptr;

    quint64 count;

    bool masterModule = false;

private slots:

    void readData();

};

#endif // TcpControlConnection_H
