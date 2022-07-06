#include "MagicBoxApi.h"
#include <QJsonArray>
#include "P3FrameAdapter.h"

typedef  enum {
    GetMagicBoxModeId = 192,
    GetMagicBoxNumId = 193,
    SetMagicBoxScriptModeId = 199,
    StopPointServerId = 217
}MagicBoxID;

typedef enum {
    CBGetMagicBoxModeId = 0xC0,
    CBGetMagicBoxNumId = 0xC1,
    CBSetMagicBoxScriptModeId = 0xC7,
    CBStopPointServerId = 0xD9
}CBMagicBoxID;


MagicBoxApi *MagicBoxApi::m_staticMagicBoxApi = new MagicBoxApi();

MagicBoxApi *MagicBoxApi::instance()
{
    return m_staticMagicBoxApi;
}

MagicBoxApi::MagicBoxApi()
{
    connect(this, &MagicBoxApi::sendCmd_signal, P3FrameAdapter::instance(), &P3FrameAdapter::sendCmd_slot);

    P3FrameAdapter::instance()->registerMagicBoxCB(CBGetMagicBoxModeId, GetMagicBoxMode_CB);
    P3FrameAdapter::instance()->registerMagicBoxCB(CBGetMagicBoxNumId, GetMagicBoxNum_CB);
    P3FrameAdapter::instance()->registerMagicBoxCB(CBSetMagicBoxScriptModeId, SetMagicBoxScriptMode_CB);

    P3FrameAdapter::instance()->registerMagicBoxCB(CBStopPointServerId, SetStopPointServer_CB);
    P3FrameAdapter::instance()->registerMagicBoxCB(CBStopPointServerId, SetStopPointParam_CB);
    P3FrameAdapter::instance()->registerMagicBoxCB(CBStopPointServerId, GetStopPointState_CB);
}

MagicBoxApi::~MagicBoxApi()
{
}

