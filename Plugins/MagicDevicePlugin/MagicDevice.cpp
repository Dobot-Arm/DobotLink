#include "MagicDevice.h"
#include "MagicDevice_p.h"
#ifndef __wasm__
#include <QSerialPort>
#include <QSerialPortInfo>
#else
#include "WebAssembly/EmSerialPort.h"
#endif
#include <QMetaObject>
#include <QDebug>

#include "DP2Packet.h"
#include "MessageHandler.h"
#include "DMagicianProtocol.h"
#include "DM1Protocol.h"
#include "DError/DError.h"
#include "DP3Packet.h"
#include "QEventLoop"
#include "QJsonObject"

#include "../version.h"

const QString VERSION = QString(VERSION_PLUGIN_MAGICDEVICE);
MessagePacket::MessagePacket(QString cmd, quint64 id) :
    id(id),
    m_cmd(cmd)
{
    wsPort = 0;
    m_isQueued = false;
    m_slaveID = 0;
    m_cmdid = 0;
    m_rw = 0;
    m_targetType = DEVICE_MAGICIAN;
    m_isPrivate = false;
    m_isWaitForFinish = false;
}

bool MessagePacket::isEmpty()
{
    return m_cmd.isEmpty();
}

void MessagePacket::setSourceObj(QJsonObject obj)
{
    m_sourceObj = obj;
}

QJsonObject MessagePacket::getSourceObj()
{
    return m_sourceObj;
}

void MessagePacket::setCommand(QString cmd)
{
    m_cmd = cmd;
}

void MessagePacket::setIsQueue(bool isQueued)
{
    m_isQueued = isQueued;
}

bool MessagePacket::getIsQueue()
{
    return m_isQueued;
}

void MessagePacket::setParams(QJsonObject params)
{
    if (!params.isEmpty()) {
        m_params = params;
    }
}

void MessagePacket::setSlaveID(quint8 slaveid)
{
    if (slaveid < 4) {
        m_slaveID = slaveid;
    }
}

quint8 MessagePacket::getSlaveID()
{
    return m_slaveID;
}

void MessagePacket::setTargetType(DeviceType type)
{
    m_targetType = type;
}

void MessagePacket::setTargetType(QString typeStr, quint8 slaveIndex)
{
    int type = MagicDevicePrivate::DeviceTypeMap.key(typeStr);
    m_targetType = DeviceType(type);
    setSlaveID(slaveIndex);
}

void MessagePacket::setWaitForFinishEnable(bool enable, int timeout)
{
    m_isWaitForFinish = enable;
    m_timeout = timeout;
}

bool MessagePacket::getIsWaitForFinish()
{
    return m_isWaitForFinish;
}

int MessagePacket::getTimeoutValue()
{
    return m_timeout;
}

QJsonObject MessagePacket::getParamsObj()
{
    return m_params;
}

QString MessagePacket::getCmdStr()
{
    return m_cmd;
}

void MessagePacket::setPrivatePacket(bool isPrivate)
{
    m_isPrivate = isPrivate;
}

DeviceType MessagePacket::getTargetType()
{
    return m_targetType;
}

QString MessagePacket::getTargetTypeStr()
{
    return MagicDevicePrivate::DeviceTypeMap.value(m_targetType);
}

QJsonObject MessagePacket::getPacketObj()
{
    QJsonObject resObj;
    resObj.insert("WSport", static_cast<quint16>(wsPort));
    resObj.insert("id", static_cast<double>(id));
    resObj.insert("cmd", m_cmd);
    resObj.insert("isQueued", m_isQueued);
    resObj.insert("targetType", MagicDevicePrivate::DeviceTypeMap.value(m_targetType));
    resObj.insert("slaveIndex", m_slaveID);

    if (!m_params.isEmpty()) {
        resObj.insert("params", m_params);
    }
    if (m_isPrivate) {
        resObj.insert("isPrivate", true);
    }
    /* targetType 为信息包发送的目的单元 */
    /* deviceType 则表示信息的来源设备 */
    return resObj;
}

