#ifndef DAVRUPLOADER_H
#define DAVRUPLOADER_H

#include <QObject>
#include <QProcess>
#include <QDir>

class DAvrUploader : public QObject
{
    Q_OBJECT

public:
    enum DeviceType{
        ARDUINO_UTYPE_UNO,
        ARDUINO_UTYPE_MEAG2560,
        ARDUINO_UTYPE_UNKNOWN
    } mDeviceType;

    enum ErrorType{
        ARDUINO_NOERROR,
        ARDUINO_ERROR_HEX_FILE_NOTFOUND = 20,
        ARDUINO_ERROR_NO_AVAILABLE_DEVICE,
        ARDUINO_ERROR_NO_TYPE,
        ARDUINO_ERROR_PROCESS_CRASH,
        ARDUINO_ERROR_TIMEOUT,
        ARDUINO_ERROR_UNKNOWN
    };

    QString portName;

    explicit DAvrUploader(QObject *parent = nullptr);
    ~DAvrUploader();

    void startUpload();

signals:
    void uploadFinished_signal(int type, QString massage = QString());
    void onProcessPercent_signal(int percent, QString message = QString());

private:
    QDir workSpaceDir;
    QDir codeDir;
    QFile codeFile;
    int m_percent;

    QString uploadLog;

    QProcess *m_process;

    void _processInit();
    void _codeFileInit();
    void handleProcessPercentValue(int value);

private slots:
    void onStartedHandle_slot();
    void onProcessReadyRead_slot(void);
    void onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus);
    void errorHandle_slot(QProcess::ProcessError error);
};

#endif // DAVRUPLOADER_H
