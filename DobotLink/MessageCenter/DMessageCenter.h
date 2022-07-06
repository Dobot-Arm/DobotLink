#ifndef DMESSAGECENTER_H
#define DMESSAGECENTER_H

#include <QObject>
#include <QMap>
#include <QJsonObject>

#include "DPacket.h"

class DPluginManager;
class DNetworkManager;
class DWebSocketServer;
class DUpgrade;

class DMessageCenter : public QObject
{
    Q_OBJECT
public:
    static DMessageCenter *getInstance();

    void loadDefaultPlugin();
    void loadPlugin(QString pluginName);
    void unloadPlugin(QString pluginName);
    void sendMessageToPlugin(QString plugID, QJsonObject messageObj);

signals:
    void showTrayMessage_signal(QString title, QString message);
    void showDobotLinkWidget_signal(QString widgetName, QJsonObject params = QJsonObject());
    void onLoadPluginFinish_signal(QString name, bool isLoaded);
    void SwitchLanguage_signal(QString language);

public slots:
    void onClose_slot();

private:
    explicit DMessageCenter(QObject *parent = nullptr);

    DPluginManager *m_pluginManager;

    DNetworkManager *m_network;
    DWebSocketServer *m_websocketServer;

    DUpgrade *m_upgrade;
    QMap<quint64, DRequestPacket> m_handlingRequestMap;

    void handleWSMessageReceived(QJsonObject obj);
    void handleSelfMessage(const DRequestPacket &packet);
    QString getPluginName(QString device);

    void handleDownloadFinish(QString url, quint64 id);
    void handleLatestVersion(QJsonObject obj, quint64 id);

private slots:
    void _deleteChilden_slot();

//![PluginManager]
    void receivePluginMessage_slot(QString id, QJsonObject obj);

//![DWebSocket]
    void receiveWSMessage_slot(quint16 port, QString message);
    void wsClientClose_slot(quint16 port);

//![Network]
    void handleNetworkReplyMessage_slot(QString url, QJsonObject resObj, QString user);
    void handleNetworkReplyData_slot(QString url, QByteArray resBa);
};

#endif // DMESSAGECENTER_H