/* MagicDevicePrivate */

QMap<int, QString> MagicDevicePrivate::DeviceTypeMap = initDeviceTypeStringMap();
QMap<int, QString> MagicDevicePrivate::initDeviceTypeStringMap()
{
    QMap<int, QString> map;
    map.insert(DEVICE_MAGICIAN, "Magician");
    map.insert(DEVICE_MAGICIAN_LITE, "MagicianLite");
    map.insert(DEVICE_MAGICBOX, "MagicBox");
    map.insert(DEVICE_M1, "M1");
    map.insert(DEVICE_MAGICIAN_GO, "MagicianGO");
    return map;
}

QString MagicDevicePrivate::checkIsIpAddress(QString ip)
{
    QRegExp rx("((25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)\\.){3}(25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)");
    if (rx.indexIn(ip) > -1) {
        return rx.cap();
    }
    return QString();
}

MagicDevicePrivate::MagicDevicePrivate(MagicDevice *parent) : q_ptr(parent)
{
    poseX = 0;
    poseY = 0;
    poseZ = 0;
    poseR = 0;

    m_wsport = 0;
    m_deviceType = 0;

    m_status = UNCONNECTED;

    m_MessageHandler = new MessageHandler(this);
    connect(m_MessageHandler, &MessageHandler::errorOccurred_signal,
            this, &MagicDevicePrivate::errorOccurred_slot);
    connect(m_MessageHandler, &MessageHandler::serialErrorOccuerd_signal,
            this, &MagicDevicePrivate::serialErrorOccurred_slot);
    /* P2 */
    connect(m_MessageHandler, &MessageHandler::recviceP2Data_signal,
            this, &MagicDevicePrivate::recviceP2Data_slot);
    /* P3 */
    connect(m_MessageHandler, &MessageHandler::recviceP3Data_signal,
            this, &MagicDevicePrivate::recviceP3Data_slot);

    m_actionTimerManager = new ActionTimerManager(this);
    connect(m_actionTimerManager, &ActionTimerManager::getCurrentIndex_signal,
            this, &MagicDevicePrivate::checkCurrentIndex_slot);
    connect(m_actionTimerManager, &ActionTimerManager::actionFinish_signal,
            this, &MagicDevicePrivate::handleActionFinish_slot);
}

MagicDevicePrivate::~MagicDevicePrivate()
{
}

/* 发送获取queued index指令 */
void MagicDevicePrivate::checkCurrentIndex_slot()
{
    Q_Q(MagicDevice);

    MessagePacket packet("GetQueuedCmdCurrentIndex");
    packet.setTargetType(m_actionTimerManager->getCurrentTargetType(),
                         m_actionTimerManager->getCurrentSlaveIndex());
    packet.setPrivatePacket(true);
    // GO组包
    if (packet.getTargetType() == DeviceType::DEVICE_MAGICIAN_GO) {
        QJsonObject GOpacket;
        GOpacket.insert("method","dobotlink.MagicianGO.GetQueuedCmdCurrentIndex");
        GOpacket.insert("WSport", m_actionTimerManager->getWsPort());
        GOpacket.insert("id", 0);
        GOpacket.insert("jsonrpc", "2.0");
        packet.setSourceObj(GOpacket);
    }
    q->sendCommand(packet);
}


/* 收到[错误]:断开连接 通讯超时 队列满 */
void MagicDevicePrivate::errorOccurred_slot(int code, quint64 id)
{
    m_PacketListMap.remove(id);
    MessagePacket packet = m_RequestMap.take(id);

    Q_Q(MagicDevice);
    emit q->onResultMessage_signal(id, packet.getCmdStr(), code);
}

void MagicDevicePrivate::serialErrorOccurred_slot(int code, QString errorString)
{
    Q_Q(MagicDevice);
    emit q->serialErrorOccurred_signal(code, errorString);
}

