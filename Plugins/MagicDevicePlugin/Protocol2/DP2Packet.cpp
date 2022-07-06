#include "DP2Packet.h"

#include <QByteArray>
#include <QJsonObject>
#include <QDataStream>
#include <QDebug>

#include "DMagicianProtocol.h"
#include "DM1Protocol.h"

const quint8 Head1 = 0xAA;
const quint8 Head2 = 0xAA;

/****************************************  Payload::Ctrl  ****************************************
 * Bit 0    rw          0-读取 1-写入
 * Bit 1    isQueued    0-非队列指令 1-队列指令
 * Bit 4-5  targetType  00-发送给Magician/MagicianLite
 *                      01-发送给ExController
 * Bit 6-7  targetIndex 0~3-从机序号
 *************************************************************************************************/
PacketCtrl::PacketCtrl()
{
    c_rw = 0;
    c_isQueued = false;
    c_targetType = 0;
    c_slaveid = 0;
}

void PacketCtrl::setPacketCtrl(const quint8 ctrl)
{
    /* [0: rw] */
    c_rw = ctrl & (1 << 0);

    /* [1: isQueued] */
    quint8 isQueue = ctrl & (1 << 1);
    if (isQueue > 0) {
        c_isQueued = true;
    } else {
        c_isQueued = false;
    }

    /* [4~5: type] */
    quint8 type = ctrl & (3 << 4);
    c_targetType = type >> 4;

    /* [6~7: slaveid] */
    quint8 slaveid = ctrl & (3 << 6);
    c_slaveid = slaveid >> 6;
}

void PacketCtrl::clear()
{
    c_rw = 0;
    c_isQueued = false;
    c_targetType = 0;
    c_slaveid = 0;
}

quint8 PacketCtrl::getPacketCtrl()
{
    quint8 res = 0;

    if (c_rw > 0) {
        res |= (1 << 0);
    }
    if (c_isQueued == true) {
        res |= (1 << 1);
    }
    if (c_targetType > 0) {
        res |= (1 << 4);
    }
    if (c_slaveid < 4) {
        res |= (c_slaveid << 6);
    }
    return res;
}

QString PacketCtrl::getTargetType()
{
    return c_targetType == 0 ? QString("Magician/lite") : QString("MagicBox");
}

QString PacketCtrl::getRWType()
{
    return c_rw == 0 ? QString("Read") : QString("Write");
}

int PacketCtrl::getSlaveID()
{
    return c_slaveid;
}

/****************************************  Payload  ****************************************/

PacketPayload::PacketPayload()
{
    p_cmdID = 0;
}

void PacketPayload::setPacketPayload(QByteArray payload)
{
    if (payload.size() < 2) {
        qDebug() << "payload size error.";
        return;
    }

    QDataStream out(&payload, QIODevice::ReadOnly);
    quint8 ctrl = 0;
    out >> p_cmdID >> ctrl;

    p_ctrl.setPacketCtrl(ctrl);

    if (payload.size() >= 2) {
        p_params = payload.right(payload.size() - 2);
    }
}

QByteArray PacketPayload::getPacketPayload()
{
    QByteArray resPayload;
    QDataStream in(&resPayload, QIODevice::WriteOnly);
    in << p_cmdID << p_ctrl.getPacketCtrl();

    if (p_params.size() > 0) {
        resPayload.append(p_params);
    }
    return resPayload;
}

void PacketPayload::clear()
{
    p_cmdID = 0;
    p_ctrl.clear();
    p_params.clear();
}


/****************************************  Packet  ****************************************/
/* Packet:[ head1 | head2 | length | payload(ID | Ctrl | Params(...)) | checksum ] */

P2Packet::P2Packet(quint64 id) : m_id(id)
{
    m_isDetailMode = false;
    m_isEmpty = true;
    m_isPrivate = false;
}

bool P2Packet::isEmpty()
{
    return m_isEmpty;
}

void P2Packet::clear()
{
    m_id = 0;
    m_payload.clear();
    m_isEmpty = true;
}

void P2Packet::setPacketID(quint64 id)
{
    m_id = id;
}

quint64 P2Packet::getPacketID()
{
    return m_id;
}

void P2Packet::setPacket(QByteArray data)
{
    m_isEmpty = false;

    /* for detail print */
    if (m_isDetailMode == true) {
        qDebug() << "# packet:" << data.toHex();

        QString parseStr = data.toHex();
        parseStr.insert(4, " l:");
        parseStr.insert(4 + 3 + 2, " (cmdid:");
        parseStr.insert(9 + 8 + 2, " ctrl:");
        parseStr.insert(19 + 6 + 2, " params:");
        parseStr.insert(parseStr.length() - 2, ") checksum:");

        qDebug() << "# content:" << parseStr;
    }

    quint8 length = static_cast<quint8>(data[2]);

    /* [payload] */
    QByteArray payload = data.mid(3, length);
    m_payload.setPacketPayload(payload);

    /* for detail print */
    if (m_isDetailMode == true) {
        qDebug().noquote() << QString("# ctrl (targetType:%1, slaveID:%2, isQueued:%3, rw:%4)")
                              .arg(m_payload.p_ctrl.c_targetType)
                              .arg(m_payload.p_ctrl.c_slaveid)
                              .arg(m_payload.p_ctrl.c_isQueued)
                              .arg(m_payload.p_ctrl.c_rw);
    }
}

