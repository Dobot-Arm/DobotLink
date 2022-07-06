#ifndef PYIMAGEOMPLUGIN_H
#define PYIMAGEOMPLUGIN_H

#include "DPluginInterface.h"

#include <QObject>
#include <QJsonObject>
#include <QTimer>
#include <QMap>
#include <QProcess>
#include <QDir>

class PyImageOMPlugin : public DPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Dobot.Dobot.plugin.interface")
    Q_INTERFACES(DPluginInterface)

public:
    static const QString PluginName;
    static const QString Version;

    PyImageOMPlugin(QObject *parent = nullptr);
    virtual ~PyImageOMPlugin() override;
    virtual QString getVersion() override;
    void setLog_dir();

private:
    QProcess *m_process;
    QDir m_workSpaceDir;
    QString m_processPath;
    QDir logDir;
    QString log_path;

    void _processInit();
    void _stopProcess();
    
private slots:
    virtual void pReceiveMassage_slot(QString id, QJsonObject obj) override;

};

#endif // PYIMAGEOMPLUGIN_H