/* 收到[common 结果] */
void MagicDevicePrivate::recviceP2Data_slot(QJsonObject message)
{
    Q_Q(MagicDevice);

    quint64 id = static_cast<quint64>(message.value("id").toDouble());
    QString cmd = message.value("cmd").toString();

    if (cmd == "GetPose") {
        QJsonObject poseParams = message.value("params").toObject();

        poseX = static_cast<float>(poseParams.value("x").toDouble());
        poseY = static_cast<float>(poseParams.value("y").toDouble());
        poseZ = static_cast<float>(poseParams.value("z").toDouble());
        poseR = static_cast<float>(poseParams.value("r").toDouble());
    }

    if (message.value("isPrivate").toBool() == true) {
        /* 检测是否有自发请求 */
        if (cmd == "GetQueuedCmdCurrentIndex") {
            QJsonObject paramsObj = message.value("params").toObject();
            if (paramsObj.contains("queuedCmdIndex")) {
                quint64 currentIndex = static_cast<quint64>(paramsObj.value("queuedCmdIndex").toDouble());
                m_actionTimerManager->updateCurrentIndex(currentIndex);
            }
        }
    } else if (m_actionTimerManager->containsTimerId(id)) {
        /* 检测是否有需要动作等待结束的指令 */
        /* 当发送队列指令后，会返回带有index的参数，获取该参数用以对比 */
        if (message.contains("params")) {
            QJsonObject paramsObj = message.value("params").toObject();
            quint64 targetIndex = static_cast<quint64>(paramsObj.value("queuedCmdIndex").toDouble());
            m_actionTimerManager->setTargetIndexWithId(id, targetIndex);
        } else {
            m_RequestMap.remove(id);
        }
    } else if (m_PacketListMap.contains(id)) {
        /* 是否有列队顺序执行指令 */
        m_RequestMap.remove(id);

        QList<MessagePacket> list = m_PacketListMap.value(id);

        if (!list.isEmpty()) {
            MessagePacket p = list.takeFirst();
            bool ok = q->sendCommand(p);
            if (ok && !list.isEmpty()) {
                m_PacketListMap.insert(id, list);
            } else {
                m_PacketListMap.remove(id);
            }
        } else {
            m_PacketListMap.remove(id);
        }
    } else {
        /* 常规结束指令 */
        emit q->onResultMessage_signal(id, cmd, NOERROR, message.value("params"));
        m_RequestMap.remove(id);
    }
}

/* 传递收到的MagicianGoPlugin结果 */
void MagicDevicePrivate::recviceP3Data_slot(QJsonObject obj)
{
    Q_Q(MagicDevice);
    QJsonObject message;
    quint64 id = static_cast<quint64>(obj.value("id").toDouble());
    QJsonObject resObj = obj.value("result").toObject();
    if (resObj.contains("queueCmdCurrentIndex")){
        message.insert("cmd", "GetQueuedCmdCurrentIndex");
        message.insert("isPrivate", true);
        message.insert("result", resObj);
    }
    QString cmd = message.value("cmd").toString();

    if (message.value("isPrivate").toBool() == true) {
        /* 检测是否有自发请求 */
        if (cmd == "GetQueuedCmdCurrentIndex") {
            QJsonObject paramsObj = message.value("result").toObject();
            if (paramsObj.contains("queueCmdCurrentIndex")) {
                quint64 currentIndex = static_cast<quint64>(paramsObj.value("queueCmdCurrentIndex").toDouble());
                m_actionTimerManager->updateCurrentIndex(currentIndex);
            }
        }
    } else if (m_actionTimerManager->containsTimerId(id)) {
        /* 检测是否有需要动作等待结束的指令 */
        /* 当发送队列指令后，会返回带有index的参数，获取该参数用以对比 */
        if (obj.contains("result")) {
            QJsonObject resultObj = obj.value("result").toObject();
            quint64 targetIndex = static_cast<quint64>(resultObj.value("index").toDouble());
            m_actionTimerManager->setTargetIndexWithId(id, targetIndex);
        } else {
            m_RequestMap.remove(id);
        }
    } else {
        /* 常规结束指令 */
        emit q->onMagicianGoResultMessage_signal(obj);
        m_RequestMap.remove(id);
    }
}

