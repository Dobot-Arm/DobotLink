#ifndef ARDUINOPLUGIN_H
#define ARDUINOPLUGIN_H

#include "DPluginInterface.h"

#include <QObject>
#include <QSerialPort>
#include <QJsonObject>
#include "ArduinoPacket.h"
#include "DAvrCompiler.h"
#include "DAvrUploader.h"

class ArduinoPlugin : public DPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Dobot.plugin.interface")
    Q_INTERFACES(DPluginInterface)

public:
    enum ErrorType{
        DataError = 1,
        TypeError,
        PortError,
        FileOpenFail,
        IsBusy,
        TimeOut,
        ERROR_DL_UNKOWN
    };

    static const QString PluginName;
    static const QString Version;

    ArduinoPlugin(QObject *parent = nullptr);
    virtual ~ArduinoPlugin() override;
    virtual QString getVersion() override;


private:
    quint64 requestId;
    quint16 m_port;
    bool m_isBusy;
    QFile codeFile;
    QSerialPort *m_serialPort;

    DAvrCompiler *m_compiler;
    DAvrUploader *m_uploader;

    void _codefileInit();
    bool handleProgram(const ArduinoPacket &packet);
    bool writeIntoCodeFile(QString text);

private slots:
    virtual void pReceiveMassage_slot(QString id, QJsonObject obj) override;
    void handleCompileFinish_slot(int type, QString massage = QString());
    void handleUploadFinish_slot(int type, QString massage = QString());
    void handleProcessPercent_slot(int percent, QString message);
};

#endif // ARDUINOPLUGIN_H
