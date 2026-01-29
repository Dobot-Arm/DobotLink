#include "MagicDevicePlugin.h"

#include <QList>
#ifndef __wasm__
#include <QSerialPortInfo>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QNetworkDatagram>
#else
#include "WebAssembly/EmSerialPort.h"
#endif
#include <QDebug>

#define DEVICE_DLL_ERROR_BASE (120)

const QString MagicDevicePlugin::PluginName = "MagicDevice";
const QString MagicDevicePlugin::Version = "3.2.4";
#ifndef __wasm__
const QByteArray BroadCastMessage = "WhoisDobotM1";
const quint16 BroadCastPort = 6000;

const QByteArray BROADCAST_KEYWORD = "Who is Dobot?";
const int BROADCAST_PORT  = 48899;
#endif

#ifndef __wasm__
MagicDevicePlugin::MagicDevicePlugin(QObject *parent) : DPluginInterface(parent)
{
    m_udpSocket = new QUdpSocket();//临时方案：不挂父对象，解决有时候退出进程时崩溃问题
    connect(m_udpSocket, &QUdpSocket::readyRead,
            this, &MagicDevicePlugin::_onUdpReadyRead_slot);
    m_udpSocketUSR = new QUdpSocket();
    connect(m_udpSocketUSR, &QUdpSocket::readyRead,
            this, &MagicDevicePlugin::onUdpUSRReadyRead_slot);
#else
MagicDevicePlugin::MagicDevicePlugin() : QObject(nullptr)
{
#endif
    m_handlingid = 0;
    m_researchFileter = false;
    m_isSearchingDevices = false;

    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);
    m_searchTimer->setInterval(100);
    connect(m_searchTimer, &QTimer::timeout,
            this, &MagicDevicePlugin::_onSearchTimeout_slot);
}

MagicDevicePlugin::~MagicDevicePlugin()
{
    m_searchTimer->disconnect();
    m_searchTimer->stop();
}

QString MagicDevicePlugin::getVersion()
{
    QString protocolV = MagicDevice::getProtocolVersion();
    QString version = QString("%1(protocal:%2)").arg(Version).arg(protocolV);
    return version;
}

/* 收到消息 */
void MagicDevicePlugin::pReceiveMassage_slot(QString id, QJsonObject obj)
{
    if (id == "ALL") {
        _handleDobotLinkCommand(obj);
    } else if (id.contains(PluginName)) {
        _handleMagicDeviceCommand(obj);
    }
}

/* [!!!消息分发!!!] */
void MagicDevicePlugin::_handleDobotLinkCommand(const QJsonObject &obj)
{
    qDebug() << "[ALL] {MagicDevice} get obj" << obj;
    if (obj.contains("METHOD")) {
        QString method = obj.value("METHOD").toString();

        if (method == "EXIT") {
            _closeAllDevice();
        } else if (method == "CloseWebSocket") {
            QJsonObject params = obj.value("params").toObject();
            quint16 port = static_cast<quint16>(params.value("WSport").toInt());
            pDisconnectDobot(port);
        }
    }
}

