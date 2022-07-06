#ifndef DKFLASH_H
#define DKFLASH_H

#include <QObject>
#include <QProcess>
#include <QDir>
#include "DError/DError.h"

class DKflash : public QObject
{
    Q_OBJECT
public:
    explicit DKflash(QObject *parent = nullptr);

    ~DKflash();
    void setFileName(QString fileName, QString device);
    void startDownload(QString COM, quint64 start_id);
    void stopDownload(quint64 stop_id);
    void stopProcess();

signals:
    void onProcessReadyRead_signal(QString message);
    void onFinish_signal(bool ok, quint64 id);
    void onStop_signal(quint64 id);
    void onProcessError_signal(quint64 id, int code, QString message);

private:
    QDir m_workSpaceDir;
    QString m_fileName;

    QProcess *m_process;

    quint64 start_id;

    QString device;

    void _processInit();
    void _codefileInit();

private slots:
    void onProcessReadyRead_slot(void);
    void onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus);
    void errorHandle_slot(QProcess::ProcessError error);

};

#endif // DKFLASH_H
