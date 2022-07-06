#ifndef MAGICIANPROJOYSTICKPLUGIN_H
#define MAGICIANPROJOYSTICKPLUGIN_H

#include "DPluginInterface.h"

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QMap>
#include <QProcess>
#include <QDir>

#include "MessageCenter/DPacket.h"

class MagicianProJoystickPlugin : public DPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Dobot.plugin.interface")
    Q_INTERFACES(DPluginInterface)

public:
    static const QString PluginName;
    static const QString Version;

    MagicianProJoystickPlugin(QObject *parent = nullptr);

    virtual ~MagicianProJoystickPlugin();

    QString getVersion() override;

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

#endif // MAGICIANPROJOYSTICKPLUGIN_H