void MagicDevicePlugin::_handleMagicDeviceCommand(const QJsonObject &obj)
{
    DRequestPacket packet;
    packet.setPacketFromObj(obj);
    DResultPacket resPacket(packet);

    m_sourceObj = obj;

    if (m_requestPacketMap.contains(packet.id)) {
        qWarning() << tr("The same id can cause system instability.");
    }

    m_requestPacketMap.insert(packet.id, packet);

    if (packet.api == "SearchDobot") {
        pSearchDobot(packet);
    } else {
        /* check portName */
        if (packet.getParamValue("portName").toString().isEmpty()) {
            m_requestPacketMap.remove(packet.id);
            emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_INVALID_PORTNAME));
        } else {
            if (packet.api == "ConnectDobot") {
                pConnectDobot(packet);
            } else {
                QString portName = packet.getParamValue("portName").toString();
                MagicDevice *device = m_deviceMap.value(portName, nullptr);
                if (device != nullptr)
                {
                    if (packet.api == "DisconnectDobot") {
                        pDisconnectDobot(device, packet);
                    } else if (packet.api == "DownloadProgram") {
                        _handleDownloadCmd(device, packet);
                    } else if (packet.api == "CheckBoxSpace") {
                        _handleCheckBoxSpaceCmd(device, packet);
                    } else if (packet.api == "ClearBoxSpace") {
                        _handleClearBoxSpaceCmd(device, packet);
                    } else if (packet.api == "QueuedCmdStop") {
                        _pQueuedCmdStop(device, packet);
                    } else if (packet.api == "SetCommuTimeout") {
                        _pSetCommuTimeout(device, packet);
                    } else {
                        bool ok = false;
                        if (_checkActionApi(packet.api)) {
                            ok = _handleActionCmd(device, packet);
                        } else if (_checkQueueApi(packet.api)) {
                            ok = _handleQueueCmd(device, packet);
                        }  else {
                            ok = SendCommandInner(device, obj);
                        }
//                        if (ok == false) {
//                            emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_INVALID_COMMAND));
//                        }
                    }
                } else {
                    m_requestPacketMap.remove(packet.id);
                    emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_INVALID_DEVICE));
                }
            }
        }
    }
}

