#include "DMessageCenter.h"

#include <QJsonParseError>
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

#include "Module/DNetworkManager.h"
#include "Module/DWebSocketServer.h"
#include "Module/DLogger.h"
#include "Module/DSerialPort.h"
#include "Module/DSettings.h"
#include "Module/DNetworkManager.h"
#include "PluginManager/DPluginManager.h"

#include "Upgrade/DUpgrade.h"

DMessageCenter *DMessageCenter::getInstance()
{
    static DMessageCenter *instance = nullptr;
    if (instance == nullptr) {
        instance = new DMessageCenter();
    }
    return instance;
}

DMessageCenter::DMessageCenter(QObject *parent) : QObject(parent)
{
#ifdef QT_DEBUG
    qDebug() << __FUNCTION__ << "create";
#endif

    m_pluginManager = new DPluginManager(this);
    connect(m_pluginManager, &DPluginManager::receiveMessage_signal,
            this, &DMessageCenter::receivePluginMessage_slot);
    connect(m_pluginManager, SIGNAL(onLoadPluginFinish_signal(QString, bool)),
            this, SIGNAL(onLoadPluginFinish_signal(QString, bool)));

    m_network = DNetworkManager::getInstance();
    connect(m_network, &DNetworkManager::onReplyMassage_signal,
            this, &DMessageCenter::handleNetworkReplyMessage_slot);
    connect(m_network, &DNetworkManager::onReplyData_signal,
            this, &DMessageCenter::handleNetworkReplyData_slot);
    connect(m_network, &DNetworkManager::downloadFinished_signal,
            this, &DMessageCenter::handleDownloadFinish);

    m_websocketServer = DWebSocketServer::getInstance();
    connect(m_websocketServer, &DWebSocketServer::receiveMassage_signal,
            this, &DMessageCenter::receiveWSMessage_slot);
    connect(m_websocketServer, &DWebSocketServer::clientClose_signal,
            this, &DMessageCenter::wsClientClose_slot);

    m_upgrade = new DUpgrade(this);
    connect(m_upgrade, &DUpgrade::getNewVersionRes_signal,
            this, &DMessageCenter::handleLatestVersion);
}

void DMessageCenter::loadDefaultPlugin()
{
    m_pluginManager->loadDefaultPlugin();
}

void DMessageCenter::loadPlugin(QString pluginName)
{
    m_pluginManager->loadPlugin(pluginName);
    DSettings::getInstance()->setPluginLoaded(pluginName, true);
}

void DMessageCenter::unloadPlugin(QString pluginName)
{
    m_pluginManager->unloadPlugin(pluginName);
    DSettings::getInstance()->setPluginLoaded(pluginName, false);
}

void DMessageCenter::sendMessageToPlugin(QString plugID, QJsonObject messageObj)
{
    m_pluginManager->sendMassage(plugID, messageObj);
}

/**************************************** SLOT ****************************************/

/* 退出前处理 */
void DMessageCenter::onClose_slot()
{
    /* 关闭websocket */
    m_websocketServer->closeServer();

    /* 通知所有插件即将关闭 */
    QJsonObject obj;
    obj.insert("METHOD", "EXIT");
    m_pluginManager->sendMassage("ALL", obj);

    /* 关闭其他模块 */
    QTimer::singleShot(60, this, &DMessageCenter::_deleteChilden_slot);
}

void DMessageCenter::_deleteChilden_slot()
{
    qDebug() << "-- THE END --";

    m_pluginManager->deleteLater();

    /* 关闭日志 */
    DLogger::getInstance()->destroyLog_slot();
}

/* [收到 插件 消息] */
void DMessageCenter::receivePluginMessage_slot(QString id, QJsonObject obj)
{
//    qDebug() << "Center get message from:" << id << obj;

    if (!id.isEmpty()) {
        m_websocketServer->sendMessageObj(obj);
    }
}

/* [收到 WebSocket 消息] */
void DMessageCenter::receiveWSMessage_slot(quint16 port, QString message)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "Json Parse Error" << jsonError.errorString();
        return;
    }

    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        obj.insert("WSport", port);

        /* GET MESSAGE */
        handleWSMessageReceived(obj);
    }
}

