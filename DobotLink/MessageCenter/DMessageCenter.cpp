#include "DMessageCenter.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <tlhelp32.h>
#endif

#include <QJsonParseError>
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QSharedMemory>
#include <QThread>

#include "Module/DNetworkManager.h"
#include "Module/DWebSocketServer.h"
#include "Module/DLogger.h"
#include "Module/DSerialPort.h"
#include "Module/DSettings.h"
#include "Module/DNetworkManager.h"
#include "PluginManager/DPluginManager.h"

#include "Upgrade/DUpgrade.h"

extern QSharedMemory g_sharedMemoryMain;

QString DMessageCenter::m_strProcessName;
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

    m_pTimerCheckKillProcess = new QTimer(this);
    connect(m_pTimerCheckKillProcess, &QTimer::timeout, this, &DMessageCenter::slotCheckKillProcess);

    connect(this, &DMessageCenter::signalClose, this, &DMessageCenter::onClose_slot);
    connect(this, &DMessageCenter::signal_loadPlugin, this, &DMessageCenter::loadPlugin);
    connect(this, &DMessageCenter::signal_unloadPlugin, this, &DMessageCenter::unloadPlugin);

    auto pThread = new QThread();
    connect(pThread, &QThread::finished, pThread, &QThread::deleteLater);
    this->moveToThread(pThread);
    m_pluginManager->moveToThread(pThread);
    m_network->moveToThread(pThread);
    m_websocketServer->moveToThread(pThread);
    m_upgrade->moveToThread(pThread);
    m_pTimerCheckKillProcess->moveToThread(pThread);
    pThread->start();
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

    startCheckProcessAndMaybeKill();
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
        if (handleMsgFilter(obj)){
            return;
        }
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
        killAllSubProcess();
        /* 关闭DobotLink */
        QJsonObject resObj = resPacket.getResultObj();
        m_websocketServer->sendMessageObj(resObj);
        qDebug() << "DobotLinkMain closing...CloseDobotLink";
        detroySharedMemory();
        QTimer::singleShot(30, this, &DMessageCenter::onClose_slot);
        QTimer::singleShot(200, qApp, &QCoreApplication::quit);
        std::thread thd([]{
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
            abort();
        });
        thd.detach();
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
    }  else if (packet.api == "OpenLogFile") {
        DLogger::getInstance()->openLogFile();
        QJsonObject resObj = resPacket.getResultObj();
        m_websocketServer->sendMessageObj(resObj);
    }  else if (packet.api == "OpenLogDir") {
        DLogger::getInstance()->openLogDir();
        QJsonObject resObj = resPacket.getResultObj();
        m_websocketServer->sendMessageObj(resObj);
    } else if (packet.api == "GetLog") {

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

    } else if (packet.api == "SetProcessName"){
        m_strProcessName = packet.getParamValue("name").toString();
        QJsonObject resObj = resPacket.getResultObj();
        m_websocketServer->sendMessageObj(resObj);
    }else {
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

void DMessageCenter::killAllSubProcess()
{
#ifdef Q_OS_WIN
    QString strCmd("taskkill");
    QStringList args;
    args<<"/f"<<"/t"
        <<"/im"<<"PyImageOM.exe"
        <<"/im"<<"MultiFuncTool.exe"
        <<"/im"<<"Mcuisp.exe"
        <<"/im"<<"kflash.exe"
        <<"/im"<<"dfucmd.exe"
        <<"/im"<<"debugger.exe"
        <<"/im"<<"CodingAgent.exe";
    QProcess::startDetached(strCmd, args);
#endif
}

QSet<QString> DMessageCenter::getAllProcessName()
{
    const qint64 procId = QCoreApplication::applicationPid();
    QSet<QString> allName;
#ifdef Q_OS_WIN
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot)
    {
        return allName;
    }
    PROCESSENTRY32W pe32;
    memset(&pe32, 0, sizeof(PROCESSENTRY32W));
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    Process32FirstW(hSnapshot, &pe32);
    while (Process32NextW(hSnapshot, &pe32))
    {
        if (procId != pe32.th32ProcessID)
        {
            QString str = QString::fromWCharArray(pe32.szExeFile);
            allName.insert(str);
        }
    }
    CloseHandle(hSnapshot);
#endif
    return allName;
}

void DMessageCenter::startCheckProcessAndMaybeKill()
{
    if (m_strProcessName.isEmpty()) return ;
    m_pTimerCheckKillProcess->start(500);
}

void DMessageCenter::slotCheckKillProcess()
{
    bool bFind = false;
    QSet<QString> allProcess = getAllProcessName();
    for(auto itr=allProcess.begin(); itr!=allProcess.end(); ++itr)
    {
        if (0 == itr->compare(m_strProcessName, Qt::CaseInsensitive))
        {
            bFind = true;
            break ;
        }
    }
    if (!bFind)
    {
        m_pTimerCheckKillProcess->stop();

        killAllSubProcess();

        qDebug() << "DobotLinkMain closing...slotCheckKillProcess";
        detroySharedMemory();
        QTimer::singleShot(30, this, &DMessageCenter::onClose_slot);
        QTimer::singleShot(200, qApp, &QCoreApplication::quit);
    }
    return ;
}

void DMessageCenter::detroySharedMemory()
{
    /* 关闭websocket */
    m_websocketServer->closeServer();

    g_sharedMemoryMain.setKey(QString());//清空
}

bool DMessageCenter::handleMsgFilter(QJsonObject obj)
{
#ifndef Q_OS_MAC
    Q_UNUSED(obj)
    return false;
#endif
    DRequestPacket packet;
    packet.setPacketFromObj(obj);
    if (packet.method.compare("dobotlink.MagicBox.DownloadProgram",Qt::CaseInsensitive)!=0){
        return false;
    }
    QJsonObject params = packet.paramsObj;
    QString strPort = params.value("portName").toString(); //COM4
    QString strCode = params.value("code").toString(); //base64
    QString strFile = params.value("fileName").toString(); //Draw/aa.py  Script/aa.py Playback/aa.py
    if (strPort.isEmpty() || strFile.isEmpty()){
        DResultPacket resPacket(packet);
        QJsonObject resObj = resPacket.getErrorObjWithCode(ERROR_INVALID_PARAMS);
        m_websocketServer->sendMessageObj(resObj);
        return true;
    }
    if (!strFile.contains('/')){
        //认为是在MagicBox的根目录创建一个文件
        emit signal_openCreateFileDlgOnMacOS(strFile, obj);
    } else if (strFile.startsWith("Draw/") || strFile.startsWith("Script/") || strFile.startsWith("Playback/")){
        //认为是在这些目录下创建文件
        emit signal_openCreateFileDlgOnMacOS(strFile, obj);
    } else{
        DResultPacket resPacket(packet);
        QJsonObject resObj = resPacket.getErrorObjWithCode(ERROR_INVALID_PARAMS);
        m_websocketServer->sendMessageObj(resObj);
        return true;
    }
}

void DMessageCenter::slot_onOpenCreateFileDlgOnMacOSResponse(QJsonObject obj, int errorCode)
{
    DRequestPacket packet;
    packet.setPacketFromObj(obj);
    DResultPacket resPacket(packet);

    if (errorCode==NOERROR){
        m_websocketServer->sendMessageObj(resPacket.getResultObj());
    }else{
        m_websocketServer->sendMessageObj(resPacket.getErrorObjWithCode(errorCode));
    }
}
