#ifndef DNETWORKMANAGER_H
#define DNETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QDir>
#include <QFile>

class DNetworkManager : public QObject
{
    Q_OBJECT
public:
    static DNetworkManager *getInstance();

    void showIPAddress();

    void setDownloadFilePath(QString dir);
    QString getDownloadPath();

    void downloadFile(quint64 id, QJsonObject obj, QString user = QString());
    QNetworkReply *downloadFile(QString urlstr, QString fileName = QString(), quint64 id = 0);
    QNetworkReply *getDobotRequest(QString urlstr, QString user = QString(), int timeoutMs = 0);

    QNetworkReply *getRequest(QUrl url, int timeoutMs = 0);
    QNetworkReply *getRequest(QUrl url, QJsonObject queryObj, int timeoutMs = 0);
    QNetworkReply *postRequest(QString url, QJsonObject queryObj = QJsonObject(), int timeoutMs = 0);

signals:
    void onReplyMassage_signal(QString url, QJsonObject resObj, QString user = QString());
    void onReplyData_signal(QString url, QByteArray resBa);
    void downloadProgress_signal(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished_signal(QString url, quint64 id = 0);

private:
    explicit DNetworkManager(QObject *parent = nullptr);
    ~DNetworkManager();

    QNetworkAccessManager *m_manager;
    QMap<QNetworkReply *, QFile *> m_downloadReplyMap;
    QDir m_downloadPath;
    quint64 m_handleID;

    void setTimeoutRequest(QNetworkReply *reply, int msec = 3000);
    QJsonObject parseJsonData(QByteArray byteArray);

private slots:
    /* manager slot */
    void replyFinished_slot(QNetworkReply* reply);

    /* reply slot */
    void onReplyError_slot(QNetworkReply::NetworkError code);
#ifndef QT_NO_SSL
    void onSslErrors_slot(const QList<QSslError> &errors);
#endif
};

#endif // DNETWORKMANAGER_H
