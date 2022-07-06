#ifndef DMCUISPDOWNLOAD_H
#define DMCUISPDOWNLOAD_H

#include <QObject>
#include <QProcess>
#include <QDir>
#include "DError/DError.h"

class DMcuisp : public QObject
{
    Q_OBJECT
public:
    explicit DMcuisp(QObject *parent = nullptr);

    ~DMcuisp();

    void setFileName(QString fileName);
    void startDownload(QString COM, quint64 start_id, bool is3DPrinter=false);
    void stopDowenload(quint64 stop_id);
    void stopProcess();

signals:
    void onProcessReadyRead_signal(QString message);
    void onFinish_signal(bool ok, quint64 id);
    void onStop_signal(quint64 id);
    void onProcessError_signal(quint64 id, int code, QString message);

private:
    QDir m_workSpaceDir;
    QString m_fileName;

    QString m_strSerialPort;//only used in linux

    QProcess *m_process;

    quint64 start_id;

    void _processInit();
    void _codefileInit();

    //linux固件下载后，需要自己重启
    void downloadFirwareForLinux(const QString& strCOM);
    void restartSTM32FlashForLinux();

private slots:
    void onProcessReadyRead_slot(void);
    void onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus);
    void errorHandle_slot(QProcess::ProcessError error);
};

#endif // DMCUISPDOWNLOAD_H