void MagicDevicePlugin::_getAvailableSerialPort(QStringList filter)
{
#ifndef __wasm__
    foreach (const QSerialPortInfo &portInfo, QSerialPortInfo::availablePorts()) {
#else
    foreach (const EmSerialPortInfo &portInfo, EmSerialPortInfo::availablePorts()) {
#endif
        DeviceInfo info;
        info.portName = portInfo.portName();
        info.description = portInfo.description();
        info.status = "unconnected";

        if (m_deviceMap.contains(portInfo.portName())) {
            MagicDevice *device = m_deviceMap.value(portInfo.portName());
            if (device) {
                /* unconnected/connected/occupied/unknown */
                info.status = device->getConnectStatus();
            }
        }

        m_preDeviceMap.insert(portInfo.portName(), info);
    }

    if (!filter.isEmpty()) {
        foreach (const QString portName, m_preDeviceMap.keys()) {
            foreach (const QString &f, filter) {
                if (portName.contains(f, Qt::CaseInsensitive)) {
                    m_preDeviceMap.remove(portName);
                    break;
                }
            }
        }
    }
}

void MagicDevicePlugin::pSearchDobot(const DRequestPacket &packet)
{
    if (m_isSearchingDevices == true) {
        _sendErrorMessage(packet, ERROR_DL_API_BUSY);
        return;
    }

    m_isSearchingDevices = true;

    QString filter = packet.getParamValue("filter").toString();
    QStringList filterList = filter.split(" ", QString::SkipEmptyParts);
    QString type = packet.target;
    bool isM1 = (type.compare("M1", Qt::CaseInsensitive) == 0) ? true : false;

    m_preDeviceMap.clear();

    _getAvailableSerialPort(filterList);

    if (isM1) {
        m_m1deviceMap.clear();
        m_searchTimer->setProperty("id", packet.id);
        _broadcastForSearchM1();
    } else {
        QJsonArray array = _getSearchResult();
        _sendResMessage(packet, array);
        m_isSearchingDevices = false;
    }
}

QJsonArray MagicDevicePlugin::_getSearchResult()
{
    QJsonArray devicesArray;
    foreach (const DeviceInfo &info, m_preDeviceMap.values()) {
        QJsonObject deviceObj;
        deviceObj.insert("portName", info.portName);
        deviceObj.insert("description", info.description);
        deviceObj.insert("status", info.status);
        devicesArray.append(deviceObj);
    }
    return devicesArray;
}

void MagicDevicePlugin::pConnectDobot(const DRequestPacket &packet)
{
    QString portName = packet.getParamValue("portName").toString();
    qDebug() << __FUNCTION__ << QString("id:%1, portName:%2").arg(packet.id).arg(portName);

    MagicDevice *device = m_deviceMap.value(portName);
    if (device == nullptr) {
        device = new MagicDevice(this);
        device->setPortName(portName);

        static QRegExp rx("((25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)\\.){3}(25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)");
        if (rx.indexIn(portName) > -1) {
            /* M1 */
            QString localHostIP;
            QStringList list = portName.split(".");
            list.removeLast();
            QString iphead = list.join(".");
            foreach (QString ip, _getHostIpList()) {
                if (ip.contains(iphead)) {
                    localHostIP = ip;
                    qDebug() << "verify localIp:" << localHostIP;
                    break;
                }
            }
            device->setHostIpAddress(localHostIP);
        }

        connect(device, &MagicDevice::onResultMessage_signal,
                this, &MagicDevicePlugin::_handleReceiveMessage_slot);
        connect(device, &MagicDevice::onMagicianGoResultMessage_signal, [=](QJsonObject obj){
            if (obj.contains("id")) {
                quint64 id = static_cast<quint64>(obj.value("id").toInt());
                m_requestPacketMap.remove(id);
            }
            emit pSendMessage_signal(PluginName, obj);
        });
        // 串口报错
        connect(device, &MagicDevice::serialErrorOccurred_signal, [=](int code, QString errorString){
            DNotificationPacket notiPacket(device->getWebsocketPort());
            QJsonObject paramsObj;
            paramsObj.insert("portName", device->getPortName());
            paramsObj.insert("code", code);
            paramsObj.insert("message", errorString);
            QString method = "dobotlink.notification.serialError";
            QJsonObject notificationObj = notiPacket.getNotificationObj(method, paramsObj);

            emit pSendMessage_signal(PluginName, notificationObj);
        });

        qDebug() << "create a device. portName:" << portName << "wsPort:" << packet.wsPort;
        m_deviceMap.insert(portName, device);
    }

    /* 绑定 websocket 端口 */
    device->setWebsocketPort(packet.wsPort);

    bool isNeedCheckFirmware(false);
    if (packet.target.compare("Magician", Qt::CaseInsensitive) == 0) {
        isNeedCheckFirmware = true;
        device->setDeviceType(DEVICE_MAGICIAN);
    } else if (packet.target.compare("MagicianLite", Qt::CaseInsensitive) == 0) {
        device->setDeviceType(DEVICE_MAGICIAN_LITE);
    } else if (packet.target.compare("MagicBox", Qt::CaseInsensitive) == 0) {
        device->setDeviceType(DEVICE_MAGICBOX);
    } else if (packet.target.compare("M1", Qt::CaseInsensitive) == 0) {
        device->setDeviceType(DEVICE_M1);
    } else if (packet.target.compare("MagicianGO", Qt::CaseInsensitive) == 0) {
//        device->setDeviceType(DEVICE_MAGICBOX);
        qDebug() << "MagicianGO no connect function";
    }else {
        qDebug() << "Device type is not specified.";
    }

    device->connectDevice(packet.wsPort, packet.id);
}

void MagicDevicePlugin::pDisconnectDobot(MagicDevice *device, const DRequestPacket &packet)
{
    QString portName = packet.getParamValue("portName").toString();
    qDebug() << __FUNCTION__ << QString("id:%1, portName:%2").arg(packet.id).arg(portName);

    if (device) {
        device->disConnectDevice(packet.id);
    }
}

void MagicDevicePlugin::pDisconnectDobot(quint16 wsport)
{
    qDebug() << __FUNCTION__ << "websocket port:" << wsport;

    foreach (MagicDevice *device, m_deviceMap.values()) {
        if (device && device->getWebsocketPort() == wsport) {
            device->disConnectDevice();
        }
    }
}

/* 检查是否为运动等待指令 */
bool MagicDevicePlugin::_checkActionApi(QString api)
{
    QStringList actionList;
    actionList << "SetHOMECmd" << "SetPTPCmd" << "SetPTPWithLCmd"
               << "SetPTPPOCmd" << "SetPTPPOWithLCmd"
               << "SetCPCmd" << "SetCPLECmd" << "SetARCCmd" << "SetCircleCmd"
               << "SetMotivateCmd" << "SetTRIGCmd" << "SetWAITCmd"
                  /*Magician GO api */
               << "SetRotate" << "SetMoveDist"
               << "SetMovePos" << "SetArcRad"
               << "SetArcCent" << "SetIncrementClosedLoop"
                <<"SetAutoLeveling";

    return actionList.contains(api);
}

/* 设置默认等待运动时间 */
int MagicDevicePlugin::_getDefaultTimeoutValue(QString devideType, QString cmd)
{
    int defaultTimeout = 5000;
    if (cmd == "SetHOMECmd") {
        if (devideType == "Magician") {
            defaultTimeout = 30000;
        } else if (devideType == "MagicianLite") {
            defaultTimeout = 8000;
        } else if (devideType == "MagicBox") {
            defaultTimeout = 30000;
        } else if (devideType == "M1") {
            defaultTimeout = 60000;
        } else {
            defaultTimeout = 5000;
        }
    } else {
        if (devideType == "M1") {
            defaultTimeout = 60000;
        } else {
            defaultTimeout = 5000;
        }
    }
    return defaultTimeout;
}

/* 需要运动等待的 api */
bool MagicDevicePlugin::_handleActionCmd(MagicDevice *device, const DRequestPacket &packet)
{
    qDebug() << __FUNCTION__ << QString("id:%1, api:%2").arg(packet.id).arg(packet.api);

    MessagePacket sendPacket(packet.api, packet.id);
    sendPacket.setParams(packet.paramsObj);

    if (!packet.paramsObj.contains("isQueued")) {
        qDebug() << "'isQueued':missing, use (true)";
    }
    if (!packet.paramsObj.contains("isWaitForFinish")) {
        qDebug() << "'isWaitForFinish':missing, use (true)";
    }

    bool isQueued = packet.paramsObj.value("isQueued").toBool(true);
    bool isWaitForFinish = packet.paramsObj.value("isWaitForFinish").toBool(true);
    sendPacket.setIsQueue(isQueued);

    if (isQueued == true and isWaitForFinish == true) {
        int defaultValue = _getDefaultTimeoutValue(packet.target, packet.api);
        if (!packet.paramsObj.contains("timeout")) {
            qDebug().noquote() << QString("'timeout':missing, use(%1)").arg(defaultValue);
        }

        int timeout = packet.paramsObj.value("timeout").toInt(defaultValue);
        sendPacket.setWaitForFinishEnable(isWaitForFinish, timeout);
    }

    if (packet.target == "MagicianLite" and !packet.paramsObj.contains("slaveIndex")) {
        qDebug() << "'slaveIndex':missing, use (0)";
    }

    quint8 slaveIndex = static_cast<quint8>(packet.paramsObj.value("slaveIndex").toInt());
    sendPacket.setTargetType(packet.target, slaveIndex);

    if (device) {
        bool ok = device->sendCommand(sendPacket);
        return ok;
    }
    return false;
}

/* 检查是否为队列（非运动等待）指令 */
bool MagicDevicePlugin::_checkQueueApi(QString api)
{
    if (!api.startsWith("Set")) {
        return false;
    }

    if (api.endsWith("Params") || api.endsWith("Cmd") || api.endsWith("Sensor")) {
        return true;
    }

    if (api.contains("EndEffector")) {
        return true;
    }

    if (api.startsWith("SetIO")) {
        return true;
    }

    if (api == "SetEMotor") {
        return true;
    }
    return false;
}

/* 处理队列指令 API */
bool MagicDevicePlugin::_handleQueueCmd(MagicDevice *device, const DRequestPacket &packet)
{
    qDebug() << __FUNCTION__ << QString("id:%1, api:%2").arg(packet.id).arg(packet.api);

    MessagePacket sendPacket(packet.api, packet.id);
    sendPacket.setParams(packet.paramsObj);

    QStringList list;
    list << "SetCPCmd" << "SetCPLECmd";

    bool defaultValue = false;
    if (list.contains(packet.api)) {
        defaultValue = true;
    }

    bool isQueued = packet.paramsObj.value("isQueued").toBool(defaultValue);
    sendPacket.setIsQueue(isQueued);

    quint8 slaveIndex = static_cast<quint8>(packet.paramsObj.value("slaveIndex").toInt());
    sendPacket.setTargetType(packet.target, slaveIndex);

    if (device) {
        bool ok = device->sendCommand(sendPacket);
        return ok;
    }
    return false;
}

void MagicDevicePlugin::_pQueuedCmdStop(MagicDevice *device, const DRequestPacket &packet)
{
    qDebug() << __FUNCTION__;

    MessagePacket p(packet.api, packet.id);
    bool isForceStop = packet.paramsObj.value("forceStop").toBool();
    if (isForceStop == true) {
        p.setCommand("QueuedCmdForceStop");
    }
    p.setTargetType(packet.target);
    if (device) {
        device->sendCommand(p);
    }
}

void MagicDevicePlugin::_pSetCommuTimeout(MagicDevice *device, const DRequestPacket &packet)
{
    qDebug() << __FUNCTION__;

    int timeout = packet.paramsObj.value("timeout").toInt();

    if (device) {
        device->setCommuTimeout(packet.id, timeout);
    }
}

bool MagicDevicePlugin::_pSendCommand(MagicDevice *device, const DRequestPacket &packet)
{
    MessagePacket p(packet.api, packet.id);
    p.setParams(packet.paramsObj);
    p.setSourceObj(m_sourceObj);
    p.setTargetType(packet.target);
    p.wsPort = packet.wsPort;

    if (device) {
        bool ok = device->sendCommand(p);
        return ok;
    }
    return false;
}

bool MagicDevicePlugin::SendCommandInner(MagicDevice *device, const QJsonObject &obj)
{
    bool bOk = false;
    DRequestPacket packet;
    packet.setPacketFromObj(obj);
    DResultPacket resPacket(packet);

    if ("GetEndEffectorStatus" == packet.api)
    {
        /*
         * 这个接口，是要将GetEndEffectorType，GetEndEffectorSuctionCup，GetEndEffectorGripper的功能合并起来一次性请求获取，
         * 但是目前下位机没有这样的命令，不支持这样的操作，所以只能在应用层做接口任务分解，待到所有请求都完毕后再合并结果，并返回给前端*/

        //先删除，否则下列操作会出现重复id
        m_requestPacketMap.remove(packet.id);

        QStringList all;
        all<<"GetEndEffectorType"<<"GetEndEffectorSuctionCup"<<"GetEndEffectorGripper";
        MagicSpecialTask task;
        task.response = resPacket;
        for(int i=0; i<all.size(); ++i)
        {
            QJsonObject objTask(obj);
            QString strMethod = "dobotlink."+packet.target+"."+all[i];
            objTask.insert("method", strMethod);
            task.request.enqueue(objTask);
        }
        auto itr = m_magicSpecialTask.insert(packet.id, task);

        //取出一个并开始发送
        QJsonObject objTask = itr.value().request.dequeue();

        bOk = SendCommandSpecial(device, objTask);
    }
    else
    {
        bOk = _pSendCommand(device, packet);
    }
    return bOk;
}

bool MagicDevicePlugin::SendCommandSpecial(MagicDevice *device, const QJsonObject &obj)
{
    m_sourceObj = obj;

    DRequestPacket dataSendPack;
    dataSendPack.setPacketFromObj(obj);
    m_requestPacketMap.insert(dataSendPack.id, dataSendPack);

    return _pSendCommand(device, dataSendPack);
}

bool MagicDevicePlugin::DoSpecialResponse(MagicDevice *device, quint64 id, QString cmd, int res, QJsonValue params)
{
    auto itrSpecial = m_magicSpecialTask.find(id);
    if (itrSpecial == m_magicSpecialTask.end())
    {
        return false;
    }

    QJsonObject resObj;
    if (res == NOERROR) {
        if (params.isNull() or params.isUndefined()) {
            resObj.insert("result", true);
        } else {
            resObj.insert("result", params);
        }
        itrSpecial.value().result.insert(cmd, resObj);
    } else {
        QJsonObject errorObj;
        errorObj.insert("code", res);
        errorObj.insert("message", DError::getErrorMessage(res));
        resObj.insert("error", errorObj);
        itrSpecial.value().result.insert(cmd, resObj);
    }
    if (itrSpecial->request.isEmpty())
    {
        DResultPacket resPacket;
        emit pSendMessage_signal(PluginName, itrSpecial.value().response.getResultObj(itrSpecial.value().result));
        m_magicSpecialTask.remove(id);
    }
    else
    {
        QJsonObject objTask = itrSpecial.value().request.dequeue();
        SendCommandSpecial(device, objTask);
    }
}

/* 处理box下载命令 */
void MagicDevicePlugin::_handleDownloadCmd(MagicDevice *device, const DRequestPacket &packet)
{
    Q_UNUSED(device)
    QString code;
    QString fileName;
    if (packet.paramsObj.contains("code")) {
        QString base64Code = packet.paramsObj.value("code").toString();
        QByteArray base64Bytes = base64Code.toUtf8();
        code = QString(QByteArray::fromBase64(base64Bytes));

        fileName = packet.paramsObj.value("fileName").toString("temp");
    }

    DBoxDownload* thread = new DBoxDownload(NULL);
    thread->setDownloadParams(fileName, code, packet.id);
    connect(thread, &DBoxDownload::onDownloadFinished_signal,
            this, &MagicDevicePlugin::_handleDownloadFinished_slot);
    connect(thread, &DBoxDownload::finished,
            thread, &QObject::deleteLater);
    thread->start();
}

/* 检查box剩余空间是否够创建一个脚本文件 */
void MagicDevicePlugin::_handleCheckBoxSpaceCmd(MagicDevice *device, const DRequestPacket &packet)
{
    Q_UNUSED(device)
    m_requestPacketMap.remove(packet.id);
    DResultPacket resPacket(packet);
    QString code;
    if (packet.paramsObj.contains("code")) {
        QString base64Code = packet.paramsObj.value("code").toString();
        QByteArray base64Bytes = base64Code.toUtf8();
        code = QString(QByteArray::fromBase64(base64Bytes));
    }else{
        emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_INVALID_PARAMS));
    }
    bool bCan = DBoxDownload::isCanWriteCode(code);
    emit pSendMessage_signal(PluginName, resPacket.getResultObj(bCan));
}