/* 运动结束 / 运动超时 */
void MagicDevicePrivate::handleActionFinish_slot(quint64 id, ActionTimerManager::FinishType type, QString targetType)
{
    Q_Q(MagicDevice);

    /* MagicianGO的处理 */
    if(targetType == "MagicianGO" and !m_RequestMap.contains(id)){
        QJsonObject resObj;
        QJsonValue id_int;
        id_int.toInt(id);
        resObj.insert("id", id_int);
        resObj.insert("jsonrpc", "2.0");
        if (type == ActionTimerManager::NOERROR){
            resObj.insert("result", true);
        } else if (type == ActionTimerManager::TIMEOUT){
            resObj.insert("error", "TIMEOUT");
        } else {
            resObj.insert("error", "CANCELED");
        }
        emit q->onMagicianGoResultMessage_signal(resObj);
    }

    if (m_RequestMap.contains(id)) {
        MessagePacket packet = m_RequestMap.take(id);

        if (type == ActionTimerManager::NOERROR)
        {
            emit q->onResultMessage_signal(id, packet.getCmdStr(), NOERROR);
            if (m_PacketListMap.contains(id)) {
                QList<MessagePacket> list = m_PacketListMap.value(id);

                if (!list.isEmpty()) {
                    MessagePacket p = list.takeFirst();
                    bool ok = q->sendCommand(p);
                    if (ok && !list.isEmpty()) {
                        m_PacketListMap.insert(id, list);
                    } else {
                        m_PacketListMap.remove(id);
                    }
                } else {
                    m_PacketListMap.remove(id);
                }
            }
        }
        else if (type == ActionTimerManager::TIMEOUT)
        {
            emit q->onResultMessage_signal(id, packet.getCmdStr(), ERROR_DEVICE_ACTION_TIMEOUT);
            if (m_PacketListMap.contains(id)) {
                m_PacketListMap.remove(id);
            }
            m_RequestMap.remove(id);
        }
        else if (type == ActionTimerManager::CANCELED)
        {
            emit q->onResultMessage_signal(id, packet.getCmdStr(), ERROR_DEVICE_ACTION_CANCELED);
            if (m_PacketListMap.contains(id)) {
                m_PacketListMap.remove(id);
            }
            m_RequestMap.remove(id);
        }
    } else {
        qDebug() << "can not find request id.";
    }
}


/* MagicDevice */
MagicDevice::MagicDevice(QObject *parent)
    : QObject(parent), Dptr(new MagicDevicePrivate(this))
{
    Q_D(MagicDevice);
    d->m_deviceType = DEVICE_UNKNOWN;
}

MagicDevice::MagicDevice(DeviceType type, QObject *parent)
    : QObject(parent), Dptr(new MagicDevicePrivate(this))
{
    Q_D(MagicDevice);
    d->m_deviceType = type;
}

MagicDevice::~MagicDevice()
{
    Q_D(MagicDevice);
    delete d;
}

QString MagicDevice::getDeviceTypeToString(DeviceType type)
{
    return MagicDevicePrivate::DeviceTypeMap.value(type);
}

DeviceType MagicDevice::getDeviceStringToType(QString type)
{
    int t = MagicDevicePrivate::DeviceTypeMap.key(type);
    return DeviceType(t);
}

QString MagicDevice::getProtocolVersion()
{
    return VERSION;
}

