#ifndef DGETVERSION_H
#define DGETVERSION_H

#include <QObject>
#include <QDir>
class DGetVersion : public QObject
{
    Q_OBJECT
public:
    explicit DGetVersion(QObject *parent = nullptr);

    ~DGetVersion();

    void getVersionInfo(quint64 id, QString device, QString version);
    void getLatestVersionInfo(quint64 id, QString device);
    void getLatestVersion(quint64 id, QString device);
    bool _getLatestVersion(quint64 id, QString device, QString &version);
    bool _devicefilter(quint64 id, QString device, QString version);
    bool _VersionFileInfo(quint64 id, QString device);
    QString getInfo(QString Ver);




private:
    QDir m_fileDir;
    QString m_fileName;
    QDir m_firmwareDir;
    QString m_firmwareName;
    QString m_md5;
    QString m_length;
    int m_CameraNum;

signals:
    void on_getVersionInfo_signal(QString res);
    void on_getVersion_signal(QString ver, QString length, QString md5, int cameraNum);
    void on_getVersionErr_signal(quint64 id, int code, QString message);
};

#endif // DGETVERSION_H