/* ws Clients close slot */
void DMessageCenter::wsClientClose_slot(quint16 port)
{
    QJsonObject obj;
    obj.insert("METHOD", "CloseWebSocket");
    QJsonObject params;
    params.insert("WSport", port);
    obj.insert("params", params);
    m_pluginManager->sendMassage("ALL", obj);
}

void DMessageCenter::handleNetworkReplyMessage_slot(QString url, QJsonObject resObj, QString user)
{
    if (user != "DMessageCenter") {
        return;
    }

    if (url == "DobotLink/xx") {
        qDebug() << "center get " << resObj;
    }
}

void DMessageCenter::handleNetworkReplyData_slot(QString url, QByteArray resBa)
{
    if (url == "xxxx") {
        qDebug() << "center get " << resBa;
    }
}

void DMessageCenter::handleWSMessageReceived(QJsonObject obj)
{
    DRequestPacket packet;
    packet.setPacketFromObj(obj);

    if (packet.target == "api") {
        handleSelfMessage(packet);
    } else if (!packet.target.isEmpty()) {
        bool ok = m_pluginManager->sendMassage(getPluginName(packet.target), obj);
        if (ok == false) {
            DResultPacket resPacket(packet.id, packet.wsPort);
            QJsonObject resObj = resPacket.getErrorObjWithCode(ERROR_INVALID_PLUGIN);
            m_websocketServer->sendMessageObj(resObj);
        }
    } else {
        qDebug() << "can not handle message without dobotlink method:" << obj;

        DResultPacket resPacket(packet);
        QJsonObject resObj = resPacket.getErrorObjWithCode(ERROR_INVALID_METHOD);
        m_websocketServer->sendMessageObj(resObj);
    }
}

QString DMessageCenter::getPluginName(QString device)
{
    QStringList magicDeviceList;
    magicDeviceList << "Magician" << "MagicianLite" << "MagicBox" << "M1" << "MagicianGO";
    if (magicDeviceList.contains(device, Qt::CaseInsensitive)) {
        return "MagicDevice";
    }

    QStringList industrialDeviceList;
    industrialDeviceList << "CR" << "SA" << "SR" << "MagicianPro" << "M1Pro" << "MG400";
    if (industrialDeviceList.contains(device, Qt::CaseInsensitive)) {
        return "IndustrialRobot";
    }

    return device;
}

