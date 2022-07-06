#ifndef DUPGRADE_H
#define DUPGRADE_H

#include <QObject>
#include <QJsonObject>

class DNetworkManager;
class DSettings;
class DUpgrade : public QObject
{
    Q_OBJECT
public:
    explicit DUpgrade(QObject *parent = nullptr);

    void checkNewVersion(QString user = QString(), quint64 id = 0);
    int compareVersion(QString v1, QString v2);
    QJsonObject getLocalVersion();

signals:
    void getNewVersionRes_signal(QJsonObject obj, quint64 id);

private:
    quint64 m_handlingID;
    DNetworkManager *m_network;
    DSettings *settings;

private slots:
    void handleNetworkReplyMessage_slot(QString url, QJsonObject resObj, QString user);
    void handleNetworkReplyData_slot(QString url, QByteArray resBa);
};

#endif // DUPGRADE_H