/* 清空box的空间内存 */
void MagicDevicePlugin::_handleClearBoxSpaceCmd(MagicDevice *device, const DRequestPacket &packet)
{
    Q_UNUSED(device)
    bool bVal = DBoxDownload::clearBoxSpace();
    m_requestPacketMap.remove(packet.id);
    DResultPacket resPacket(packet);
    emit pSendMessage_signal(PluginName, resPacket.getResultObj(bVal));
}

/* 接收 Device 消息 */
void MagicDevicePlugin::_handleReceiveMessage_slot(quint64 id, QString cmd, int res, QJsonValue params)
{
    MagicDevice *device = qobject_cast<MagicDevice *>(sender());
    ErrorType e = static_cast<ErrorType>(res);

    if (cmd.isEmpty()) {
        qDebug() << "No cmd value. id:" << id;
    }

    if (m_requestPacketMap.contains(id)) {
        DRequestPacket requestPacket = m_requestPacketMap.take(id);

        if (DoSpecialResponse(device,id,cmd,res,params))
        {
            qDebug()<<"response ok";
        }
        else
        {
            DResultPacket resPacket(requestPacket);
            if (res == NOERROR) {
                emit pSendMessage_signal(PluginName, resPacket.getResultObj(params));
            } else {
                emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(res));
            }
        }
    } else if (e == ERROR_DEVICE_LOST_CONNECTION) {
        DNotificationPacket notiPacket(device->getWebsocketPort());
        QJsonObject paramsObj;
        paramsObj.insert("portName", device->getPortName());
        QString method = "dobotlink.notification.lostConnection";
        QJsonObject notificationObj = notiPacket.getNotificationObj(method, paramsObj);

        emit pSendMessage_signal(PluginName, notificationObj);
    } else {
        qDebug() << "request packet is not found. id:" << id;
    }
}

