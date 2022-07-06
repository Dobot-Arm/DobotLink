#include "M1Controller.h"

#include <QJsonArray>
#include <QSerialPortInfo>
#include <QDebug>
#include "DobotDll.h"

DeviceInfo::DeviceInfo()
{
    id = -1;
    isConnected = false;
    status = "unknown";
}

M1Controller::M1Controller(QObject *parent) : QObject(parent)
{
    isCheckState = false;

    checkStateTimer = new QTimer(this);
    checkStateTimer->setInterval(300);
    connect(checkStateTimer, &QTimer::timeout, this, &M1Controller::handleCheckTimeout_slot);
    //    checkStateTimer->start();
}

bool M1Controller::isDeviceConnected(QString portName)
{    
    if (m_DevInfoMap.contains(portName)) {
        DeviceInfo info = m_DevInfoMap.value(portName);
        if (info.isConnected) {
            return true;
        }
        qDebug() << "M1 status:" << info.status;
    }
    qDebug() << "! M1 is not connected. portName:" << portName;
    return false;
}

/* 心跳检测判断设备连接状态 */
void M1Controller::updataDeviceState()
{
    QStringList serialPortList;
    foreach (const auto &portInfo, QSerialPortInfo::availablePorts()) {
        serialPortList.append(portInfo.portName());
    }

    isCheckState = false;
    foreach (auto deviceInfo, m_DevInfoMap.values()) {
        if (deviceInfo.isConnected) {
            isCheckState = true;
            if (!serialPortList.contains(deviceInfo.portName)) {
                qDebug() << "M1 device:" << deviceInfo.portName << "lost connection.";

                deviceInfo.isConnected = false;
                deviceInfo.status = "unConnected";
                m_DevInfoMap.insert(deviceInfo.portName, deviceInfo);
                pDisConnectDobot(deviceInfo.portName);

                quint16 wsPort = m_portNameClientMap.value(deviceInfo.portName);
                emit deviceDisconnected_signal(deviceInfo.portName, wsPort);
            }
        }
    }
}

void M1Controller::disConnectDevices(quint16 port)
{
    QStringList portNameList = m_portNameClientMap.keys(port);
    foreach (const QString &portName, portNameList) {
        pDisConnectDobot(portName);
    }
}

QJsonObject M1Controller::getCmdResultObj(const M1Packet &packet, int res, QJsonValue data)
{
    M1ResPacket resPacket(packet.id, packet.port);

    if (res == DobotCommunicate_NoError) {
        if (data.isNull()) {
            resPacket.setResultObj(true);
        } else {
            resPacket.setResultObj(data);
        }
    } else {
        if (res == DobotCommunicate_BufferFull) {
            resPacket.setErrorObj(1, "Communicate Buffer Full.");
        } else if (res == DobotCommunicate_Timeout) {
            resPacket.setErrorObj(2, "Communicate Timeout.");
        } else if (res == DobotCommunicate_InvalidParams) {
            resPacket.setErrorObj(3, "Communicate Invalid Params.");
        } else if (res == DobotCommunicate_IsRuning) {
            resPacket.setErrorObj(4, "Communicate is running.");
        } else {
            resPacket.setErrorObj(20, "DobotCommunicate Unknow error.");
        }
    }
    return resPacket.getResultObj();
}

QJsonObject M1Controller::getConnectResultObj(const M1Packet &packet, const int res)
{
    M1ResPacket resPacket(packet.id, packet.port);

    if (res == DobotConnect_NoError) {
        resPacket.setResultObj(true);
    } else if (res == DobotConnect_NotFound) {
        resPacket.setErrorObj(10, "DobotConnect NotFound.");
    } else if (res == DobotConnect_Occupied) {
        resPacket.setErrorObj(11, "DobotConnect Occupied.");
    } else if (res == DobotConnect_AllReadyConnected) {
        resPacket.setErrorObj(12, "Magician is Already Connected.");
    } else if (res == DobotConnect_Unknown) {
        resPacket.setErrorObj(13, "DobotConnect Unknown error.");
    }

    return resPacket.getResultObj();
}

QJsonObject M1Controller::pSetCmdTimeout(const M1Packet &packet)
{
    qDebug()<<"pSetCmdTimeout";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    quint32 cmdTimeout = static_cast<quint32>(packet.paramsObj.value("cmdTimeout").toInt());

    int res = SetCmdTimeout(cmdTimeout);
    return getCmdResultObj(packet, res);
}

/* 搜索 */
QJsonObject M1Controller::pSearchDobot(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    QJsonObject resObj;
    QJsonArray deviceArray;
    char *devsChr = new char[2000]();
    int devCount = SearchDobot(devsChr, 1024);
    if (devCount <= 0) {
        M1ResPacket resPacket(packet.id, packet.port);
        resPacket.setErrorObj(81, "No Dobot was found :( ");
        return resPacket.getResultObj();
    }
    QString devsStr(devsChr);
    QStringList devList = devsStr.split(' ', QString::SkipEmptyParts);
    delete[] devsChr;
    for (int i=0; i < devList.count(); i++) {
        QString portName = devList.at(i);
        DeviceInfo info = m_DevInfoMap.value(portName);

//        if (info.portName.isEmpty() || info.isConnected == false) {
//            int res = ConnectDobot(portName.toLatin1().data(), 115200, nullptr, nullptr);
//            if (res == DobotConnect_NoError) {
//                //![get info]
//                char deviceName[64] = {0};
//                GetDeviceName(deviceName, sizeof(deviceName));
//                QString name(deviceName);
//                info.name = name;

//                char *snchar = new char[100];
//                int sn = GetDeviceSN(snchar, 100);
//                info.SN = QString::number(sn);

//                DisconnectDobot();
//                //![get info]
//                info.isConnected = false;
//                info.status = "unConnected";
//            } else if (res == DobotConnect_Occupied) {
//                info.status = "Occupied";
//            }

//            info.portName = portName;
//            m_DevInfoMap.insert(portName, info);
//        }

        if (info.isConnected) {
            info.status = "Connected";
        }
        else {
            info.status = "unConnected";
        }

        QMap<QString, QString> portInfoMap;
        foreach(const QSerialPortInfo &portInfo, QSerialPortInfo::availablePorts()) {
            QString portName = portInfo.portName();
            QString description = portInfo.description();

            portInfoMap.insert(portName, description);
        }

        QJsonObject deviceInfoObj;
        deviceInfoObj.insert("description", portInfoMap.value(portName));
        deviceInfoObj.insert("portName", portName);
//        deviceInfoObj.insert("name", info.name);
//        deviceInfoObj.insert("SN", info.SN);
        deviceInfoObj.insert("status", info.status);
        deviceArray.append(deviceInfoObj);
    }
    resObj.insert("array", deviceArray);

    return getCmdResultObj(packet, 0, deviceArray);
}

/* 连接 */
QJsonObject M1Controller::pConnectDobot(const M1Packet &packet)
{
    qDebug() << "pConnectDobot " << packet.portName;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    int res = ConnectDobot(packet.portName.toLatin1().data(), 115200, nullptr, nullptr);
    if (res == DobotConnect_NoError) {

        info.portName = packet.portName;
        info.isConnected = true;
        m_DevInfoMap.insert(packet.portName, info);

        m_portNameClientMap.insert(info.portName, packet.port);

        isCheckState = true;    /* start heart-beat test */

        if (packet.paramsObj.value("QueueStart").toBool(true)) {
            SetQueuedCmdStartExec();
        }

        qDebug() << "M1 connected:" << packet.portName;
    }

    return getConnectResultObj(packet, res);
}

/* 1.3.3 断开连接 */
QJsonObject M1Controller::pDisConnectDobot(const M1Packet &packet)
{
    qDebug()<<"pDisConnectDobot";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);
    if (info.isConnected == false) {
        qDebug() << "M1 already disconnected.";
        return getCmdResultObj(packet);
    }

    if (packet.paramsObj.value("QueueStop").toBool(true)) {
        SetQueuedCmdStopExec();
    }
    if (packet.paramsObj.value("QueueClear").toBool(true)) {
        SetQueuedCmdClear();
    }

    int res = DisconnectDobot();
    if (res == DobotConnect_NoError) {

        info.isConnected = false;
        info.status = "unConnected";
        m_DevInfoMap.insert(packet.portName, info);

        m_portNameClientMap.remove(packet.portName);

        qDebug() << "M1 disConnected:" << packet.portName;
    }

    /* 如果留有定时器，使它停止，并删除 */
    foreach (auto wait, m_waitFinishMap.keys()) {
        wait->stopWaiting();
        m_waitFinishMap.remove(wait);
        wait->deleteLater();
    }

    return getCmdResultObj(packet, res);
}

void M1Controller::pDisConnectDobot(const QString &portName)
{
    qDebug() << "pDisConnectDobot portName:" << portName;
    if (!m_DevInfoMap.contains(portName)) {
        qDebug() << "portName was not register";
        return;
    }
    DeviceInfo info = m_DevInfoMap.value(portName);

    if (info.isConnected == false) {
        qDebug() << "M1 already disconnected";
        return;
    }

    SetQueuedCmdStopExec();
    SetQueuedCmdClear();

    int res = DisconnectDobot();
    if (res == DobotConnect_NoError) {
        info.isConnected = false;
        info.status = "unConnected";
        m_DevInfoMap.insert(portName, info);
        m_portNameClientMap.remove(portName);

        qDebug() << "M1 disConnected:" << portName;
    }
}