void MagicBoxApi::GetMagicBoxMode(uint32_t seqNum)
{
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x03;
    m_packFrame.cmdSet = 0x03;
    m_packFrame.cmdId = GetMagicBoxModeId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void MagicBoxApi::GetMagicBoxNum(uint32_t seqNum)
{
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x03;
    m_packFrame.cmdSet = 0x03;
    m_packFrame.cmdId = GetMagicBoxNumId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void MagicBoxApi::SetMagicBoxScriptMode(uint32_t seqNum, uint8_t isEnable, QString name)
{
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x03;
    m_packFrame.cmdSet = 0x03;
    m_packFrame.cmdId = SetMagicBoxScriptModeId;

    QByteArray bt = name.toUtf8();
    char *c = bt.data();
    memcpy(&m_packFrame.payloadAndCRC[0], &isEnable, sizeof (uint8_t));
    memcpy(&m_packFrame.payloadAndCRC[1], &c, strlen(c));

    uint16_t payloadLen = sizeof (uint8_t) + strlen(c);

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void MagicBoxApi::SetStopPointServer(uint32_t seqNum, int16_t PointX, int16_t PointY)
{
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x03;
    m_packFrame.cmdSet = 0x03;
    m_packFrame.cmdId = StopPointServerId;

    uint8_t subID = 10;
    uint16_t payloadLen = sizeof (uint8_t) * 5 ;
    memcpy(&m_packFrame.payloadAndCRC[0], &subID, sizeof (uint8_t));
    memcpy(&m_packFrame.payloadAndCRC[1], &PointX, sizeof (int16_t));
    memcpy(&m_packFrame.payloadAndCRC[3], &PointY, sizeof (int16_t));

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void MagicBoxApi::SetStopPointParam(uint32_t seqNum, int16_t scopeErr, int16_t stopErr)
{
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x03;
    m_packFrame.cmdSet = 0x03;
    m_packFrame.cmdId = StopPointServerId;

    uint8_t subID = 11;
    uint16_t payloadLen = sizeof (uint8_t) * 5 ;
    memcpy(&m_packFrame.payloadAndCRC[0], &subID, sizeof (uint8_t));
    memcpy(&m_packFrame.payloadAndCRC[1], &scopeErr, sizeof (int16_t));
    memcpy(&m_packFrame.payloadAndCRC[3], &stopErr, sizeof (int16_t));

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void MagicBoxApi::GetStopPointState(uint32_t seqNum)
{
    memset(&m_packFrame, 0, sizeof (DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x03;
    m_packFrame.cmdSet = 0x03;
    m_packFrame.cmdId = StopPointServerId;

    uint8_t subID = 12;
    uint16_t payloadLen = sizeof (uint8_t)  ;
    memcpy(&m_packFrame.payloadAndCRC[0], &subID, sizeof (uint8_t));

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

// 回调函数
void MagicBoxApi::GetMagicBoxMode_CB(DobotV3CmdInterCBParm packet)
{
    bool isTimeout = false;
    QJsonObject resObj;
    if(packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeout = true;
    }else {
        isTimeout = false;

        uint8_t mode;
        memcpy(&mode, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
        resObj.insert("mode", mode);
    }

    emit m_staticMagicBoxApi->receiveData_signal(isTimeout, resObj, packet.packet.seqNum);
}


void MagicBoxApi::GetMagicBoxNum_CB(DobotV3CmdInterCBParm packet)
{
    bool isTimeout = false;
    QJsonObject resObj;
    if(packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeout = true;
    }else {

        isTimeout = false;

        uint8_t num;
        memcpy(&num, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
        resObj.insert("num", num);

        for (int i=0; i<num; i++) {
            uint8_t devicenum;
            memcpy(&devicenum, &packet.packet.payloadAndCRC[i+1], sizeof (uint8_t));
            QString devicename = QString("Device%1").arg(i+1);
            resObj.insert(devicename, devicenum);
        }
    }

    emit m_staticMagicBoxApi->receiveData_signal(isTimeout, resObj, packet.packet.seqNum);
}


void MagicBoxApi::SetMagicBoxScriptMode_CB(DobotV3CmdInterCBParm packet)
{
    bool isTimeout = false;
    QJsonObject resObj;
    if(packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeout = true;
    }else {

        isTimeout = false;

        uint8_t result;
        memcpy(&result, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
        resObj.insert("result", result);

    }
    emit m_staticMagicBoxApi->receiveData_signal(isTimeout, resObj, packet.packet.seqNum);
}

void MagicBoxApi::SetStopPointServer_CB(DobotV3CmdInterCBParm packet)
{
    bool isTimeout = false;
    QJsonObject resObj;
    if(packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeout = true;
    }else {

        isTimeout = false;

        resObj.insert("result", "true");

    }
    emit m_staticMagicBoxApi->receiveData_signal(isTimeout, resObj, packet.packet.seqNum);
}

void MagicBoxApi::SetStopPointParam_CB(DobotV3CmdInterCBParm packet)
{
    bool isTimeout = false;
    QJsonObject resObj;
    if(packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeout = true;
    }else {

        isTimeout = false;

        resObj.insert("result", "true");

    }
    emit m_staticMagicBoxApi->receiveData_signal(isTimeout, resObj, packet.packet.seqNum);
}

void MagicBoxApi::GetStopPointState_CB(DobotV3CmdInterCBParm packet)
{
    bool isTimeout = false;
    QJsonObject resObj;
    if(packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeout = true;
    }else {

        isTimeout = false;
        uint8_t subID;
        bool result;
        memcpy(&subID, &packet.packet.payloadAndCRC[0], sizeof (uint8_t));
        memcpy(&result, &packet.packet.payloadAndCRC[1], sizeof (uint8_t));
        resObj.insert("result", result);

    }
    emit m_staticMagicBoxApi->receiveData_signal(isTimeout, resObj, packet.packet.seqNum);
}









