#ifndef PROTOCOL2_H
#define PROTOCOL2_H


#include <QObject>
#include <QQueue>
#include <QHash>
#include <QTimer>
#include <QJsonObject>
#include "DP2Packet.h"
#include "DProtocolInterface.h"

class DProtocol2: public DProtocolInterface
{
    Q_OBJECT
public:
    explicit DProtocol2(QObject *parent = nullptr);
    ~DProtocol2();

    bool sendData(quint64 id, const QJsonObject &obj, bool needReply=true) override;
    HandleBytesState handleRawData(QByteArray &rawData) override;
    void start() override;
    void stop() override;
    void setCommPars(quint8 resendTimesMax, quint32 commTimeout) override;

private:
    QQueue<P2Packet> m_sendingList;

    /* Timer */
    QTimer *m_timerSendWorker;

    /* time over */
    quint8 m_resendTimesMax;
    quint32 m_commTimeout;

    QHash<QPair<int,int>, P2AsyncPacket*> m_sendingPackets;  //QPair<int,int>对应commid,subid

    quint8 _CalcCheckSum(const QByteArray &payload);

private slots:
    void _PeriodicTaskAsync_slot();
};

#endif // PROTOCOL2_H