void MagicDevicePlugin::_onUdpReadyRead_slot()
{
#ifndef __wasm__
    QByteArray m_data;
    while (m_udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_udpSocket->receiveDatagram();
        QString deviceName = datagram.data();
        qDebug() << "[UDP BroadCast]get reply:" << deviceName;

        if (deviceName.startsWith("dobotM1_")) {
            QString address = datagram.senderAddress().toString();
            QRegExp rx("((25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)\\.){3}(25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)");
            if (rx.indexIn(address) > -1) {
                QString deviceIp = rx.cap();
                m_m1deviceMap.insert(deviceIp, deviceName);
                qDebug() << "[UDP BroadCast]find a M1 Device, ip:" << deviceIp << "name:" << deviceName;
            }
        }
    }
#endif
}

void MagicDevicePlugin::onUdpUSRReadyRead_slot()
{
#ifndef __wasm__
    while (m_udpSocketUSR->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_udpSocketUSR->receiveDatagram();
        QString strContent = datagram.data();
        //strContent=192.168.137.128,D8B04CB33868,USR-C322,2.17.14
        qDebug() << "[UDP USR BroadCast]get reply:" << strContent;

        QString deviceIp;
        QString deviceName;
        QStringList lstItem = strContent.split(',');
        if (lstItem.size()>0)
        {
            QString str = lstItem.at(0);
            QRegExp reg("^((25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)\\.){3}(25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)$");
            if (!str.isEmpty() && reg.exactMatch(str))
            {
                deviceIp = str;
            }
        }
        if (lstItem.size()>1)
        {
            deviceName += lstItem.at(1);
        }
        if (lstItem.size()>2)
        {
            deviceName += lstItem.at(2);
        }
        if (!deviceIp.isEmpty())
        {
            m_m1deviceMap.insert(deviceIp, deviceName);
            qDebug() << "[UDP USR BroadCast]find a Magician Device, ip:" << deviceIp << "name:" << deviceName;
        }
    }
#endif
}

