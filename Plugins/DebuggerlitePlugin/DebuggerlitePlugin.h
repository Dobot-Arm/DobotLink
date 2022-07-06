#ifndef DEBUGGERLITEPLUGIN_H
#define DEBUGGERLITEPLUGIN_H

#include "DPluginInterface.h"
#include "MessageCenter/DPacket.h"
#include <QJsonArray>
#include <QMap>
#include "DProcess.h"

class DebuggerLitePlugin : public DPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Dobot.plugin.interface")
    Q_INTERFACES(DPluginInterface)

public:
    enum ErrorType{
        DebuggerStateError = 1,
        DebuggerStartFailed,
        ERROR_DL_UNKOWN
    };

    static const QString PluginName;
    static const QString Version;

    DebuggerLitePlugin(QObject *parent = nullptr);
    virtual ~DebuggerLitePlugin() override;
    virtual QString getVersion() override;

private:
    inline DProcess* _getProc(const DRequestPacket &packet);

    void _handleCloseWebSocket(const quint16 &wPort = 0);
    void _handleStartCommand(const DRequestPacket &packet);
    void _handlePrepareCommand(const DRequestPacket &packet);
    void _handleWaitCommand(const DRequestPacket &packet);
    void _handleStopCommand(const DRequestPacket &packet);
    void _handleEmergencyStopCommand(const DRequestPacket &packet);
    void _handleInputCommand(const DRequestPacket &packet);
    void _handleSetupCustomModuleCommand(const DRequestPacket &packet);
    void _handleStartAllCommand(const DRequestPacket &packet);
    void _handleStopAllCommand(const DRequestPacket &packet);
    void _handleGetProcessStatusCommand(const DRequestPacket &packet);

    void stopProcess();

    QMap<quint64, DProcess*> m_procs;


private slots:
    virtual void pReceiveMassage_slot(QString id, QJsonObject obj) override;

    void slotFinish();
    void slotNotifyStdout(QJsonArray datas);
    void slotNotifyStderr(QJsonArray datas);
};

#endif // DEBUGGERLITEPLUGIN_H
