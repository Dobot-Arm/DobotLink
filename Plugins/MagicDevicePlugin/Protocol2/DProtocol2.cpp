#include "DProtocol2.h"


#include <QDebug>
#include <QJsonObject>
#include "DError/DError.h"

#ifndef __wasm__
#define DEFAULT_COMM_TIMEOUT (400)       /* 超时时间设置 */
#else
#define DEFAULT_COMM_TIMEOUT (8000)       /*chromebook性能堪忧，通信太慢，所以超时设置长一点儿,前端有接口SetCommTimeout可以设置*/
#endif
#define DEFAULT_RESEND_TIMES_MAX (3)    /* 重发次数设置 */

DProtocol2::DProtocol2(QObject *parent):
    DProtocolInterface(parent),
    m_resendTimesMax(DEFAULT_RESEND_TIMES_MAX),
    m_commTimeout(DEFAULT_COMM_TIMEOUT)
{
    /* Timer - period */
    m_timerSendWorker = new QTimer(this);
    m_timerSendWorker->setSingleShot(true);
    m_timerSendWorker->setInterval(1);
    connect(m_timerSendWorker, &QTimer::timeout, this, &DProtocol2::_PeriodicTaskAsync_slot);
    m_timerSendWorker->start();
}

DProtocol2::~DProtocol2()
{
    stop();
}

void DProtocol2::start() {

    m_timerSendWorker->start();
}

void DProtocol2::stop()
{
    m_timerSendWorker->stop();

    QHash<QPair<int,int>, P2AsyncPacket*>::Iterator iter = m_sendingPackets.begin();
    while(iter != m_sendingPackets.end()) {
        delete iter.value();
        iter++;
    }
    m_sendingPackets.clear();
}

void DProtocol2::setCommPars(quint8 resendTimesMax, quint32 commTimeout)
{
    this->m_resendTimesMax = resendTimesMax;
    this->m_commTimeout = commTimeout;

    QHash<QPair<int,int>, P2AsyncPacket*>::Iterator iter = m_sendingPackets.begin();
    while(iter != m_sendingPackets.end()) {
        iter.value()->resetTimeout(commTimeout);
        iter++;
    }
}

quint8 DProtocol2::_CalcCheckSum(const QByteArray &payload)
{
    quint8 sum = 0;
    for (int i = 0; i < payload.size(); ++i) {
        int t = payload.at(i);
        sum += static_cast<quint8>(t);
    }
    return static_cast<quint8>(256 - sum);
}


bool DProtocol2::sendData(quint64 id, const QJsonObject &obj, bool needReply)
{
    if (!m_timerSendWorker->isActive()) {
        emit errorOccurred_signal(ERROR_DEVICE_DISCONNECT_FALID, id);
        return false;
    }

    P2Packet packet(id);

    if (!packet.setPacket(obj)) {
        emit errorOccurred_signal(ERROR_PACKET_JSON, id);
        return false;
    }

    if (needReply) {
        if (m_sendingList.count() >= 1000) {
            emit errorOccurred_signal(ERROR_COMMUNICATION_BUFFER_FULL, id);
            return false;
        } else {
            m_sendingList.append(packet);
        }
        // qDebug() << "p2 sendlist" << m_sendingList.size();
    } else {
        // todo: 正在发送的列表中包含相同ID会出错
        P2Packet pack(id);
        pack.setPacket(obj);
        emit sendRawData_signal(pack.getPacketData());
        emit recviceData_signal(packet.getMessage());
    }

    return true;
}

HandleBytesState DProtocol2::handleRawData(QByteArray &rawData)
{
    if (rawData.size() < 5) {
        return TOO_SHORT;
    }

    quint8 len = static_cast<quint8>(rawData[2]);
    if (rawData.size() < 4 + len) {
        return TOO_SHORT;
    }

    quint8 checkSum = static_cast<quint8>(rawData[3 + len]);
    if (checkSum != _CalcCheckSum(rawData.mid(3, len))) return CHECKSUM_ERR;

    QByteArray data = rawData.left(4 + len);
    rawData.remove(0, 4 + len);
    P2Packet receivePacket;
    receivePacket.setPacket(data);

    if (m_sendingPackets.contains(QPair<int,int>(receivePacket.getCommondID(), receivePacket.getSubID()))) {
        // 这是一个回包
        P2AsyncPacket *packSend = m_sendingPackets.take(QPair<int,int>(receivePacket.getCommondID(), receivePacket.getSubID()));

        receivePacket.setPacketID(packSend->packet.getPacketID());
        receivePacket.setPrivate(packSend->packet.isPrivate());
        receivePacket.setDeviceType(packSend->packet.getDeviceType());

        delete packSend;
        emit recviceData_signal(receivePacket.getMessage());
    } else {
        // 这是一个通知
        // 暂时协议里面没有通知，是下位机多发出的数据，直接丢掉
        qDebug() << "abandon";
    }
    return OK;
}


/* Periodic Task timer (interval=1) */
void DProtocol2::_PeriodicTaskAsync_slot()
{
    // 发送新的数据包
    QQueue<P2Packet>::Iterator iterPack = m_sendingList.begin();
    while(iterPack != m_sendingList.end()) {
        int commId(iterPack->getCommondID());
        int subId(iterPack->getSubID());

        if (m_sendingPackets.contains(QPair<int,int>(commId, subId))) {
            iterPack++;
            continue;
        }

        emit sendRawData_signal(iterPack->getPacketData());

        P2AsyncPacket *pack = new P2AsyncPacket(*iterPack, m_resendTimesMax, m_commTimeout);
        connect(pack, &P2AsyncPacket::timeout, [=](bool isVaild) {
            if (isVaild) {
                qDebug() << "P2 resend" << commId;
                emit sendRawData_signal(pack->packet.getPacketData());
            } else {
                qDebug() << "P2 send fail" << commId;
                emit errorOccurred_signal(ERROR_COMMUNICATION_TIMEOUT, pack->packet.getPacketID());
                delete pack;
                m_sendingPackets.take(QPair<int,int>(commId, subId));
            }
        });

        m_sendingPackets.insert(QPair<int,int>(commId, subId), pack);
        // qDebug() << "p2 sendpacks" << m_sendingPackets.size();
        iterPack = m_sendingList.erase(iterPack);
    }

    m_timerSendWorker->start();
}