void MagicDevicePlugin::_onSearchTimeout_slot()
{
    if (!m_m1deviceMap.isEmpty()) {
        foreach (const QString &ip, m_m1deviceMap.keys()) {

            DeviceInfo info;
            info.portName = ip;
            info.description = m_m1deviceMap.value(ip);
            info.status = "unconnected";

            if (m_deviceMap.contains(ip)) {
                MagicDevice *device = m_deviceMap.value(ip);
                if (device) {
                    info.status = device->getConnectStatus();
                }
            }

            m_preDeviceMap.insert(ip, info);
        }
    }

    QJsonArray array = _getSearchResult();
    _sendResMessage(m_searchTimer->property("id").toInt(), array);
    m_isSearchingDevices = false;
}

void MagicDevicePlugin::_closeAllDevice()
{
    foreach (const QString &portName, m_deviceMap.keys()) {
        MagicDevice *device = m_deviceMap.value(portName);
        if (device) {
            device->disConnectDevice();
        }
    }
}

void MagicDevicePlugin::_sendResMessage(const quint64 id, const QJsonValue resValue)
{
    DRequestPacket requestPacket = m_requestPacketMap.take(id);
    DResultPacket resPacket(requestPacket);
    emit pSendMessage_signal(PluginName, resPacket.getResultObj(resValue));
}

