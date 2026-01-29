#ifndef DMESSAGECENTER_H
#define DMESSAGECENTER_H

#include <QObject>
#include <QMap>
#include <QSet>
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
    static void killAllSubProcess();
    static QSet<QString> getAllProcessName(); //only use windows

    void loadDefaultPlugin();

public slots:
    void slot_onOpenCreateFileDlgOnMacOSResponse(QJsonObject obj, int errorCode);

signals:
    void showTrayMessage_signal(QString title, QString message);
    void showDobotLinkWidget_signal(QString widgetName, QJsonObject params = QJsonObject());
    void onLoadPluginFinish_signal(QString name, bool isLoaded);
    void SwitchLanguage_signal(QString language);
    void signalClose();
    void signal_loadPlugin(QString pluginName);
    void signal_unloadPlugin(QString pluginName);

    //在MacOS打开文件选择并创建文件
    void signal_openCreateFileDlgOnMacOS(QString strFile, QJsonObject obj);

private slots:
    void onClose_slot();
    void loadPlugin(QString pluginName);
    void unloadPlugin(QString pluginName);

private:
    explicit DMessageCenter(QObject *parent = nullptr);
    void sendMessageToPlugin(QString plugID, QJsonObject messageObj);

    DPluginManager *m_pluginManager;

    DNetworkManager *m_network;
    DWebSocketServer *m_websocketServer;

    DUpgrade *m_upgrade;
    QMap<quint64, DRequestPacket> m_handlingRequestMap;

    /*
     * DobotLink的外壳程序意外强制退出，导致DobotLink本体程序无法接收到退出消息，
     * 从而使得一些进程（比如PyImageOM.exe）无法正常退出。
     * 为了能让这些进程都退出，前端连接ws后就发送dobotlink.api.SetProcessName接口，将它的进程名告知dobotlink，
     * 然后本体DobotLink收到ws断开消息，就遍历进程，没有发现m_strProcessName，就自杀。
     * m_strProcessName默认为空，不做任何判断.
     * 只在Windows有效
    */
    static QString m_strProcessName;
    QTimer* m_pTimerCheckKillProcess;

    void startCheckProcessAndMaybeKill();

    void handleWSMessageReceived(QJsonObject obj);
    void handleSelfMessage(const DRequestPacket &packet);
    QString getPluginName(QString device);

    void handleDownloadFinish(QString url, quint64 id);
    void handleLatestVersion(QJsonObject obj, quint64 id);

    /*
     *当DobotLink收到要退出时，因为释放各种资源，导致DobotLink并不能立马退出，因为DobotLink设置了只能启动一个，
     *在DobotLink还未来的及完全退出期间，用户又立刻想要打开DobotLink，结果无法打开，外壳也随之关闭。
     *所以解决这个问题：在收到dobotlink退出时就删掉这个“启动一个”限制
    */
    void detroySharedMemory();

    //处理拦截过滤消息，主要是解决在Mac上读写MagicBox的权限问题（Mac上读写外部存储器要打开原生窗口）
    //返回true表示拦截，外部不要再处理
    bool handleMsgFilter(QJsonObject obj);

private slots:
    void _deleteChilden_slot();
    void slotCheckKillProcess();

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
