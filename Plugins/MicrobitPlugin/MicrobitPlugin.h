#ifndef MICROBITPLUGIN_H
#define MICROBITPLUGIN_H

#include "DPluginInterface.h"

#include <QObject>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QJsonObject>

#include "MicrobitPacket.h"

class MicrobitPlugin : public DPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Dobot.plugin.interface")
    Q_INTERFACES(DPluginInterface)

public:
    static const QString PluginName;
    static const QString Version;

    enum ErrorType{
        DataError = 1,
        FileOpenFail
    };
    enum SourceType{
        MICROBIT_PYTHON,
        MICROBIT_JAVASCRIPT,
        MICROBIT_HEX
    } mSourceType;

    MicrobitPlugin(QObject *parent = nullptr);
    virtual ~MicrobitPlugin() override;
    virtual QString getVersion() override;

    void startUpload();

private:
    MicrobitPacket requstPacket;

    QFile codeFile;
    QDir workSpaceDir;
    QDir codeDir;

    QProcess *m_process;

    void _processInit();
    void _codeFileInit();
    bool handleProgram(const MicrobitPacket &packet);
    bool writeIntoCodeFile(const MicrobitPacket &packet);

private slots:
    virtual void pReceiveMassage_slot(QString id, QJsonObject obj) override;

    void onProcessReadyRead_slot(void);
    void onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus);
    void errorHandle_slot(QProcess::ProcessError error);
};

#endif // MICROBITPLUGIN_H
