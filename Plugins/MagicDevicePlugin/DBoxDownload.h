#ifndef DBOXDOWNLOAD_H
#define DBOXDOWNLOAD_H
#include <QThread>
#include <QObject>
#include <QJsonObject>
#include <QDir>
#include <QFile>
class DBoxDownload : public QThread
{
    Q_OBJECT
public:
    DBoxDownload(QObject* par);
    ~DBoxDownload();

    bool searchDeviceDrive();
    bool writeToTempFile(QString text);
    bool copyTempToMagicBox();

    void handleProgramRequest(QString fileName, QString code, quint64 id);
    bool writeToBoxFile(QString code);
    void setDownloadParams(QString fileName, QString code, quint64 id);
    void run();

    static bool isCanWriteCode(QString code);
    static bool clearBoxSpace();
signals:
    void onDownloadFinished_signal(quint64 id, bool isOk);

private:
    QDir m_localTempDir;
    QDir m_boxDir;

    QFile m_codePyFile;
    QFile m_boxPyFile;

    quint64 m_handleID;
    bool m_isDownloading;

    QString m_fileName;
    quint64 m_id;
    QString m_code;
};

#endif // DBOXDOWNLOAD_H
