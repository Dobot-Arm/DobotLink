#ifndef PROTOCOLTHREAD_H
#define PROTOCOLTHREAD_H

#include <QTimer>
#include <QMutex>
#include <QThread>
extern "C" {
#include "DobotV3API.h"
}


class P3FrameAdapter : public QObject
{
    Q_OBJECT
public:
    static P3FrameAdapter *instance();
    void registerGeneralCB(quint16 id, DobotV3CmdInter_CB cb);
    void registerMagicianGoCB(quint16 id, DobotV3CmdInter_CB cb);
    void registerCarCameraCB(quint16 id, DobotV3CmdInter_CB cb);
    void registerArmCameraCB(quint16 id, DobotV3CmdInter_CB cb);
    void registerMagicBoxCB(quint16 id, DobotV3CmdInter_CB cb);

private:
    explicit P3FrameAdapter(QObject *parent = nullptr);
    ~P3FrameAdapter();

public slots:
    void addRawData_slot(QByteArray data);
    void sendCmd_slot(DobotV3Format_Packet pPacket, quint16 payloadLen, bool isAck);
    void setComPars_slot(quint8 resendTimesMax, quint32 commTimeout);

signals:
    void sendRawData_signal(quint32 seqNum, QByteArray data);

private slots:
    void execTimeout_slot();
    void cntTimeout_slot();

private:
    static P3FrameAdapter *sm_instance;
    static QList<QByteArray> sm_recviveBytes;

    static uint16_t readData(uint8_t *dataArray, uint16_t dataLength);
    static void sendData(uint8_t *dataArray, uint16_t dataLength);

    QTimer *m_timerExec;
    QTimer *m_timerCNT;
    PProtocolFrame_LinkPARM m_linkPath;
    QThread *m_thread;
};

#endif // PROTOCOLTHREAD_H
