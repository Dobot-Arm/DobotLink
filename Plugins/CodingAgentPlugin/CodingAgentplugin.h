#ifndef CODINGAGENTPLUGIN_H
#define CODINGAGENTPLUGIN_H

#include "DPluginInterface.h"

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QMap>
#include <QProcess>
#include <QDir>

#include "MessageCenter/DPacket.h"
class CodingAgentPlugin : public DPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Dobot.plugin.interface")
    Q_INTERFACES(DPluginInterface)
public:
    static const QString PluginName;
    static const QString Version;

    CodingAgentPlugin(QObject *parent = nullptr);
    virtual ~CodingAgentPlugin() override;
    virtual QString getVersion() override;
    void setPort(QString port);
    bool isCodingAgent();
    QString getPort();

private:
    QProcess    *m_process;
    QDir m_workSpaceDir;
    QString m_processPath;
    QString codingport;

    void _processInit();
    void _stopProcess();

private slots:
    virtual void pReceiveMassage_slot(QString id, QJsonObject obj) override;

};

#endif // CODINGAGENTPLUGIN_H
