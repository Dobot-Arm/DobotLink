#ifndef DMULTIFUNC_H
#define DMULTIFUNC_H
#include <QObject>
#include <QProcess>
#include <QDir>
#include "DError/DError.h"

class DMultiFunc : public QObject
{
    Q_OBJECT
public:
    explicit DMultiFunc(QObject *parent = nullptr);

    ~DMultiFunc();
    void setFileName(QString fileName);
    void startDownload(QString COM, QString version, quint64 start_id);
    void stopDownload(quint64 id);
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

    void _processInit();
    void _codefileInit();


private slots:
    void onProcessReadyRead_slot(void);
    void onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus);
    void errorHandle_slot(QProcess::ProcessError error);
};

#endif // DMULTIFUNC_H
