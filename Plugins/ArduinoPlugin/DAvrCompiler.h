#ifndef DAVRCOMPILER_H
#define DAVRCOMPILER_H

#include <QObject>
#include <QProcess>
#include <QDir>

class DAvrCompiler : public QObject
{
    Q_OBJECT
public:
    enum DeviceType{
        ARDUINO_CTYPE_UNO,
        ARDUINO_CTYPE_MEAG2560,
        ARDUINO_CTYPE_AISTARTER,
        ARDUINO_CTYPE_MOBILEPLATFORM,
        ARDUINO_CTYPE_SMARTKIT
    } mDeviceType;

    enum ErrorType{
        ARDUINO_NOERROR,
        ARDUINO_ERROR_TOOL_NOTFOUND = 10,
        ARDUINO_ERROR_CODEFILE_NOTFOUND,
        ARDUINO_ERROR_PROCESS_CRASH,
        ARDUINO_ERROR_GPP_FAIL,
        ARDUINO_ERROR_GCC_FAIL,
        ARDUINO_ERROR_OBJCOPY_FAIL
    };

    explicit DAvrCompiler(QObject *parent = nullptr);
    ~DAvrCompiler();

    QString getDeviceName(DeviceType type);
    void startCompile();

signals:
    void compileFinished_signal(int type, QString massage = QString());
    void onProcessPercent_signal(int percent, QString message = QString());

private:
    QDir workSpaceDir;
    QDir codeDir;
    QFile codeFile;

    QProcess *gppProcess;
    QProcess *gccProcess;
    QProcess *objcopyProcess;

    void _processInit();
    void _codefileInit();

private slots:
    void onProcessReadyRead_slot(void);
    void onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus);
    void errorHandle_slot(QProcess::ProcessError error);
};

#endif // DAVRCOMPILER_H
