#ifndef DYNAMICALGORITHMRPCPLUGIN_H
#define DYNAMICALGORITHMRPCPLUGIN_H

#include "DPluginInterface.h"

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QMap>
#include <QProcess>
#include <QDir>

#include "MessageCenter/DPacket.h"

class DynamicAlgorithmRPCPlugin : public DPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Dobot.plugin.interface")
    Q_INTERFACES(DPluginInterface)

public:
    static const QString PluginName;
    static const QString Version;


    DynamicAlgorithmRPCPlugin(QObject *parent = nullptr);
    virtual ~DynamicAlgorithmRPCPlugin() override;
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

#endif // DYNAMICALGORITHMRPCPLUGIN_H