/* 1.4 指令队列控制 */
QJsonObject M1Controller::pSetQueuedCmd(const M1Packet &packet)
{
    qDebug()<<"pSetQueuedCmd";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    int res = 0;
    QString type = packet.paramsObj.value("type").toString();
    if (type == "Start") {
        res = SetQueuedCmdStartExec();
    } else if (type == "Stop") {
        res = SetQueuedCmdStopExec();
    } else if (type == "ForceStop") {
        res = SetQueuedCmdForceStopExec();
    } else if (type == "Clear") {
        res = SetQueuedCmdClear();
    } else {
        res = DobotCommunicate_InvalidParams;
    }

    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pQueuedCmdStart(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    int res = SetQueuedCmdStartExec();
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pQueuedCmdStop(const M1Packet &packet)
{
    int res = SetQueuedCmdStopExec();
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pQueuedCmdClear(const M1Packet &packet)
{
    int res = SetQueuedCmdClear();
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pSetQueuedCmdForceStopExec(const M1Packet &packet)
{
    qDebug() << __FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    int res = SetQueuedCmdForceStopExec();
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pSetQueuedCmdStartDownload(const M1Packet &packet)
{
    qDebug() << __FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    uint32_t totalLoop = static_cast<uint32_t>(packet.paramsObj.value("totalLoop").toInt());
    uint32_t linePerLoop = static_cast<uint32_t>(packet.paramsObj.value("linePerLoop").toInt());

    int res = SetQueuedCmdStartDownload(totalLoop, linePerLoop);
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pSetQueuedCmdStopDownload(const M1Packet &packet)
{
    qDebug() << __FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);
    int res = SetQueuedCmdStopDownload();
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pGetQueuedCmdLeftSpace(const M1Packet &packet)
{
    qDebug() << __FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);
    QJsonObject paramsObj;
    uint64_t leftSpace;

    int res = GetQueuedCmdLeftSpace(&leftSpace);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("leftSpace", static_cast<double>(leftSpace));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pSetDeviceSN(const M1Packet &packet)
{
    qDebug() << __FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QString devSN = packet.paramsObj.value("deviceSN").toString();

    int res = SetDeviceSN(devSN.toLatin1().data());
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pGetDeviceSN(const M1Packet &packet)
{
    qDebug() << __FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    char devSN[64] = {0};

    int res = GetDeviceSN(devSN, sizeof(devSN));
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("deviceSN", devSN);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

/* 1.5 设备信息 */
//![1.5.3]
QJsonObject M1Controller::pSetDeviceName(const M1Packet &packet)
{
    qDebug()<<"pSetDeviceName";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QString devName = packet.paramsObj.value("deviceName").toString();

    int res = SetDeviceName(devName.toLatin1().data());
    return getCmdResultObj(packet, res);
}

//![1.5.4]
QJsonObject M1Controller::pGetDeviceName(const M1Packet &packet)
{
    qDebug()<<"pGetDeviceName";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    char deviceName[64] = {0};

    int res = GetDeviceName(deviceName, sizeof(deviceName));
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("deviceName", deviceName);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.5.6]
QJsonObject M1Controller::pSetDeviceWithL(const M1Packet &packet)
{
    qDebug()<<"pSetDeviceWithL";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    bool isEnable = packet.paramsObj.value("isEnable").toBool();

    int res = SetDeviceWithL(isEnable);
    return getCmdResultObj(packet, res);
}

//![1.5.7]
QJsonObject M1Controller::pGetDeviceWithL(const M1Packet &packet)
{
    qDebug()<<"pGetDeviceWithL";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    bool isEnable;

    int res = GetDeviceWithL(&isEnable);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("isEnable", isEnable);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pGetDeviceVersion(const M1Packet &packet)
{
    qDebug() << __FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    quint8 majorVersion = 0, minorVersion = 0, revision = 0;
    char A9_version[100] = {0};

    int typeIndex = packet.paramsObj.value("typeIndex").toInt();
    int res = GetDeviceVersion(static_cast<uint8_t>(typeIndex), &majorVersion, &minorVersion, &revision, A9_version);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("majorVersion", majorVersion);
        paramsObj.insert("minorVersion", minorVersion);
        paramsObj.insert("revision", revision);
        paramsObj.insert("A9_version", A9_version);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pGetHardwareVersion(const M1Packet &packet)
{
    qDebug() << __FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    HardwareInfo hwInfo;

    int res = GetHardwareVersion(&hwInfo);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("machineNum", hwInfo.machineNum);
        paramsObj.insert("mainBoard", hwInfo.mainBoard);
        paramsObj.insert("driverRearArm", hwInfo.driverRearArm);
        paramsObj.insert("driverFrontArm", hwInfo.driverFrontArm);
        paramsObj.insert("driverZArm", hwInfo.driverZArm);
        paramsObj.insert("driverRArm", hwInfo.driverRArm);
        paramsObj.insert("encoderRearArm", hwInfo.encoderRearArm);
        paramsObj.insert("encoderFrontArm", hwInfo.encoderFrontArm);
        paramsObj.insert("encoderZArm", hwInfo.encoderZArm);
        paramsObj.insert("encoderRArm", hwInfo.encoderRArm);
        paramsObj.insert("brakeBoard", hwInfo.brakeBoard);
        paramsObj.insert("endIOBoard", hwInfo.endIOBoard);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

/* 1.6 实时位姿 */
QJsonObject M1Controller::pGetPose(const M1Packet &packet)
{
    qDebug()<<"pGetPose";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    Pose poseParams;

    int res = GetPose(&poseParams);
    if (res == DobotCommunicate_NoError)
    {
        paramsObj.insert("x", static_cast<double>(poseParams.x));
        paramsObj.insert("y", static_cast<double>(poseParams.y));
        paramsObj.insert("z", static_cast<double>(poseParams.z));
        paramsObj.insert("r", static_cast<double>(poseParams.r));

#if 0
        QJsonObject jointAngleObj;
        jointAngleObj.insert("a0", static_cast<double>(poseParams.jointAngle[0]));
        jointAngleObj.insert("a1", static_cast<double>(poseParams.jointAngle[1]));
        jointAngleObj.insert("a2", static_cast<double>(poseParams.jointAngle[2]));
        jointAngleObj.insert("a3", static_cast<double>(poseParams.jointAngle[3]));
        paramsObj.insert("jointAngle", jointAngleObj);
#else
        QJsonArray jointAngleArr;
        for (int i=0; i<4; i++) {
            jointAngleArr.append(static_cast<double>(poseParams.jointAngle[i]));
        }
        paramsObj.insert("jointAngle", jointAngleArr);
#endif
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pResetPose(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    bool manual = packet.paramsObj.value("manualEnable").toBool(true);
    float rearArmAngle = static_cast<float>(packet.paramsObj.value("rearArmAngle").toDouble());
    float frontArmAngle = static_cast<float>(packet.paramsObj.value("frontArmAngle").toDouble());

    int res = ResetPose(manual, rearArmAngle, frontArmAngle);
    return getCmdResultObj(packet, res);
}

/* 1.7 报警功能 */
QJsonObject M1Controller::pGetAlarmsState(const M1Packet &packet)
{
    qDebug()<<"pGetAlarmsState";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    quint8 alarmsState[32];
    quint32 len;

    int res = GetAlarmsState(alarmsState, &len, 200);
    if (res == DobotCommunicate_NoError) {
        QJsonArray arr;
        for (uint i = 0; i < sizeof (alarmsState); i++) {
            arr.append(alarmsState[i]);
        }
        paramsObj.insert("state", arr);
        //        qDebug() << "alarmsState:" << arr.count();
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pClearAllAlarmsState(const M1Packet &packet)
{
    qDebug()<<"pClearAllAlarmsState";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    int res = ClearAllAlarmsState();
    return getCmdResultObj(packet, res);
}

/* 1.8 回零功能 */
QJsonObject M1Controller::pSetHOMECmd(const M1Packet &packet)
{
    qDebug()<<"pSetHOMECmd";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);
    quint64 queuedCmdIndex = 0;

    bool isWaitForFinish = packet.paramsObj.value("isWaitForFinish").toBool(true);
    quint8 isResetPars = static_cast<quint8>(packet.paramsObj.value("isResetPars").toInt());
    int timeOut = packet.paramsObj.value("syncTimeOut").toInt(90000);
    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);

    int res = SetHOMECmd(isResetPars, isQueued, &queuedCmdIndex);

    /* wait for finish */
    if (isWaitForFinish == true && res == DobotCommunicate_NoError && isQueued == true) {
        M1WaitForFinish *wait = new M1WaitForFinish(info.id, queuedCmdIndex, packet, this);
        wait->startWaiting(5000, timeOut);
        connect(wait, &M1WaitForFinish::finish_signal, this, &M1Controller::waitForFinish_slot);
        m_waitFinishMap.insert(wait, info.id);
        return QJsonObject();
    }
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pSetHOMEParams(const M1Packet &packet)
{
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    HOMEParams homeParams;
    quint64 queuedCmdIndex = 0;

    homeParams.x = static_cast<float>(packet.paramsObj.value("x").toDouble());
    homeParams.y = static_cast<float>(packet.paramsObj.value("y").toDouble());
    homeParams.z = static_cast<float>(packet.paramsObj.value("z").toDouble());
    homeParams.r = static_cast<float>(packet.paramsObj.value("r").toDouble());

    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);

    int res = SetHOMEParams(&homeParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pGetHOMEParams(const M1Packet &packet)
{
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    HOMEParams homeParams;

    int res = GetHOMEParams(&homeParams);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("x", static_cast<double>(homeParams.x));
        paramsObj.insert("y", static_cast<double>(homeParams.y));
        paramsObj.insert("z", static_cast<double>(homeParams.z));
        paramsObj.insert("r", static_cast<double>(homeParams.r));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

/* 1.9 HHT 手持示教器 (1.9.1~1.9.5) */
//![1.9.1]
QJsonObject M1Controller::pSetHHTTrigMode(const M1Packet &packet)
{
    qDebug()<<"pSetHHTTrigMode";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    HHTTrigMode hhtParams;

    int trigInt = packet.paramsObj.value("HHTTrigMode").toInt();
    hhtParams = static_cast<HHTTrigMode>(trigInt);

    int res = SetHHTTrigMode(hhtParams);
    return getCmdResultObj(packet, res);
}

//![1.9.2]
QJsonObject M1Controller::pGetHHTTrigMode(const M1Packet &packet)
{
    qDebug()<<"pGetHHTTrigMode";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    HHTTrigMode hhtTrigMode;

    int res = GetHHTTrigMode(&hhtTrigMode);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("HHTTrigMode", static_cast<int>(hhtTrigMode));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.9.3]
QJsonObject M1Controller::pSetHHTTrigOutputEnabled(const M1Packet &packet)
{
    qDebug()<<"pSetHHTTrigOutputEnabled";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    bool isEnabled = packet.paramsObj.value("isEnabled").toBool();
    int res = SetHHTTrigOutputEnabled(isEnabled);
    return getCmdResultObj(packet, res);
}

//![1.9.4]
QJsonObject M1Controller::pGetHHTTrigOutputEnabled(const M1Packet &packet)
{
    qDebug()<<"pGetHHTTrigOutputEnabled";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    bool isEnabled;

    int res = GetHHTTrigOutputEnabled(&isEnabled);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("isEnabled", isEnabled);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.9.5]
QJsonObject M1Controller::pGetHHTTrigOutput(const M1Packet &packet)
{
    qDebug()<<"pGetHHTTrigOutput";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    bool isTriggered;

    int res = GetHHTTrigOutput(&isTriggered);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("isTriggered", isTriggered);
    }
    return getCmdResultObj(packet, res, paramsObj);
}


/* 1.10 末端执行器 (1.10.1 ~ 1.10.8) */
//![1.10.1]
QJsonObject M1Controller::pSetEndEffectorParams(const M1Packet &packet)
{
    qDebug()<<"pSetEndEffectorParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    EndEffectorParams effectorParams;
    quint64 queuedCmdIndex = 0;

    effectorParams.xBias = static_cast<float>(packet.paramsObj.value("xOffset").toDouble());
    effectorParams.yBias = static_cast<float>(packet.paramsObj.value("yOffset").toDouble());
    effectorParams.zBias = static_cast<float>(packet.paramsObj.value("zOffset").toDouble());
    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);

    int res = SetEndEffectorParams(&effectorParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.10.2]
QJsonObject M1Controller::pGetEndEffectorParams(const M1Packet &packet)
{
    qDebug()<<"pGetEndEffectorParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    EndEffectorParams effectorParams;

    int res = GetEndEffectorParams(&effectorParams);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("xOffset", static_cast<double>(effectorParams.xBias));
        paramsObj.insert("yOffset", static_cast<double>(effectorParams.yBias));
        paramsObj.insert("zOffset", static_cast<double>(effectorParams.zBias));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.10.3]
QJsonObject M1Controller::pSetEndEffectorLaser(const M1Packet &packet)
{
    qDebug()<<"pSetEndEffectorLaser";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    bool enable = packet.paramsObj.value("enable").toBool();
    bool on = packet.paramsObj.value("on").toBool();
    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);
    quint64 queuedCmdIndex = 0;

    int res = SetEndEffectorLaser(enable, on, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.10.4]
QJsonObject M1Controller::pGetEndEffectorLaser(const M1Packet &packet)
{
    qDebug()<<"pGetEndEffectorLaser";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    bool enable, isOn;

    int res = GetEndEffectorLaser(&enable, &isOn);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("isEnabled", enable);
        paramsObj.insert("isOn", isOn);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.10.5]
QJsonObject M1Controller::pSetEndEffectorSuctionCup(const M1Packet &packet)
{
    qDebug()<<"pSetEndEffectorSuctionCup";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    bool enable = packet.paramsObj.value("enable").toBool();
    bool isOn = packet.paramsObj.value("on").toBool();
    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);
    quint64 queuedCmdIndex = 0;

    int res = SetEndEffectorSuctionCup(enable, isOn, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.10.6]
QJsonObject M1Controller::pGetEndEffectorSuctionCup(const M1Packet &packet)
{
    qDebug()<<"pGetEndEffectorSuctionCup";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    bool enable, isOn;

    int res = GetEndEffectorSuctionCup(&enable, &isOn);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("isEnabled", enable);
        paramsObj.insert("isOn", isOn);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.10.7]
QJsonObject M1Controller::pSetEndEffectorGripper(const M1Packet &packet)
{
    qDebug()<<"pSetEndEffectorGripper";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    bool enable = packet.paramsObj.value("enable").toBool();
    bool isOn = packet.paramsObj.value("on").toBool();
    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);
    quint64 queuedCmdIndex = 0;

    int res = SetEndEffectorSuctionCup(enable, isOn, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.10.8]
QJsonObject M1Controller::pGetEndEffectorGripper(const M1Packet &packet)
{
    qDebug()<<"pGetEndEffectorGripper";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    bool enable, isOn;

    int res = GetEndEffectorGripper(&enable, &isOn);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("isEnabled", enable);
        paramsObj.insert("isOn", isOn);
    }
    return getCmdResultObj(packet, res, paramsObj);
}


/* JOG 功能 (1.11.1 ~ 1.11.9) */
//![1.11.1]
QJsonObject M1Controller::pSetJOGJointParams(const M1Packet &packet)
{
    qDebug()<<"pSetJOGJointParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonArray velocityArr = packet.paramsObj.value("velocity").toArray();
    QJsonArray accelerationArr = packet.paramsObj.value("acceleration").toArray();
    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);

    JOGJointParams jogParams;
    quint64 queuedCmdIndex = 0;

    for (int i=0; i<velocityArr.count(); i++) {
        jogParams.velocity[i] = static_cast<float>(velocityArr.at(i).toInt());
    }
    for (int i=0; i<accelerationArr.count(); i++) {
        jogParams.acceleration[i] = static_cast<float>(accelerationArr.at(i).toInt());
    }

    int res = SetJOGJointParams(&jogParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.11.2]
QJsonObject M1Controller::pGetJOGJointParams(const M1Packet &packet)
{
    qDebug()<<"pGetJOGJointParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    JOGJointParams jogParams;

    int res = GetJOGJointParams(&jogParams);
    if (res == DobotCommunicate_NoError) {

        QJsonArray velocityArr, accelerationArr;
        for (int i=0; i<4; i++) {
            velocityArr.insert(i, static_cast<double>(jogParams.velocity[i]));
            accelerationArr.insert(i, static_cast<double>(jogParams.acceleration[i]));
        }
        paramsObj.insert("velocity", velocityArr);
        paramsObj.insert("acceleration", accelerationArr);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.11.3]
QJsonObject M1Controller::pSetJOGCoordinateParams(const M1Packet &packet)
{
    qDebug()<<"pSetJOGCoordinateParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonArray velocityArr = packet.paramsObj.value("velocity").toArray();
    QJsonArray accelerationArr = packet.paramsObj.value("acceleration").toArray();
    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);

    JOGCoordinateParams jogParams;
    quint64 queuedCmdIndex = 0;

    for (int i=0; i<velocityArr.count(); i++) {
        jogParams.velocity[i] = static_cast<float>(velocityArr.at(i).toInt());
    }
    for (int i=0; i<accelerationArr.count(); i++) {
        jogParams.acceleration[i] = static_cast<float>(accelerationArr.at(i).toInt());
    }

    int res = SetJOGCoordinateParams(&jogParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.11.4]
QJsonObject M1Controller::pGetJOGCoordinateParams(const M1Packet &packet)
{
    qDebug()<<"pGetJOGCoordinateParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    JOGCoordinateParams jogParams;

    int res = GetJOGCoordinateParams(&jogParams);
    if (res == DobotCommunicate_NoError) {

        QJsonArray velocityArr, accelerationArr;
        for (int i=0; i<4; i++) {
            velocityArr.insert(i, static_cast<double>(jogParams.velocity[i]));
            accelerationArr.insert(i, static_cast<double>(jogParams.acceleration[i]));
        }
        paramsObj.insert("velocity", velocityArr);
        paramsObj.insert("acceleration", accelerationArr);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.11.5]
QJsonObject M1Controller::pSetJOGLParams(const M1Packet &packet)
{
    qDebug()<<"pSetJOGLParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    JOGLParams jogParams;
    quint64 queuedCmdIndex = 0;

    jogParams.velocity = static_cast<float>(packet.paramsObj.value("velocity").toInt());
    jogParams.acceleration = static_cast<float>(packet.paramsObj.value("acceleration").toInt());
    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);

    int res = SetJOGLParams(&jogParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.11.6]
QJsonObject M1Controller::pGetJOGLParams(const M1Packet &packet)
{
    qDebug()<<"pGetJOGLParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    JOGLParams jogParams;

    int res = GetJOGLParams(&jogParams);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("velocity", static_cast<double>(jogParams.velocity));
        paramsObj.insert("acceleration", static_cast<double>(jogParams.acceleration));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.11.7]
QJsonObject M1Controller::pSetJOGCommonParams(const M1Packet &packet)
{
    qDebug()<<"pSetJOGCommonParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    JOGCommonParams jogParams;
    quint64 queuedCmdIndex = 0;

    jogParams.velocityRatio = static_cast<float>(packet.paramsObj.value("velocityRatio").toInt());
    jogParams.accelerationRatio = static_cast<float>(packet.paramsObj.value("accelerationRatio").toInt());
    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);

    int res = SetJOGCommonParams(&jogParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.11.8]
QJsonObject M1Controller::pGetJOGCommonParams(const M1Packet &packet)
{
    qDebug()<<"pGetJOGCommonParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    JOGCommonParams jogParams;

    int res = GetJOGCommonParams(&jogParams);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("velocityRatio", static_cast<double>(jogParams.velocityRatio));
        paramsObj.insert("accelerationRatio", static_cast<double>(jogParams.accelerationRatio));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.11.9]
QJsonObject M1Controller::pSetJOGCmd(const M1Packet &packet)
{
    qDebug()<<"pSetJOGCmd";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    quint64 queuedCmdIndex = 0;

    JOGCmd jogcmd;
    jogcmd.isJoint = static_cast<quint8>(packet.paramsObj.value("isJoint").toBool());
    jogcmd.cmd = static_cast<quint8>(packet.paramsObj.value("cmd").toInt());

    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);

    int res = SetJOGCmd(&jogcmd, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}


/* PTP 功能 (1.12.1 ~ 1.12.13) */
//![1.12.1]
QJsonObject M1Controller::pSetPTPJointParams(const M1Packet &packet)
{
    qDebug()<<"pSetPTPJointParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    PTPJointParams ptpParams;
    quint64 queuedCmdIndex = 0;

    QJsonArray velocityArr = packet.paramsObj.value("velocity").toArray();
    for (int i=0; i<4; i++) {
        ptpParams.velocity[i] = static_cast<float>(velocityArr.at(i).toDouble());
    }

    QJsonArray accelerationArr = packet.paramsObj.value("acceleration").toArray();
    for (int i=0; i<4; i++) {
        ptpParams.acceleration[i] = static_cast<float>(accelerationArr.at(i).toDouble());
    }
    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);
    int res = SetPTPJointParams(&ptpParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.12.2]
QJsonObject M1Controller::pGetPTPJointParams(const M1Packet &packet)
{
    qDebug()<<"pGetPTPJointParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    PTPJointParams ptpParams;

    int res = GetPTPJointParams(&ptpParams);
    if (res == DobotCommunicate_NoError)
    {
#if 0
        QJsonObject velocityObj;
        velocityObj.insert("v0", static_cast<double>(ptpParams.velocity[0]));
        velocityObj.insert("v1", static_cast<double>(ptpParams.velocity[1]));
        velocityObj.insert("v2", static_cast<double>(ptpParams.velocity[2]));
        velocityObj.insert("v3", static_cast<double>(ptpParams.velocity[3]));
        paramsObj.insert("velocity", velocityObj);

        QJsonObject accelerationObj;
        accelerationObj.insert("a0", static_cast<double>(ptpParams.acceleration[0]));
        accelerationObj.insert("a1", static_cast<double>(ptpParams.acceleration[1]));
        accelerationObj.insert("a2", static_cast<double>(ptpParams.acceleration[2]));
        accelerationObj.insert("a3", static_cast<double>(ptpParams.acceleration[3]));
        paramsObj.insert("acceleration", accelerationObj);
#else
        QJsonArray velocityArr;
        for (int i=0; i<4; i++) {
            velocityArr.append(static_cast<double>(ptpParams.velocity[i]));
        }
        paramsObj.insert("velocity", velocityArr);

        QJsonArray accelerationArr;
        for (int i=0; i < 4; i++) {
            accelerationArr.append(static_cast<double>(ptpParams.acceleration[i]));
        }
        paramsObj.insert("acceleration", accelerationArr);
#endif
    }

    return getCmdResultObj(packet, res, paramsObj);
}

//![1.12.3]
QJsonObject M1Controller::pSetPTPCoordinateParams(const M1Packet &packet)
{
    qDebug()<<"pSetPTPCoordinateParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    PTPCoordinateParams ptpParams;
    quint64 queuedCmdIndex = 0;

    ptpParams.xyzVelocity = static_cast<float>(packet.paramsObj.value("xyzVelocity").toDouble()); //X,Y,Z 3轴坐标轴速度
    ptpParams.rVelocity = static_cast<float>(packet.paramsObj.value("rVelocity").toDouble());     //末端 R 轴速度
    ptpParams.xyzAcceleration = static_cast<float>(packet.paramsObj.value("xyzAcceleration").toDouble()); //X,Y,Z 3轴坐标轴加速度
    ptpParams.rAcceleration = static_cast<float>(packet.paramsObj.value("rAcceleration").toDouble());     //末端 R 轴加速度
    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);

    int res = SetPTPCoordinateParams(&ptpParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.12.4]
QJsonObject M1Controller::pGetPTPCoordinateParams(const M1Packet &packet)
{
    qDebug()<<"pGetPTPCoordinateParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    PTPCoordinateParams ptpParams;

    int res = GetPTPCoordinateParams(&ptpParams);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("xyzVelocity", static_cast<double>(ptpParams.xyzVelocity));
        paramsObj.insert("rVelocity", static_cast<double>(ptpParams.rVelocity));
        paramsObj.insert("xyzAcceleration", static_cast<double>(ptpParams.xyzAcceleration));
        paramsObj.insert("rAcceleration", static_cast<double>(ptpParams.rAcceleration));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.12.5]
QJsonObject M1Controller::pSetPTPJumpParams(const M1Packet &packet)
{
    qDebug()<<"pSetPTPJumpParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    PTPJumpParams ptpParams;
    quint64 queuedCmdIndex = 0;

    ptpParams.jumpHeight = static_cast<float>(packet.paramsObj.value("jumpHeight").toDouble());   //抬升高度
    ptpParams.zLimit = static_cast<float>(packet.paramsObj.value("zLimit").toDouble());           //最大抬升高度
    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);

    int res = SetPTPJumpParams(&ptpParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.12.6]
QJsonObject M1Controller::pGetPTPJumpParams(const M1Packet &packet)
{
    qDebug()<<"pGetPTPJumpParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    PTPJumpParams ptpParams;

    int res = GetPTPJumpParams(&ptpParams);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("jumpHeight", static_cast<double>(ptpParams.jumpHeight));
        paramsObj.insert("zLimit", static_cast<double>(ptpParams.zLimit));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.12.7]
QJsonObject M1Controller::pSetPTPJump2Params(const M1Packet &packet)
{
    qDebug()<<"pSetPTPJump2Params";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    PTPJump2Params ptpParams;
    quint64 queuedCmdIndex = 0;

    ptpParams.startJumpHeight = static_cast<float>(packet.paramsObj.value("startJumpHeight").toDouble()); //起始点抬升高度
    ptpParams.endJumpHeight = static_cast<float>(packet.paramsObj.value("endJumpHeight").toDouble());     //结束点抬升高度
    ptpParams.zLimit = static_cast<float>(packet.paramsObj.value("zLimit").toDouble());                   //最大抬升高度
    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);

    int res = SetPTPJump2Params(&ptpParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.12.8]
QJsonObject M1Controller::pGetPTPJump2Params(const M1Packet &packet)
{
    qDebug()<<"pGetPTPJump2Params";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    PTPJump2Params ptpParams;

    int res = GetPTPJump2Params(&ptpParams);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("startJumpHeight", static_cast<double>(ptpParams.startJumpHeight));
        paramsObj.insert("endJumpHeight", static_cast<double>(ptpParams.endJumpHeight));
        paramsObj.insert("zLimit", static_cast<double>(ptpParams.zLimit));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

////![1.12.9]
//QJsonObject M1Controller::pSetPTPLParams(const M1Packet &packet)
//{
//    qDebug()<<"pSetPTPLParams";
//    DeviceInfo info = m_DevInfoMap.value(packet.portName);

//    PTPLParams ptpParams;
//    quint64 queuedCmdIndex = 0;

//    ptpParams.velocity = static_cast<float>(packet.paramsObj.value("velocity").toDouble());           //滑轨速度
//    ptpParams.acceleration = static_cast<float>(packet.paramsObj.value("acceleration").toDouble());   //滑轨加速度

//    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);

//    int res = SetPTPLParams(&ptpParams, isQueued, &queuedCmdIndex);
//    return getCmdResultObj(packet, res);
//}

////![1.12.10]
//QJsonObject M1Controller::pGetPTPLParams(const M1Packet &packet)
//{
//    qDebug()<<"pGetPTPLParams";
//    DeviceInfo info = m_DevInfoMap.value(packet.portName);

//    QJsonObject paramsObj;
//    PTPLParams ptpParams;

//    int res = GetPTPLParams(&ptpParams);
//    if (res == DobotCommunicate_NoError) {
//        paramsObj.insert("velocity", static_cast<double>(ptpParams.velocity));
//        paramsObj.insert("acceleration", static_cast<double>(ptpParams.acceleration));
//    }
//    return getCmdResultObj(packet, res, paramsObj);
//}

//![1.12.11]
QJsonObject M1Controller::pSetPTPCommonParams(const M1Packet &packet)
{
    qDebug()<<"pSetPTPCommonParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    PTPCommonParams ptpParams;
    quint64 queuedCmdIndex = 0;

    //    bool isWaitForFinish = packet.paramsObj.value("isWaitForFinish").toBool(true);
    ptpParams.velocityRatio = static_cast<float>(packet.paramsObj.value("velocityRatio").toDouble());
    ptpParams.accelerationRatio = static_cast<float>(packet.paramsObj.value("accelerationRatio").toDouble());
    //    int timeOut = packet.paramsObj.value("syncTimeOut").toInt(90000);

    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);

    int res = SetPTPCommonParams(&ptpParams, isQueued, &queuedCmdIndex);
    //    /* wait for finish */
    //    if (isWaitForFinish == true && res == DobotCommunicate_NoError && isQueued == true) {
    //        M1WaitForFinish *wait = new M1WaitForFinish(info.id, queuedCmdIndex, packet, this);
    //        wait->startWaiting(500, timeOut);
    //        connect(wait, &M1WaitForFinish::finish_signal, this, &M1Controller::waitForFinish_slot);
    //        m_waitFinishMap.insert(wait, info.id);
    //        return QJsonObject();
    //    }
    return getCmdResultObj(packet, res);
}

//![1.12.12]
QJsonObject M1Controller::pGetPTPCommonParams(const M1Packet &packet)
{
    qDebug()<<"pGetPTPCommonParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    PTPCommonParams ptpParams;

    int res = GetPTPCommonParams(&ptpParams);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("velocityRatio", static_cast<double>(ptpParams.velocityRatio));
        paramsObj.insert("accelerationRatio", static_cast<double>(ptpParams.accelerationRatio));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.12.13]
QJsonObject M1Controller::pSetPTPCmd(const M1Packet &packet)
{
    qDebug()<<"pSetPTPCmd";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    quint64 queuedCmdIndex = 0;

    PTPCmd ptpParams;
    ptpParams.ptpMode = static_cast<quint8>(packet.paramsObj.value("ptpMode").toInt());
    ptpParams.x = static_cast<float>(packet.paramsObj.value("x").toDouble());
    ptpParams.y = static_cast<float>(packet.paramsObj.value("y").toDouble());
    ptpParams.z = static_cast<float>(packet.paramsObj.value("z").toDouble());
    ptpParams.r = static_cast<float>(packet.paramsObj.value("r").toDouble());

    bool isWaitForFinish = packet.paramsObj.value("isWaitForFinish").toBool(true);
    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);
    int timeOut = packet.paramsObj.value("syncTimeOut").toInt(90000);

    int res = SetPTPCmd(&ptpParams, isQueued, &queuedCmdIndex);

    /* wait for finish */
    if (isWaitForFinish == true && res == DobotCommunicate_NoError && isQueued == true) {
        M1WaitForFinish *wait = new M1WaitForFinish(info.id, queuedCmdIndex, packet, this);
        wait->startWaiting(500, timeOut);
        connect(wait, &M1WaitForFinish::finish_signal, this, &M1Controller::waitForFinish_slot);
        m_waitFinishMap.insert(wait, info.id);
        return QJsonObject();
    }
    return getCmdResultObj(packet, res);
}

//![1.12.14]
QJsonObject M1Controller::pSetPTPPOCmd(const M1Packet &packet)
{
    qDebug()<<"pSetPTPPOCmd";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject PTPCmdObj;
    PTPCmd ptpParams;

    if (packet.paramsObj.contains("ptpCmd")) {
        PTPCmdObj = packet.paramsObj.value("ptpCmd").toObject();

        ptpParams.ptpMode = static_cast<quint8>(PTPCmdObj.value("ptpMode").toInt());
        ptpParams.x = static_cast<float>(PTPCmdObj.value("x").toDouble());
        ptpParams.y = static_cast<float>(PTPCmdObj.value("y").toDouble());
        ptpParams.z = static_cast<float>(PTPCmdObj.value("z").toDouble());
        ptpParams.r = static_cast<float>(PTPCmdObj.value("r").toDouble());
    }

    QJsonObject ParallelOutputCmdObj;
    ParallelOutputCmd outputCmd;

    if (packet.paramsObj.contains("poCmd")) {
        ParallelOutputCmdObj = packet.paramsObj.value("poCmd").toObject();

        outputCmd.ratio = static_cast<quint8>(ParallelOutputCmdObj.value("ratio").toInt());
        outputCmd.address = static_cast<quint16>(ParallelOutputCmdObj.value("address").toInt());
        outputCmd.level = static_cast<quint8>(ParallelOutputCmdObj.value("level").toInt());
    }

    int parallelCmdCount = packet.paramsObj.value("poCmdCount").toInt();
    quint64 queuedCmdIndex = 0;

    bool isWaitForFinish = packet.paramsObj.value("isWaitForFinish").toBool(true);
    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);
    int timeOut = packet.paramsObj.value("syncTimeOut").toInt(90000);

    int res = SetPTPPOCmd(&ptpParams, &outputCmd, parallelCmdCount, isQueued, &queuedCmdIndex);

    /* wait for finish */
    if (isWaitForFinish == true && res == DobotCommunicate_NoError && isQueued == true) {
        M1WaitForFinish *wait = new M1WaitForFinish(info.id, queuedCmdIndex, packet, this);
        wait->startWaiting(500, timeOut);
        connect(wait, &M1WaitForFinish::finish_signal, this, &M1Controller::waitForFinish_slot);
        m_waitFinishMap.insert(wait, info.id);
        return QJsonObject();
    }
    return getCmdResultObj(packet, res);
}

////![1.12.15]
//QJsonObject M1Controller::pSetPTPWithLCmd(const M1Packet &packet)
//{
//    qDebug()<<"pSetPTPWithLCmd";
//    DeviceInfo info = m_DevInfoMap.value(packet.portName);

//    quint64 queuedCmdIndex = 0;

//    PTPWithLCmd ptpParams;
//    ptpParams.ptpMode = static_cast<quint8>(packet.paramsObj.value("ptpMode").toInt());
//    ptpParams.x = static_cast<float>(packet.paramsObj.value("x").toDouble());
//    ptpParams.y = static_cast<float>(packet.paramsObj.value("y").toDouble());
//    ptpParams.z = static_cast<float>(packet.paramsObj.value("z").toDouble());
//    ptpParams.r = static_cast<float>(packet.paramsObj.value("r").toDouble());
//    ptpParams.l = static_cast<float>(packet.paramsObj.value("l").toDouble());

//    bool isWaitForFinish = packet.paramsObj.value("isWaitForFinish").toBool(true);
//    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);
//    int timeOut = packet.paramsObj.value("syncTimeOut").toInt(90000);

//    int res = SetPTPWithLCmd(&ptpParams, isQueued, &queuedCmdIndex);

//    /* wait for finish */
//    if (isWaitForFinish == true && res == DobotCommunicate_NoError && isQueued == true) {
//        M1WaitForFinish *wait = new M1WaitForFinish(info.id, queuedCmdIndex, packet, this);
//        wait->startWaiting(500, timeOut);
//        connect(wait, &M1WaitForFinish::finish_signal, this, &M1Controller::waitForFinish_slot);
//        m_waitFinishMap.insert(wait, info.id);
//        return QJsonObject();
//    }
//    return getCmdResultObj(packet, res);
//}

////![1.12.16]
//QJsonObject M1Controller::pSetPTPPOWithLCmd(const M1Packet &packet)
//{
//    qDebug()<<"pSetPTPPOWithLCmd";
//    DeviceInfo info = m_DevInfoMap.value(packet.portName);

//    QJsonObject PTPCmdObj;
//    PTPWithLCmd ptpParams;

//    if (packet.paramsObj.contains("PTPCmd")) {
//        PTPCmdObj = packet.paramsObj.value("PTPCmdObj").toObject();

//        ptpParams.ptpMode = static_cast<quint8>(PTPCmdObj.value("ptpMode").toInt());
//        ptpParams.x = static_cast<float>(PTPCmdObj.value("x").toDouble());
//        ptpParams.y = static_cast<float>(PTPCmdObj.value("y").toDouble());
//        ptpParams.z = static_cast<float>(PTPCmdObj.value("z").toDouble());
//        ptpParams.r = static_cast<float>(PTPCmdObj.value("r").toDouble());
//        ptpParams.l = static_cast<float>(PTPCmdObj.value("l").toDouble());
//    }

//    QJsonObject ParallelOutputCmdObj;
//    ParallelOutputCmd outputCmd;

//    if (packet.paramsObj.contains("ParallelOutputCmd")) {
//        ParallelOutputCmdObj = packet.paramsObj.value("ParallelOutputCmd").toObject();

//        outputCmd.ratio = static_cast<quint8>(ParallelOutputCmdObj.value("ratio").toInt());
//        outputCmd.address = static_cast<quint16>(ParallelOutputCmdObj.value("address").toInt());
//        outputCmd.level = static_cast<quint8>(ParallelOutputCmdObj.value("level").toInt());
//    }

//    quint64 queuedCmdIndex = 0;
//    int parallelCmdCount = packet.paramsObj.value("parallelCmdCount").toInt();
//    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);

//    bool isWaitForFinish = packet.paramsObj.value("isWaitForFinish").toBool(true);
//    int res = SetPTPPOWithLCmd(&ptpParams, &outputCmd, parallelCmdCount, isQueued, &queuedCmdIndex);
//    int timeOut = packet.paramsObj.value("syncTimeOut").toInt(90000);

//    /* wait for finish */
//    if (isWaitForFinish == true && res == DobotCommunicate_NoError && isQueued == true) {
//        M1WaitForFinish *wait = new M1WaitForFinish(info.id, queuedCmdIndex, packet, this);
//        wait->startWaiting(500, timeOut);
//        connect(wait, &M1WaitForFinish::finish_signal, this, &M1Controller::waitForFinish_slot);
//        m_waitFinishMap.insert(wait, info.id);
//        return QJsonObject();
//    }
//    return getCmdResultObj(packet, res);
//}


/* CP 功能 (1.13.1 ~ 1.13.4) */
//![1.13.1]
QJsonObject M1Controller::pSetCPParams(const M1Packet &packet)
{
    qDebug()<<"pSetCPParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    CPParams cpParams;
    quint64 queuedCmdIndex = 0;

    cpParams.planAcc = static_cast<float>(packet.paramsObj.value("targetAcc").toDouble());
    cpParams.junctionVel = static_cast<float>(packet.paramsObj.value("junctionVel").toDouble());
    cpParams.realTimeTrack = static_cast<quint8>(packet.paramsObj.value("isRealTimeTrack").toInt());
    if(cpParams.realTimeTrack == 1){
        cpParams.period = static_cast<float>(packet.paramsObj.value("period").toDouble());
    }else {
        cpParams.acc = static_cast<float>(packet.paramsObj.value("acc").toDouble());
    }

    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);

    int res = SetCPParams(&cpParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.13.2]
QJsonObject M1Controller::pGetCPParams(const M1Packet &packet)
{
    qDebug()<<"pGetCPParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    CPParams cpParams;

    int res = GetCPParams(&cpParams);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("targetAcc", static_cast<double>(cpParams.planAcc));
        paramsObj.insert("junctionVel", static_cast<double>(cpParams.junctionVel));
        paramsObj.insert("isRealTimeTrack", cpParams.realTimeTrack);
        if(cpParams.realTimeTrack == 1){
            paramsObj.insert("period", static_cast<double>(cpParams.period));
        }else {
            paramsObj.insert("acc", static_cast<double>(cpParams.acc));
        }
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.13.3]
QJsonObject M1Controller::pSetCPCmd(const M1Packet &packet)
{
    qDebug()<<"pSetCPCmd";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    quint64 queuedCmdIndex = 0;

    CPCmd cpParams;
    cpParams.cpMode = static_cast<quint8>(packet.paramsObj.value("cpMode").toInt());
    cpParams.x = static_cast<float>(packet.paramsObj.value("x").toDouble());
    cpParams.y = static_cast<float>(packet.paramsObj.value("y").toDouble());
    cpParams.z = static_cast<float>(packet.paramsObj.value("z").toDouble());
    cpParams.velocity = static_cast<float>(packet.paramsObj.value("velocity").toDouble());
    cpParams.power = static_cast<float>(packet.paramsObj.value("power").toDouble());

    bool isWaitForFinish = packet.paramsObj.value("isWaitForFinish").toBool(true);
    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);
    int timeOut = packet.paramsObj.value("syncTimeOut").toInt(90000);

    int res = SetCPCmd(&cpParams, isQueued, &queuedCmdIndex);

#if 1
    /* wait for finish */
    if (isWaitForFinish == true && res == DobotCommunicate_NoError && isQueued == true) {
        M1WaitForFinish *wait = new M1WaitForFinish(info.id, queuedCmdIndex, packet, this);
        wait->startWaiting(500, timeOut);
        connect(wait, &M1WaitForFinish::finish_signal, this, &M1Controller::waitForFinish_slot);
        m_waitFinishMap.insert(wait, info.id);
        return QJsonObject();
    }
#endif

    return getCmdResultObj(packet, res);
}

//![1.13.4]
QJsonObject M1Controller::pSetCPLECmd(const M1Packet &packet)
{
    qDebug()<<"pSetCPLECmd";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    quint64 queuedCmdIndex = 0;

    CPCmd cpParams;
    cpParams.cpMode = static_cast<quint8>(packet.paramsObj.value("cpMode").toInt());
    cpParams.x = static_cast<float>(packet.paramsObj.value("x").toDouble());
    cpParams.y = static_cast<float>(packet.paramsObj.value("y").toDouble());
    cpParams.z = static_cast<float>(packet.paramsObj.value("z").toDouble());
    cpParams.velocity = static_cast<float>(packet.paramsObj.value("velocity").toDouble());
    cpParams.power = static_cast<float>(packet.paramsObj.value("power").toDouble());

    bool isWaitForFinish = packet.paramsObj.value("isWaitForFinish").toBool(true);
    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);
    int timeOut = packet.paramsObj.value("syncTimeOut").toInt(90000);

    int res = SetCPLECmd(&cpParams, isQueued, &queuedCmdIndex);

#if 1
    /* wait for finish */
    if (isWaitForFinish == true && res == DobotCommunicate_NoError && isQueued == true) {
        M1WaitForFinish *wait = new M1WaitForFinish(info.id, queuedCmdIndex, packet, this);
        wait->startWaiting(10000, timeOut);
        connect(wait, &M1WaitForFinish::finish_signal, this, &M1Controller::waitForFinish_slot);
        m_waitFinishMap.insert(wait, info.id);
        return QJsonObject();
    }
#endif

    return getCmdResultObj(packet, res);
}

/* ARC 功能 (1.14.1 ~ 1.15.2) */
//![1.14.1]
QJsonObject M1Controller::pSetARCParams(const M1Packet &packet)
{
    qDebug()<<"pSetARCParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    ARCParams arcParams;
    quint64 queuedCmdIndex = 0;

    arcParams.xyzVelocity = static_cast<float>(packet.paramsObj.value("xyzVelocity").toDouble());
    arcParams.rVelocity = static_cast<float>(packet.paramsObj.value("rVelocity").toDouble());
    arcParams.xyzAcceleration = static_cast<float>(packet.paramsObj.value("xyzAcceleration").toDouble());
    arcParams.rAcceleration = static_cast<float>(packet.paramsObj.value("rAcceleration").toDouble());

    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);

    int res = SetARCParams(&arcParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.14.2]
QJsonObject M1Controller::pGetARCParams(const M1Packet &packet)
{
    qDebug()<<"pGetARCParams";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    ARCParams arcParams;

    int res = GetARCParams(&arcParams);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("xyzVelocity", static_cast<double>(arcParams.xyzVelocity));
        paramsObj.insert("rVelocity", static_cast<double>(arcParams.rVelocity));
        paramsObj.insert("xyzAcceleration", static_cast<double>(arcParams.xyzAcceleration));
        paramsObj.insert("rAcceleration", static_cast<double>(arcParams.rAcceleration));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.14.3]
QJsonObject M1Controller::pSetARCCmd(const M1Packet &packet)
{
    qDebug()<<"pSetARCCmd";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    quint64 queuedCmdIndex = 0;

    QJsonObject cirPoint = packet.paramsObj.value("cirPoint").toObject();
    QJsonObject toPoint = packet.paramsObj.value("toPoint").toObject();

    ARCCmd arcCmd;
    arcCmd.cirPoint.x = static_cast<float>(cirPoint.value("x").toDouble());
    arcCmd.cirPoint.y = static_cast<float>(cirPoint.value("y").toDouble());
    arcCmd.cirPoint.z = static_cast<float>(cirPoint.value("z").toDouble());
    arcCmd.cirPoint.r = static_cast<float>(cirPoint.value("r").toDouble());
    arcCmd.toPoint.x = static_cast<float>(toPoint.value("x").toDouble());
    arcCmd.toPoint.y = static_cast<float>(toPoint.value("y").toDouble());
    arcCmd.toPoint.z = static_cast<float>(toPoint.value("z").toDouble());
    arcCmd.toPoint.r = static_cast<float>(toPoint.value("r").toDouble());

    bool isWaitForFinish = packet.paramsObj.value("isWaitForFinish").toBool(true);
    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);
    int timeOut = packet.paramsObj.value("syncTimeOut").toInt(90000);

    int res = SetARCCmd(&arcCmd, isQueued, &queuedCmdIndex);

#if 1
    /* wait for finish */
    if (isWaitForFinish == true && res == DobotCommunicate_NoError && isQueued == true) {
        M1WaitForFinish *wait = new M1WaitForFinish(info.id, queuedCmdIndex, packet, this);
        wait->startWaiting(500, timeOut);
        connect(wait, &M1WaitForFinish::finish_signal, this, &M1Controller::waitForFinish_slot);
        m_waitFinishMap.insert(wait, info.id);
        return QJsonObject();
    }
#endif
    return getCmdResultObj(packet, res);
}

//![1.14.4]
QJsonObject M1Controller::pSetCircleCmd(const M1Packet &packet)
{
    qDebug()<<"pSetCircleCmd";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    quint64 queuedCmdIndex = 0;

    QJsonObject cirPoint = packet.paramsObj.value("cirPoint").toObject();
    QJsonObject toPoint = packet.paramsObj.value("toPoint").toObject();
    uint32_t count = static_cast<uint32_t>(packet.paramsObj.value("count").toInt());

    CircleCmd circleCmd;
    circleCmd.cirPoint.x = static_cast<float>(cirPoint.value("x").toDouble());
    circleCmd.cirPoint.y = static_cast<float>(cirPoint.value("y").toDouble());
    circleCmd.cirPoint.z = static_cast<float>(cirPoint.value("z").toDouble());
    circleCmd.cirPoint.r = static_cast<float>(cirPoint.value("r").toDouble());
    circleCmd.toPoint.x = static_cast<float>(toPoint.value("x").toDouble());
    circleCmd.toPoint.y = static_cast<float>(toPoint.value("y").toDouble());
    circleCmd.toPoint.z = static_cast<float>(toPoint.value("z").toDouble());
    circleCmd.toPoint.r = static_cast<float>(toPoint.value("r").toDouble());
    circleCmd.count = count;

    bool isWaitForFinish = packet.paramsObj.value("isWaitForFinish").toBool(true);
    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);
    int timeOut = packet.paramsObj.value("syncTimeOut").toInt(90000);


    int res = SetCircleCmd(&circleCmd, isQueued, &queuedCmdIndex);

#if 1
    /* wait for finish */
    if (isWaitForFinish == true && res == DobotCommunicate_NoError && isQueued == true) {
        M1WaitForFinish *wait = new M1WaitForFinish(info.id, queuedCmdIndex, packet, this);
        wait->startWaiting(500, timeOut);
        connect(wait, &M1WaitForFinish::finish_signal, this, &M1Controller::waitForFinish_slot);
        m_waitFinishMap.insert(wait, info.id);
        return QJsonObject();
    }
#endif
    return getCmdResultObj(packet, res);
}

/* WAIT 功能 (1.16.1 ~ 1.16.2) */
//![1.16.1]
QJsonObject M1Controller::pSetWAITCmd(const M1Packet &packet)
{
    qDebug()<<"pSetWAITCmd";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    WAITCmd waitParams;
    quint64 queuedCmdIndex = 0;

    waitParams.timeout = static_cast<quint32>(packet.paramsObj.value("timeout").toDouble());

    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);

    int res = SetWAITCmd(&waitParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.16.2]
QJsonObject M1Controller::pSetTRIGCmd(const M1Packet &packet)
{
    qDebug()<<"pSetTRIGCmd";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    TRIGCmd trigParams;
    quint64 queuedCmdIndex = 0;

    trigParams.address = static_cast<quint8>(packet.paramsObj.value("address").toInt());
    trigParams.mode = static_cast<quint8>(packet.paramsObj.value("mode").toInt());
    trigParams.condition = static_cast<quint8>(packet.paramsObj.value("condition").toInt());
    trigParams.threshold = static_cast<quint16>(packet.paramsObj.value("threshold").toInt());

    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);

    int res = SetTRIGCmd(&trigParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}


///* EIO 功能(1.17.1 ~ 1.17.10) */
////![1.17.1]
//QJsonObject M1Controller::pSetIOMultiplexing(const M1Packet &packet)
//{
//    qDebug()<<"pSetIOMultiplexing";
//    DeviceInfo info = m_DevInfoMap.value(packet.portName);

//    IOMultiplexing eioParams;
//    quint64 queuedCmdIndex = 0;

//    eioParams.address = static_cast<quint8>(packet.paramsObj.value("address").toInt());      //IO 地址，取值范围：1~20
//    eioParams.multiplex = static_cast<quint8>(packet.paramsObj.value("multiplex").toInt());  //IO 功能。取值范围：0~6

//    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);

//    int res = SetIOMultiplexing(&eioParams, isQueued, &queuedCmdIndex);
//    return getCmdResultObj(packet, res);
//}

////![1.17.2]
//QJsonObject M1Controller::pGetIOMultiplexing(const M1Packet &packet)
//{
//    qDebug()<<"pGetIOMultiplexing";
//    DeviceInfo info = m_DevInfoMap.value(packet.portName);

//    QJsonObject paramsObj;
//    IOMultiplexing eioParams;

//    int res = GetIOMultiplexing(&eioParams);
//    if (res == DobotCommunicate_NoError) {
//        paramsObj.insert("address", static_cast<double>(eioParams.address));
//        paramsObj.insert("multiplex", static_cast<double>(eioParams.multiplex));
//    }
//    return getCmdResultObj(packet, res, paramsObj);
//}

//![1.17.3]
QJsonObject M1Controller::pSetIODO(const M1Packet &packet)
{
    qDebug()<<"pSetIODO";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    IODO eioParams;
    quint64 queuedCmdIndex = 0;

    eioParams.address = static_cast<quint8>(packet.paramsObj.value("address").toInt());  //IO 地址
    eioParams.level = static_cast<quint8>(packet.paramsObj.value("level").toInt());      //输出电平 0：低 1：高

    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);

    int res = SetIODO(&eioParams, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

//![1.17.4]
QJsonObject M1Controller::pGetIODO(const M1Packet &packet)
{
    qDebug()<<"pGetIODO";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    IODO eioParams;
    eioParams.address = static_cast<quint8>(packet.paramsObj.value("address").toInt());  //IO 地址

    int res = GetIODO(&eioParams);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("address", eioParams.address);
        paramsObj.insert("level", eioParams.level);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.17.7]
QJsonObject M1Controller::pGetIODI(const M1Packet &packet)
{
    qDebug()<<"pGetIODI";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    IODI eioParams;
    eioParams.address = static_cast<quint8>(packet.paramsObj.value("address").toInt());  //IO 地址

    int res = GetIODI(&eioParams);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("address", eioParams.address);
        paramsObj.insert("level", eioParams.level);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

//![1.17.8]
QJsonObject M1Controller::pGetIOADC(const M1Packet &packet)
{
    qDebug()<<"pGetIOADC";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    IOADC eioParams;
    eioParams.address = static_cast<quint8>(packet.paramsObj.value("address").toInt());  //IO 地址

    int res = GetIOADC(&eioParams);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("address", eioParams.address);
        paramsObj.insert("value", eioParams.value);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pSetArmOrientation(const M1Packet &packet)
{
    qDebug()<<"pSetArmOrientation";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    ArmOrientation armOrientation;

    quint64 queuedCmdIndex = 0;

    armOrientation = static_cast<ArmOrientation>(packet.paramsObj.value("armOrientation").toInt());

    bool isQueued = packet.paramsObj.value("isQueued").toBool(false);

    int res = SetArmOrientation(armOrientation, isQueued, &queuedCmdIndex);
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pGetArmOrientation(const M1Packet &packet)
{
    qDebug()<<"pGetArmOrientation";
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    ArmOrientation  armOrientation ;

    int res = GetArmOrientation(&armOrientation);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("armOrientation", armOrientation);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pSetFirmwareLanConfig(const M1Packet &packet)
{
    qDebug() << __FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonArray addrArray = packet.paramsObj.value("addr").toArray();
    QJsonArray maskArray = packet.paramsObj.value("mask").toArray();
    QJsonArray gatewayArray = packet.paramsObj.value("gateway").toArray();
    QJsonArray dnsArray = packet.paramsObj.value("dns").toArray();

    LanConfig lanConfig;

    lanConfig.status = static_cast<uint8_t>(packet.paramsObj.value("status").toInt());
    lanConfig.dhcp = static_cast<uint8_t>(packet.paramsObj.value("dhcp").toInt());
    for (int i=0; i<addrArray.count(); i++) {
        lanConfig.addr[i] = static_cast<uint8_t>(addrArray.at(i).toInt());
    }
    for (int i=0; i<maskArray.count(); i++) {
        lanConfig.mask[i] = static_cast<uint8_t>(maskArray.at(i).toInt());
    }
    for (int i=0; i<gatewayArray.count(); i++) {
        lanConfig.gateway[i] = static_cast<uint8_t>(gatewayArray.at(i).toInt());
    }
    for (int i=0; i<dnsArray.count(); i++) {
        lanConfig.dns[i] = static_cast<uint8_t>(dnsArray.at(i).toInt());
    }

    int res = SetFirmwareLanConfig(&lanConfig);
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pGetFirmwareLanConfig(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    LanConfig  lanConfig ;

    int res = GetFirmwareLanConfig(&lanConfig);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("status", lanConfig.status);
        paramsObj.insert("dhcp", lanConfig.dhcp);
        paramsObj.insert("addr", (char*)lanConfig.addr);
        paramsObj.insert("mask", (char*)lanConfig.mask);
        paramsObj.insert("gateway", (char*)lanConfig.gateway);
        paramsObj.insert("dns", (char*)lanConfig.gateway);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pFirmwareReboot(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);
    QJsonValue paramsObj;
    int res = FirmwareReboot();
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pGetTrajectory(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    uint64_t index = static_cast<uint64_t>(packet.paramsObj.value("index").toInt());
    TrajectoryData  TrajectData[8] ;
    uint8_t count;
    uint8_t countMax = 8;

    int res = GetTrajectory(TrajectData, &count, countMax, index);
    QJsonArray TrajectDataArray;
    if (res == DobotCommunicate_NoError) {
        for (int i=0; i<count ; i++) {
            QJsonArray dataArray;
            dataArray.append(static_cast<double>(TrajectData[i].data1));
            dataArray.append(static_cast<double>(TrajectData[i].data2));
            dataArray.append(static_cast<double>(TrajectData[i].data3));
            dataArray.append(static_cast<double>(TrajectData[i].data4));
            TrajectDataArray.append(dataArray);
        }
    }
    paramsObj.insert("TrajectoryData", TrajectDataArray);
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pGetCollisionCount(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    CollisionCount count;

    int res = GetCollisionCount(&count);
    QJsonArray CollisionCountArray;
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("data1", count.data1);
        paramsObj.insert("data2", count.data2);
        paramsObj.insert("data3", count.data3);
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pSetSafeStrategy(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    uint8_t flag = static_cast<uint8_t>(packet.paramsObj.value("flag").toDouble());

    int res = SetSafeStrategy(flag);
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pGetSafeStrategy(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    uint8_t flag;
    int res = GetSafeStrategy(&flag);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("flag", static_cast<double>(flag));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pSetSafeGuardMode(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);
    uint8_t mode = static_cast<uint8_t>(packet.paramsObj.value("mode").toDouble());

    int res = SetSafeGuardMode(mode);
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pGetSafeGuardMode(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    uint8_t mode;
    int res = GetSafeGuardMode(&mode);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("mode", static_cast<double>(mode));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pSetSafeGuardStatus(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);
    uint8_t status = static_cast<uint8_t>(packet.paramsObj.value("status").toDouble());

    int res = SetSafeGuardStatus(status);
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pGetSafeGuardStatus(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    uint8_t status;
    int res = GetSafeGuardStatus(&status);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("status", static_cast<double>(status));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pGetQueuedCmdCurrentIndex(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    uint64_t index = static_cast<uint64_t>(packet.paramsObj.value("queuedCmdIndex").toInt());

    int res = GetQueuedCmdCurrentIndex(&index);
    if (res == DobotCommunicate_NoError) {
        paramsObj.insert("queuedCmdIndex", static_cast<double>(index));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pSetBasicDynamicParams(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    BasicDynamicParams basicDy;
    basicDy.FS1 = static_cast<float>(packet.paramsObj.value("FS1").toDouble());
    basicDy.FS2 = static_cast<float>(packet.paramsObj.value("FS2").toDouble());
    basicDy.FV1 = static_cast<float>(packet.paramsObj.value("FV1").toDouble());
    basicDy.FV2 = static_cast<float>(packet.paramsObj.value("FV2").toDouble());
    basicDy.IA2 = static_cast<float>(packet.paramsObj.value("IA2").toDouble());
    basicDy.MX2 = static_cast<float>(packet.paramsObj.value("MX2").toDouble());
    basicDy.MY2 = static_cast<float>(packet.paramsObj.value("MY2").toDouble());
    basicDy.ZZ1 = static_cast<float>(packet.paramsObj.value("ZZ1").toDouble());
    basicDy.ZZ2 = static_cast<float>(packet.paramsObj.value("ZZ2").toDouble());

    int res = SetBasicDynamicParams(&basicDy);
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pGetBasicDynamicParams(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    BasicDynamicParams basicDy;
    QJsonArray basicDyArray;

    int res = GetBasicDynamicParams(&basicDy);
    if (res == DobotCommunicate_NoError)
    {
        paramsObj.insert("FS1",static_cast<double>(basicDy.FS1));
        paramsObj.insert("FS2",static_cast<double>(basicDy.FS2));
        paramsObj.insert("FV1",static_cast<double>(basicDy.FV1));
        paramsObj.insert("FV2",static_cast<double>(basicDy.FV2));
        paramsObj.insert("IA2",static_cast<double>(basicDy.IA2));
        paramsObj.insert("MX2",static_cast<double>(basicDy.MX2));
        paramsObj.insert("MY2",static_cast<double>(basicDy.MY2));
        paramsObj.insert("ZZ1",static_cast<double>(basicDy.ZZ1));
        paramsObj.insert("ZZ2",static_cast<double>(basicDy.ZZ2));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pSetPlayLoad(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    float playLoad = static_cast<float>(packet.paramsObj.value("playLoad").toDouble());

    int res = SetPlayLoad(playLoad);
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pGetPlayLoad(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    float playLoad;
    int res = GetPlayLoad(&playLoad);
    if (res == DobotCommunicate_NoError)
    {
        paramsObj.insert("playLoad",static_cast<double>(playLoad));
    }
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pSetExcitMode(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);
    uint8_t excitMode = static_cast<uint8_t>(packet.paramsObj.value("excitMode").toInt());
    int res = SetExcitMode(excitMode);
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pGetExcitMode(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);
    uint8_t excitMode = static_cast<uint8_t>(packet.paramsObj.value("excitMode").toInt());

    int res = GetExcitMode(&excitMode);
    QJsonObject paramsObj;
    paramsObj.insert("excitMode", excitMode);
    return getCmdResultObj(packet, res, excitMode);
}

QJsonObject M1Controller::pSetCollisionThreshold(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    TorDiffParams torDiffParams;
    torDiffParams.torDiffJ1 = static_cast<float>(packet.paramsObj.value("torDiffJ1").toDouble());
    torDiffParams.torDiffJ2 = static_cast<float>(packet.paramsObj.value("torDiffJ2").toDouble());
    torDiffParams.torDiffJ3 = static_cast<float>(packet.paramsObj.value("torDiffJ3").toDouble());
    torDiffParams.torDiffJ4 = static_cast<float>(packet.paramsObj.value("torDiffJ4").toDouble());
    int res = SetCollisionThreshold(&torDiffParams);

    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pGetCollisionThreshold(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    TorDiffParams torDiffParams;
    int res = GetCollisionThreshold(&torDiffParams);
    QJsonObject paramsObj;
    paramsObj.insert("torDiffJ1", static_cast<double>(torDiffParams.torDiffJ1));
    paramsObj.insert("torDiffJ2", static_cast<double>(torDiffParams.torDiffJ2));
    paramsObj.insert("torDiffJ3", static_cast<double>(torDiffParams.torDiffJ3));
    paramsObj.insert("torDiffJ4", static_cast<double>(torDiffParams.torDiffJ4));
    return getCmdResultObj(packet, res, paramsObj);
}

QJsonObject M1Controller::pSetExcitCmd(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    ExcitCmd excitCmd;
    excitCmd.q1 = static_cast<float>(packet.paramsObj.value("q1").toDouble());
    excitCmd.q2 = static_cast<float>(packet.paramsObj.value("q2").toDouble());
    excitCmd.dq1 = static_cast<float>(packet.paramsObj.value("dq1").toDouble());
    excitCmd.dq2 = static_cast<float>(packet.paramsObj.value("dq2").toDouble());
    excitCmd.ddq1 = static_cast<float>(packet.paramsObj.value("ddq1").toDouble());
    excitCmd.ddq2 = static_cast<float>(packet.paramsObj.value("ddq2").toDouble());
    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);
    bool isWaitForFinish = packet.paramsObj.value("isWaitForFinish").toBool(true);
    int timeOut = packet.paramsObj.value("syncTimeOut").toInt(90000);

    quint64 queuedCmdIndex = 0;

    int res = SetExcitCmd(&excitCmd, isQueued, &queuedCmdIndex);
    /* wait for finish */
    qDebug()<<"SetExcitCmd isWaitForFinish"<<isWaitForFinish;
    if (isWaitForFinish == true && res == DobotCommunicate_NoError && isQueued == true) {
        M1WaitForFinish *wait = new M1WaitForFinish(info.id, queuedCmdIndex, packet, this);
        wait->startWaiting(100, timeOut);
        connect(wait, &M1WaitForFinish::finish_signal, this, &M1Controller::waitForFinish_slot);
        m_waitFinishMap.insert(wait, info.id);
        return QJsonObject();
    }
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pSetSafeMode(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    uint8_t safeMode = static_cast<uint8_t>(packet.paramsObj.value("safeMode").toDouble());

    int res = SetSafeMode(safeMode);
    return getCmdResultObj(packet, res);
}

QJsonObject M1Controller::pGetSafeMode(const M1Packet &packet)
{
    qDebug()<<__FUNCTION__;
    DeviceInfo info = m_DevInfoMap.value(packet.portName);

    QJsonObject paramsObj;
    uint8_t safeMode;
    int res = GetSafeMode(&safeMode);
    paramsObj.insert("safeMode", safeMode);
    return getCmdResultObj(packet, res, paramsObj);
}

/* SLOT */
void M1Controller::handleCheckTimeout_slot()
{
    if (isCheckState == true) {
        updataDeviceState();
    }
}

/* 退出前处理 */
void M1Controller::closeAllDevice_slot()
{
    checkStateTimer->stop();

    foreach (const QString &portName, m_DevInfoMap.keys()) {
        pDisConnectDobot(portName);
    }

    delete checkStateTimer;
}

void M1Controller::waitForFinish_slot(int res)
{
    auto wait = qobject_cast<M1WaitForFinish*>(sender());
    m_waitFinishMap.remove(wait);

    emit sendFinishPacket_signal(getCmdResultObj(wait->m_packet, res));

    wait->deleteLater();
}
