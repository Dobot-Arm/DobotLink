#ifndef DDFUFILE_H
#define DDFUFILE_H

#include <QObject>
#include <QProcess>
#include <QDir>
#include "DError/DError.h"

class DDfufile : public QObject
{
    Q_OBJECT
public:
    explicit DDfufile(QObject *parent = nullptr);

    enum DeviceType {
        DFU_MAGICBOX,
        DFU_MAGICIANLITE
    } m_type;

    ~DDfufile();
    void setFileName(QString fileName);
    void startDownload(quint64 start_id);
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

#endif // DDFUFILE_H
