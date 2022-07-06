#ifndef DEBUGGERPLUGIN_H
#define DEBUGGERPLUGIN_H

#include "DPluginInterface.h"

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QMap>
#include <QProcess>
#include <QDir>

#include "MessageCenter/DPacket.h"


class DebuggerPlugin : public DPluginInterface
{
    Q_OBJECT
        Q_PLUGIN_METADATA(IID "Dobot.plugin.interface")
        Q_INTERFACES(DPluginInterface)

    public:
        static const QString PluginName;
        static const QString Version;


        DebuggerPlugin(QObject *parent = nullptr);
        virtual ~DebuggerPlugin() override;
        virtual QString getVersion() override;

    private:
        QProcess    *m_process;
        QDir m_workSpaceDir;
        QString m_processPath;
        QDir logDir;
        QString log_path;

        void setLog_dir();
        void _processInit();
        void _stopProcess();


    private slots:
        virtual void pReceiveMassage_slot(QString id, QJsonObject obj) override;
};

#endif // DEBUGGERPLUGIN_H