void MagicDevice::setDeviceType(DeviceType type)
{
    Q_D(MagicDevice);
    d->m_deviceType = type;

    if (type == DEVICE_M1) {
        d->m_MessageHandler->setProtocolType(MessageHandler::DEVICE_M1_PROTOCOL);
    } else {
        d->m_MessageHandler->setProtocolType(MessageHandler::DEVICE_MAGIC_PROTOCOL);
    }
}

DeviceType MagicDevice::getDeviceType()
{
    Q_D(MagicDevice);
    return DeviceType(d->m_deviceType);
}

void MagicDevice::setPortName(QString portName)
{
    Q_D(MagicDevice);

    QString ip = MagicDevicePrivate::checkIsIpAddress(portName);
    if (!ip.isEmpty()) {
        d->m_MessageHandler->setUdpIpAddress(ip);
    } else {
        d->m_MessageHandler->setPortName(portName);
    }
}

QString MagicDevice::getPortName()
{
    Q_D(MagicDevice);
    return d->m_MessageHandler->getPortName();
}

void MagicDevice::setWebsocketPort(quint16 wsport)
{
    Q_D(MagicDevice);
    d->m_wsport = wsport;
}

quint16 MagicDevice::getWebsocketPort()
{
    Q_D(MagicDevice);
    return d->m_wsport;
}

void MagicDevice::setHostIpAddress(QString ip)
{
    Q_D(MagicDevice);
    d->m_MessageHandler->setHostIpAddress(ip);
}

void MagicDevice::setDeviceIpAddress(QString ip)
{
    Q_D(MagicDevice);
    d->m_MessageHandler->setUdpIpAddress(ip);
}

bool MagicDevice::_checkP2Firmware(quint16 wsPort, quint64 id, QString &devType, QString &fwVer)
{
    Q_D(MagicDevice);
//    QEventLoop loop;

//    QJsonObject obj;
//    d->m_MessageHandler->sendProtocolData(id, DEVICE_MAGICIAN, obj);

    return true;
}

bool MagicDevice::_checkP3Firmware(quint16 wsPort, quint64 id, QString &devType, QString &fwVer)
{
    Q_D(MagicDevice);
//    QEventLoop loop;

//    QJsonObject obj;
//    obj.insert("WSport", wsPort);
//    obj.insert("id", 1);
//    obj.insert("targetType", 1);
//    obj.insert("cmd", "GetProductName");
//    d->m_MessageHandler->sendProtocolData(id, DEVICE_MAGICIAN_GO, obj);
    return true;
}


/* Connect Device */
void MagicDevice::connectDevice(quint16 wsPort, quint64 id)
{
    Q_D(MagicDevice);
#ifndef __wasm__
    QString devType, fwType, fwVer;
    if (d->m_MessageHandler->connectDevice(id, devType, fwType, fwVer)) {
//        if (fwVer.isEmpty()) {
//            // 不是马林固件
//            if (!_checkP2Firmware(wsPort, id, devType, fwVer) &&
//                !_checkP3Firmware(wsPort, id, devType, fwVer)) {
//                emit this->onResultMessage_signal(id, "ConnectDobot", ERROR_INVALIED_FIRMWARE);
//                return;
//            }
//        }
        d->m_status = CONNECTED;
        QJsonObject res;
        res.insert("productName", devType);
        res.insert("firmwareName", fwType);
        res.insert("firmwareVersion", fwVer);
        emit this->onResultMessage_signal(id, "ConnectDobot", NOERROR, res);
    }
#else
    d->m_MessageHandler->connectDevice(id, [this, id](QString devType, QString fwType, QString fwVer){
        Q_D(MagicDevice);
        d->m_status = CONNECTED;
        QJsonObject res;
        res.insert("productName", devType);
        res.insert("firmwareName", fwType);
        res.insert("firmwareVersion", fwVer);
        emit this->onResultMessage_signal(id, "ConnectDobot", NOERROR, res);
    });
#endif
}

