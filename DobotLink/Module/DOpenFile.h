#ifndef DOPENFILE_H
#define DOPENFILE_H

#include <QObject>
#include <QProcess>

class DOpenFile : public QObject
{
    Q_OBJECT
public:
    static DOpenFile *getInstance();

    void openFileWithVSCode(QString fileName);
    void openFileWithDesktopServices(QString fileName);

private:
    explicit DOpenFile(QObject *parent = nullptr);

    QProcess *m_vscodeProcess;

    void _processInit();

private slots:
    void onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus);
    void errorHandle_slot(QProcess::ProcessError error);
};

#endif // DOPENFILE_H
