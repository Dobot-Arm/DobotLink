#ifndef MAGICIANBOXAPI_H
#define MAGICIANBOXAPI_H

#include <QObject>
#include <QJsonObject>
#include "DobotType.h"
#include "DP3Packet.h"

extern "C" {
#include "DobotV3API.h"
}

class MagicBoxApi : public QObject
{
    Q_OBJECT
public:
    static MagicBoxApi *instance();

    void GetMagicBoxMode(uint32_t seqNum);
    void GetMagicBoxNum(uint32_t seqNum);
    void SetMagicBoxScriptMode(uint32_t seqNum, uint8_t isEnable, QString name);

    void SetStopPointServer(uint32_t seqNum, int16_t PointX, int16_t PointY);
    void SetStopPointParam(uint32_t seqNum, int16_t scopeErr, int16_t stopErr);
    void GetStopPointState(uint32_t seqNum);

    //接受的回调函数
    static void GetMagicBoxMode_CB(DobotV3CmdInterCBParm packet);
    static void GetMagicBoxNum_CB(DobotV3CmdInterCBParm packet);
    static void SetMagicBoxScriptMode_CB(DobotV3CmdInterCBParm packet);

    static void SetStopPointServer_CB(DobotV3CmdInterCBParm packet);
    static void SetStopPointParam_CB(DobotV3CmdInterCBParm packet);
    static void GetStopPointState_CB(DobotV3CmdInterCBParm packet);


signals:
    void receiveData_signal(bool isTimeOut, QJsonObject jsonObj, quint32 seqNum);
    void sendCmd_signal(DobotV3Format_Packet pPacket, quint16 payloadLen, bool isAck);

private:
    static MagicBoxApi *m_staticMagicBoxApi;
    DobotV3Format_Packet m_packFrame;
    MagicBoxApi();
    ~MagicBoxApi();

};

#endif // MAGICIANBOXAPI_H