/* DisConnect Device */
void MagicDevice::disConnectDevice(quint64 requestid)
{
    Q_D(MagicDevice);
    d->m_status = UNCONNECTED;

    d->m_actionTimerManager->clearTimers();
    d->m_PacketListMap.clear();

    d->m_MessageHandler->disconnectDevice(requestid);

    d->m_RequestMap.clear();
}

QString MagicDevice::getConnectStatus()
{
    Q_D(MagicDevice);

    //当串口被拔下来的时候，m_status的状态没法儿被通知修改，所以这里需要判断
    if (!d->m_MessageHandler->isConnected())
    {
        return QString("unconnected");
    }
    if (d->m_status == UNCONNECTED) {
        return QString("unconnected");
    } else if (d->m_status == CONNECTED) {
        return QString("connected");
    } else if (d->m_status == OCCUPIED) {
        return QString("occupied");
    }
    return QString("unknown");
}

/* Set time out value */
void MagicDevice::setCommuTimeout(quint64 requestid, int ms)
{
    if (ms > 0) {
        Q_D(MagicDevice);
        d->m_MessageHandler->setCommuTimeout(requestid, 3, static_cast<quint32>(ms));
    }
}

bool MagicDevice::sendCommand(MessagePacket &packet, bool connectCheck)
{
    Q_UNUSED(connectCheck)
    Q_D(MagicDevice);

    QJsonObject sourceJson = packet.getSourceObj();
    d->m_actionTimerManager->setWsPort((static_cast<quint16>(sourceJson.value("WSport").toDouble())));

    if (packet.getIsQueue() == true and packet.getIsWaitForFinish() == true) {
        if (!d->m_actionTimerManager->containsTimerId(packet.id)) {
            ActionTimerSet *timer = new ActionTimerSet(packet.id);
            timer->cmd = packet.getCmdStr();
            timer->timeoutValue = packet.getTimeoutValue();
            timer->targetType = packet.getTargetTypeStr();

            d->m_actionTimerManager->addTimer(timer);
        } else {
            qDebug() << "packet id error. There is a same id in ActionTimerManager";
            return false; // todo: 检查是否有错
        }
    }

    // todo：不太好，SetRCmd不应该作为SetPTPCmd的二次封装
    if (packet.getCmdStr() == "SetRCmd") {
        packet.setCommand("SetPTPCmd");

        QJsonObject paramsObj = packet.getParamsObj();
        paramsObj.insert("ptpMode", 1);

        paramsObj.insert("x", static_cast<double>(d->poseX));
        paramsObj.insert("y", static_cast<double>(d->poseY));
        paramsObj.insert("z", static_cast<double>(d->poseZ));
        packet.setParams(paramsObj);
    }

    d->m_RequestMap.insert(packet.id, packet);

    bool needReply(true);
    if (packet.getCmdStr() == "SetJOGCmdTime") {
        needReply = false;
    }

    QJsonObject packJson = packet.getPacketObj();
    return d->m_MessageHandler->sendProtocolData(packet.id, packet.getTargetType(), packJson, needReply);
}

void MagicDevice::stopQueueCmdTimer()
{
    Q_D(MagicDevice);
    d->m_actionTimerManager->clearTimers();
}

/* 公共函数 */
QStringList MagicDevice::getCommandList(QString deviceType)
{
    if (deviceType.compare("M1", Qt::CaseInsensitive) == 0) {
        return DM1Protocol::getInstance()->getCommandList();
    } else {
        return DMagicianProtocol::getInstance()->getCommandList();
    }
}

/* 暂时不用 */
QStringList MagicDevice::getDeviceList(bool isFiltered)
{
    Q_UNUSED(isFiltered);
    QStringList resList;
#ifndef __wasm__
    QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
#else
    QList<EmSerialPortInfo> availablePorts = EmSerialPortInfo::availablePorts();
#endif
    foreach (const auto &portInfo, availablePorts) {
        resList.append(portInfo.portName());
    }
    return resList;
}