void MagicDevicePlugin::_sendResMessage(const DRequestPacket &request, const QJsonValue resValue)
{
    DResultPacket resPacket(request);
    emit pSendMessage_signal(PluginName, resPacket.getResultObj(resValue));
}

void MagicDevicePlugin::_sendErrorMessage(const quint64 id, const ErrorType type)
{
    DRequestPacket requestPacket = m_requestPacketMap.take(id);
    DResultPacket resPacket(requestPacket);
    emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(type));
}

void MagicDevicePlugin::_sendErrorMessage(const DRequestPacket &request, const ErrorType type)
{
    DResultPacket resPacket(request);
    emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(type));
}

void MagicDevicePlugin::_broadcastForSearchM1()
{
#ifndef __wasm__
    QStringList allIp = _getHostIpList();
    foreach (QString ip, allIp) {
        QString boradcastIP = m_ipAndBroadcastMap.value(ip);
        qDebug() << "[Info] localIP:" << ip << ", broadcastIP:" << boradcastIP;
        m_udpSocket->writeDatagram(BroadCastMessage, QHostAddress(boradcastIP), BroadCastPort);
    }

    foreach (QString ip, allIp) {
        QString boradcastIP = m_ipAndBroadcastMap.value(ip);
        qDebug() << "[Info-USR] localIP:" << ip << ", broadcastIP:" << boradcastIP;
        m_udpSocketUSR->writeDatagram(BROADCAST_KEYWORD, QHostAddress(boradcastIP), BROADCAST_PORT);
    }

    m_searchTimer->setInterval(2000);
    m_searchTimer->start();
#endif
}

