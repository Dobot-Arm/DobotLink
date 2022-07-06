#ifndef RUNCMDPLUGIN_H
#define RUNCMDPLUGIN_H

#include "DPluginInterface.h"
#include "MessageCenter/DPacket.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>

class RunCmdPlugin : public DPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Dobot.plugin.interface")
    Q_INTERFACES(DPluginInterface)

public:
    static const QString PluginName;
    static const QString Version;
    QString cmd;

    RunCmdPlugin(QObject *parent = nullptr);
    virtual ~RunCmdPlugin() override;
    virtual QString getVersion() override;

private:
    void _stopProcess(const quint16 wPort = 0, DResultPacket * const resPacket = nullptr);
    void handleStartCommand(const DRequestPacket &packet);
    void handleWaitCommand(const DRequestPacket &packet);
    void handleStopCommand(const DRequestPacket &packet);

    void _notifyStdOut(const QString &data, QProcess * const process, const bool isFiltter = true);
    void _notifyStdErr(const QString &data, const QProcess * const process);
    void _notify(const QString method, const QJsonObject &resObj, const quint16 wPort);
    void _sendWaitResult(int code, QProcess::ExitStatus status);

    bool _checkProcess(const DRequestPacket &packet);

    QProcess* m_process;
    DResultPacket* m_waitResultPack;

    //等待命令是否调用
    std::atomic_bool m_bWaitCommandCalled;
    std::atomic_bool m_bStopCommandCalled;

private slots:
    virtual void pReceiveMassage_slot(QString id, QJsonObject obj) override;
    void processStdout_slot();
    void processStderr_slot();
    void processFinlish_slot(int code, QProcess::ExitStatus status);
    void processStarted_slot();
    void notifyStdOutTimeout_slot();
};
#endif // RUNCMDPLUGIN_H
