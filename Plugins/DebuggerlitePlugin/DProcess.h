#ifndef DPROCESS_H
#define DPROCESS_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QTimer>
#include <QTemporaryFile>
#include <QProcess>
#include <atomic>

class DProcess : public QObject
{
    Q_OBJECT
public:
    explicit DProcess(const quint16 &wPort, QObject *parent = nullptr);
    ~DProcess();

    int dpid() {
        return m_dpid;
    }

    int wPort() {
        return m_wPort;
    }

    void setWaitId(const quint64 &id) {
        m_waitId = id;
    }

    quint64 waitId() {
        return m_waitId;
    }

    bool isRunning() {
        return m_proc->state() != QProcess::NotRunning;
    }

    inline void setWaitCmdFinished(bool bValue)
    {
        m_bWaitCommandFinished.store(bValue);
    }
    inline bool isWaitCmdFinished() const
    {
        return m_bWaitCommandFinished.load();
    }

    bool prepare(const QString &b64Script, const QString &portName, const QString &token, const QString &host);
    bool start();
    bool stop();
    bool write(const QString &cmd);

    void stopProcess();
    static void forceStopProcessAll();

signals:
    void sigNotifyStdout(QJsonArray datas);
    void sigNotifyStderr(QJsonArray datas);
    void sigFinish();

private:
    inline QJsonObject _msgObj(QString msg) {
        QDateTime dateTime = QDateTime::currentDateTime();
//        QString timestamp = dateTime.toString("yyyy-MM-dd hh:mm:ss.zzz");
        QString timestamp = dateTime.toString("hh:mm:ss.zzz");

        QJsonObject msgObj;
        msgObj.insert("time", timestamp);
        msgObj.insert("data", msg);
        return msgObj;
    }

    quint64 m_dpid;
    quint64 m_pid;

    QJsonArray m_stdoutDatas;

    QTimer *m_timerStdout;
    QTemporaryFile *m_tempFile;
    QProcess *m_proc;
    quint16 m_wPort;
    quint64 m_waitId;

    //等待命令是否调用
    std::atomic_bool m_bWaitCommandFinished;

private slots:
    void slotProcStdout();
    void slotProcStderr();
    void slotProcFinish(int code, QProcess::ExitStatus status);
    void slotProcStarted();
};

#endif // DPROCESS_H