QStringList MagicDevicePlugin::_getHostIpList()
{
    m_ipAndBroadcastMap.clear();
#ifndef __wasm__
    foreach (const QNetworkInterface &interface, QNetworkInterface::allInterfaces()) {
        if (!interface.isValid()) {
            continue;
        }

        QNetworkInterface::InterfaceFlags flags = interface.flags();
        if (flags.testFlag(QNetworkInterface::IsUp)
                and flags.testFlag(QNetworkInterface::IsRunning)
                and !flags.testFlag(QNetworkInterface::IsLoopBack))
        {
            if (interface.humanReadableName().contains("VMware")
                    || interface.humanReadableName().contains("Loopback")
                    || interface.humanReadableName().contains("VirtualBox"))
            {
                continue;
            }

            foreach (const QNetworkAddressEntry &entry, interface.addressEntries())
            {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    QString ip = entry.ip().toString();
                    QString netmask = entry.netmask().toString();
                    QString broadcast = entry.broadcast().toString();

                    if (!broadcast.isEmpty()) {
                        m_ipAndBroadcastMap.insert(ip, broadcast);
                    }
                }
            }
        }
    }
#endif
    return m_ipAndBroadcastMap.keys();
}

/* SLOT */
void MagicDevicePlugin::_handleDownloadFinished_slot(quint64 id, bool isOk)
{
    DRequestPacket requestPacket = m_requestPacketMap.take(id);
    DResultPacket resPacket(requestPacket);
    if (isOk) {
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    } else {
        emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_DEVICE_DOWNLOAD_FAILD));
    }
}