bool P2Packet::setPacket(QJsonObject data)
{
    if (!data.contains("cmd")) {
        qDebug() << "packet cmd missing.";
        return false;
    }

    m_isPrivate = data.value("isPrivate").toBool();
    m_isEmpty = false;

    QString cmd = data.value("cmd").toString();
    QJsonObject params = data.value("params").toObject();

    m_payload.p_ctrl.c_isQueued = data.value("isQueued").toBool();

    /* targetType */
    m_deviceType = data.value("targetType").toString();
    if (m_deviceType == "Magician"
            or m_deviceType == "MagicianLite"
            or m_deviceType == "M1") {
        m_payload.p_ctrl.c_targetType = 0;
    } else if (m_deviceType == "MagicBox") {
        m_payload.p_ctrl.c_targetType = 1;
    } else {
        qDebug() << __FUNCTION__ << "targetType is undefined.";
        return false;
    }

    /* slaveID */
    int slaveID = data.value("slaveIndex").toInt(0);
    m_payload.p_ctrl.c_slaveid = static_cast<quint8>(slaveID);

    bool ok = false;
    if (m_deviceType == "M1") {
        ok = DM1Protocol::getInstance()->setPayload(cmd, m_payload, params);
    } else {
        ok = DMagicianProtocol::getInstance()->setPayload(cmd, m_payload, params);
    }
    return ok;
}

/* 求校验和 */
quint8 P2Packet::_getCheckSum(QByteArray payload)
{
    quint8 sum = 0;
    for (int i = 0; i < payload.size(); ++i) {
        int t = payload.at(i);
        sum += static_cast<quint8>(t);
    }
    quint8 res = static_cast<quint8>(256 - sum);
    return res;
}

QJsonObject P2Packet::getMessage()
{
    QJsonObject resObj;
    resObj.insert("id", static_cast<double>(m_id));
    resObj.insert("cmdid", m_payload.p_cmdID);
    resObj.insert("rw", m_payload.p_ctrl.c_rw);
    resObj.insert("rwType", m_payload.p_ctrl.getRWType());
    resObj.insert("isQueued", m_payload.p_ctrl.c_isQueued);
    resObj.insert("targetType", m_payload.p_ctrl.getTargetType());
    resObj.insert("slaveID", m_payload.p_ctrl.c_slaveid);

    if (m_isPrivate == true) {
        resObj.insert("isPrivate", true);
    }

    QJsonObject parseObj;
    if (m_deviceType == "M1") {
        parseObj = DM1Protocol::getInstance()->parseParams(m_payload);
    } else {
        parseObj = DMagicianProtocol::getInstance()->parseParams(m_payload);
    }

    if (parseObj.contains("cmd")) {
        resObj.insert("cmd", parseObj.value("cmd").toString());
    }
    if (parseObj.contains("params")) {
        resObj.insert("params", parseObj.value("params").toObject());
    }
    return resObj;
}

QByteArray P2Packet::getPacketData()
{
    QByteArray resData;
    QDataStream in(&resData, QIODevice::WriteOnly);
    QByteArray payload = m_payload.getPacketPayload();
    in << Head1 << Head2 << quint8(payload.size());

    resData.append(payload);

    in.device()->seek(resData.size());
    in << _getCheckSum(payload);
    return resData;
}

int P2Packet::getCommondID()
{
    return m_payload.p_cmdID;
}

int P2Packet::getSubID()
{
    int subID = 0;
    QList<int> subList{216};

    foreach(int i, subList){
        if(i == getCommondID()){
            subID = m_payload.p_params[0];
            break;
        }
    }

    return  subID;
}

void P2Packet::setPrintDetailMode(bool en)
{
    m_isDetailMode = en;
}

void P2Packet::setPrivate(bool en)
{
    m_isPrivate = en;
}

bool P2Packet::isPrivate()
{
    return m_isPrivate;
}

void P2Packet::setDeviceType(QString type)
{
    m_deviceType = type;
}

QString P2Packet::getDeviceType()
{
    return m_deviceType;
}

P2AsyncPacket::P2AsyncPacket(P2Packet &pak, quint8 resendTimesMax, quint32 commTimeout):
    QObject(nullptr),
    resendTimesMax(resendTimesMax),
    resendTimes(0)
{
    packet = pak;

    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(static_cast<int>(commTimeout));
    connect(timer, &QTimer::timeout, [&]{
        timer->start();
        emit this->timeout(resendTimes++ <= resendTimesMax);
    });
    timer->start();
}

P2AsyncPacket::~P2AsyncPacket()
{
    timer->stop();
}

void P2AsyncPacket::resetTimeout(quint32 commTimeout)
{
    timer->setInterval(static_cast<int>(commTimeout));
}