void DMessageCenter::handleSelfMessage(const DRequestPacket &packet)
{
    DResultPacket resPacket(packet);

    if (packet.api == "ShowMessage") {

        /* Dobotlink显示消息 */
        QString title = packet.getParamValue("title").toString();
        QString message = packet.getParamValue("message").toString();
        emit showTrayMessage_signal(title, message);

        QJsonObject resObj = resPacket.getResultObj();
        m_websocketServer->sendMessageObj(resObj);
    } else if (packet.api == "ShowAvailablePorts") {

        /* 检索可用串口设备 */
        QJsonObject resObj = resPacket.getResultObj(DSerialPort::getAvailablePortInfo());
        m_websocketServer->sendMessageObj(resObj);
    } else if (packet.api == "CloseDobotLink") {

        /* 关闭DobotLink */
        QJsonObject resObj = resPacket.getResultObj();
        m_websocketServer->sendMessageObj(resObj);
        qDebug() << "DobotLinkMain closing...";
        QTimer::singleShot(30, this, &DMessageCenter::onClose_slot);
        QTimer::singleShot(200, qApp, &QCoreApplication::quit);
    } else if (packet.api == "ShowSerialPortTool") {

        /* 显示串口调试助手 */
        QJsonObject resObj = resPacket.getResultObj();
        m_websocketServer->sendMessageObj(resObj);
        emit showDobotLinkWidget_signal("SerialPortTool", packet.paramsObj);
    } else if (packet.api == "ShowHelpMenu") {

        /* 显示串口调试助手 */
        QJsonObject resObj = resPacket.getResultObj();
        m_websocketServer->sendMessageObj(resObj);
        emit showDobotLinkWidget_signal("HelpMenu", packet.paramsObj);
    } else if (packet.api == "ShowCommMonitor") {

        /* 显示通讯监控器 */
        QJsonObject resObj = resPacket.getResultObj();
        m_websocketServer->sendMessageObj(resObj);
        emit showDobotLinkWidget_signal("CommMonitor", packet.paramsObj);
    } else if (packet.api == "ShowSystemTray") {

        /* 显示桌面托盘 */
        QJsonObject resObj = resPacket.getResultObj();
        m_websocketServer->sendMessageObj(resObj);
        emit showDobotLinkWidget_signal("SystemTray", packet.paramsObj);
    } else if (packet.api == "DownloadFile") {

        /* 下载 */
        m_handlingRequestMap.insert(packet.id, packet);
        m_network->downloadFile(packet.id, packet.paramsObj, "DMessageCenter");
    } else if (packet.api == "GetPluginList") {

        /* 获取已加载插件 */
        QJsonObject infoObj = m_pluginManager->getLoadedPluginInfo();
        QJsonObject resObj = resPacket.getResultObj(infoObj);
        m_websocketServer->sendMessageObj(resObj);
    } else if (packet.api == "GetVersion") {

        /* 获取本地版本 */
        QJsonObject versionObj = m_upgrade->getLocalVersion();
        QJsonObject resObj = resPacket.getResultObj(versionObj);
        m_websocketServer->sendMessageObj(resObj);
    } else if (packet.api == "GetLatestVersion") {

        /* 获取最新版本 */
        m_handlingRequestMap.insert(packet.id, packet);
        m_upgrade->checkNewVersion("DMessageCenter", packet.id);
    } else if (packet.api == "SetLogger") {

        /* 设置日志输出 */
        bool on = packet.paramsObj.value("on").toBool();
        if (on) {
            DLogger::getInstance()->startLogging();
            DSettings::getInstance()->setIsLogging(true);
        } else {
            DLogger::getInstance()->stopLogging();
            DSettings::getInstance()->setIsLogging(false);
        }

        QJsonObject resObj = resPacket.getResultObj();
        m_websocketServer->sendMessageObj(resObj);
    } else if (packet.api == "CleanLogs") {

        /* 清空日志文件夹 */
        DLogger::getInstance()->cleanLogs();

        QJsonObject resObj = resPacket.getResultObj();
        m_websocketServer->sendMessageObj(resObj);
    }  else if (packet.api == "GetLog") {

        /* 获取当前日志文件 */
        QByteArray data = DLogger::getInstance()->getLog();
        QJsonObject result;
        result.insert("data", QString(data.toBase64()));

        QJsonObject resObj = resPacket.getResultObj(result);
        m_websocketServer->sendMessageObj(resObj);
    } else if (packet.api == "SwitchLanguage") {
        /* 切换语言*/
        QString language = packet.paramsObj.value("language").toString();
        emit SwitchLanguage_signal(language);
        QJsonObject resObj = resPacket.getResultObj();
        this->setProperty("language",language);
        m_websocketServer->sendMessageObj(resObj);

    } else {
        qDebug() << "Dobotlink can not handle this method.";

        DResultPacket resPacket(packet.id, packet.wsPort);
        QJsonObject resObj = resPacket.getErrorObjWithCode(ERROR_INVALID_METHOD);
        m_websocketServer->sendMessageObj(resObj);
    }
}

void DMessageCenter::handleDownloadFinish(QString url, quint64 id)
{
    Q_UNUSED(url)

    DRequestPacket packet = m_handlingRequestMap.take(id);
    DResultPacket resPacket(packet);
    QJsonObject resObj = resPacket.getResultObj();
    m_websocketServer->sendMessageObj(resObj);
}

void DMessageCenter::handleLatestVersion(QJsonObject obj, quint64 id)
{
    DRequestPacket packet = m_handlingRequestMap.take(id);
    DResultPacket resPacket(packet);
    QJsonObject resObj = resPacket.getResultObj(obj);
    m_websocketServer->sendMessageObj(resObj);
}



