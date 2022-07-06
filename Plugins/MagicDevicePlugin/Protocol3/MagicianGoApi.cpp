#include "MagicianGoApi.h"
#include <QJsonArray>
#include "P3FrameAdapter.h"

MagicianGoApi *MagicianGoApi::m_staticMagicianGoApi = new MagicianGoApi();

MagicianGoApi *MagicianGoApi::instance()
{
    return m_staticMagicianGoApi;
}

MagicianGoApi::MagicianGoApi()
{
    connect(this, &MagicianGoApi::sendCmd_signal, P3FrameAdapter::instance(), &P3FrameAdapter::sendCmd_slot);

    P3FrameAdapter::instance()->registerMagicianGoCB(CBRunningStateId, RunningState_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBRunningModeId, RunningMode_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetBaseModeId, SetBaseMode_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBMoveSpeedId, MoveSpeed_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBBaseSpeedRatioId, BaseSpeedRatio_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetMoveSpeedDirectId, SetMoveSpeedDirect_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetRotateId, SetRotate_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetMoveDistId, SetMoveDist_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetMovePosId, SetMovePos_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetArcRadId, SetArcRad_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetArcCentId, SetArcCent_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetCoordClosedLoopId, SetCoordClosedLoop_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetIncrementClosedLoopId, SetIncrementClosedLoop_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetLightRGBId, SetLightRGB_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetLightPromptId, SetLightPrompt_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetBuzzerSoundId, SetBuzzerSound_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetOriginPointId, SetOriginPoint_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBGetUltrasoundDataId, GetUltrasoundData_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBGetSwitchStateId, GetSwitchState_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBGetLineSensorDataId, GetLineSensorData_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetSpeedometerId, SetSpeedometer_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBGetSpeedometerId, GetSpeedometer_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBGetStallProtectionId, GetStallProtection_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBGetOffGroundId, GetOffGround_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBGetBatteryVoltageId, GetBatteryVoltage_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBGetImuAngleId, GetImuAngle_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBGetImuSpeedId, GetImuSpeed_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBAlarmInfoId, AlarmInfo_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetTraceAutoId, SetTraceAuto_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetTraceLineInfoId, SetTraceLineInfo_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetTraceSpeedId, SetTraceSpeed_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBLineSensorStateId, LineSensorState_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetLineSensorAdjustId, SetLineSensorAdjust_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetLineSensorDefaultId, SetLineSensorDefault_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBGetLineSensorDataId, GetLineSensorData_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetTracePidId, SetTracePid_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetCmdQueueStartId, SetCmdQueueStart_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetCmdQueueStopId, SetCmdQueueStop_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetCmdQueueForcelyStopId, SetCmdQueueForcelyStop_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBCleanCmdQueueId, CleanCmdQueue_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBGetCmdQueueCurrentIndexId, GetQueuedCmdCurrentIndex_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBGetCmdQueueAvailableSpaceId, GetCmdQueueAvailableSpace_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBSetMoveSpeedTimeId, SetMoveSpeedTime_CB);
    P3FrameAdapter::instance()->registerMagicBoxCB(CBSetTraceLoopId, SetTraceLoop_CB);
    P3FrameAdapter::instance()->registerMagicianGoCB(CBGetImgToArmXYId, GetImgToArmXY_CB);
}

MagicianGoApi::~MagicianGoApi() {
}

void MagicianGoApi::SetRunningState(uint32_t seqNum, uint8_t runningState){
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetRunningStateId;
    uint16_t payloadLen = 1;
    memcpy(&m_packFrame.payloadAndCRC[0], &runningState, sizeof(uint8_t));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetRunningState(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetRunningStateId;
    uint16_t payloadLen = 0;
    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void MagicianGoApi::SetRunningMode(uint32_t seqNum, uint8_t runningMode){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetRunningModeId;
    uint16_t payloadLen = 1;
    memcpy(&m_packFrame.payloadAndCRC[0], &runningMode, sizeof(uint8_t));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetRunningMode(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetRunningModeId;
    uint16_t payloadLen = 0;
    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetBaseMode(uint32_t seqNum, uint8_t baseMode){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetBaseModeId;
    uint16_t payloadLen = 1;
    memcpy(&m_packFrame.payloadAndCRC[0], &baseMode, sizeof(uint8_t));

    emit sendCmd_signal(m_packFrame, payloadLen,true);

}

void MagicianGoApi::SetBaseSpeedRatio(uint32_t seqNum, uint8_t speedRatio)
{
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetBaseSpeedRatioId;
    uint16_t payloadLen = 1;
    memcpy(&m_packFrame.payloadAndCRC[0], &speedRatio, sizeof(uint8_t));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetBaseSpeedRatio(uint32_t seqNum)
{
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetBaseSpeedRatioId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetMoveSpeedTime(uint32_t seqNum, bool isAck, float time, float x, float y, float r) {
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetMoveSpeedTimeId;
    uint16_t payloadLen = 16;
    memcpy(&m_packFrame.payloadAndCRC[0], &time, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[4], &x, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[8], &y, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[12], &r, sizeof(float));
    emit sendCmd_signal(m_packFrame, payloadLen, isAck);
}

void MagicianGoApi::SetMoveSpeedDirect(uint32_t seqNum, float dir, float speed){
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetMoveSpeedDirectId;
    uint16_t payloadLen = 16;
    memcpy(&m_packFrame.payloadAndCRC[0], &dir, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[4], &speed, sizeof(float));

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void MagicianGoApi::SetMoveSpeed(uint32_t seqNum, float x, float y, float r){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetMoveSpeedId;
    uint16_t payloadLen = 12;
    memcpy(&m_packFrame.payloadAndCRC[0], &x, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[4], &y, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[8], &r, sizeof(float));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetMoveSpeed(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetMoveSpeedId;
    uint16_t payloadLen = 0;
    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetRotate(uint32_t seqNum, float r, float Vr){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 1;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetRotateId;
    uint16_t payloadLen = 8;
    memcpy(&m_packFrame.payloadAndCRC[0], &r, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[4], &Vr, sizeof(float));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetMoveDist(uint32_t seqNum, float x, float y, float Vx, float Vy){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 1;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetMoveDistId;
    uint16_t payloadLen = 16;
    memcpy(&m_packFrame.payloadAndCRC[0], &x, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[4], &y, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[8], &Vx, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[12], &Vy, sizeof(float));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetMovePos(uint32_t seqNum, float x, float y, float s)
{
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 1;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetMovePosId;
    uint16_t payloadLen = 12;
    memcpy(&m_packFrame.payloadAndCRC[0], &x, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[4], &y, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[8], &s, sizeof(float));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetArcRad(uint32_t seqNum, float velocity, float radius, float angle, int mode){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 1;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetArcRadId;
    uint16_t payloadLen = 16;
    memcpy(&m_packFrame.payloadAndCRC[0], &velocity, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[4], &radius, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[8], &angle, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[12], &mode, sizeof(int));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetArcCent(uint32_t seqNum, float velocity, float x, float y, float angle, int mode){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 1;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetArcCentId;
    uint16_t payloadLen = 20;
    memcpy(&m_packFrame.payloadAndCRC[0], &velocity, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[4], &x, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[8], &y, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[12], &angle, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[16], &mode, sizeof(int));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetCoordClosedLoop(uint32_t seqNum, uint8_t isEnable, float angle){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetCoordClosedLoopId;
    uint16_t payloadLen = 5;
    memcpy(&m_packFrame.payloadAndCRC[0], &isEnable, sizeof(uint8_t));
    memcpy(&m_packFrame.payloadAndCRC[1], &angle, sizeof(float));

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void MagicianGoApi::SetIncrementClosedLoop(uint32_t seqNum, float x, float y, float angle){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 1;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetIncrementClosedLoopId;
    uint16_t payloadLen = 12;
    memcpy(&m_packFrame.payloadAndCRC[0], &x, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[4], &y, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[8], &angle, sizeof(float));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetLightRGB(uint32_t seqNum, uint16_t number, uint8_t effect, uint8_t r, uint8_t g, uint8_t b, float cycle, int counts){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetLightRGBId;
    uint16_t payloadLen = 14;
    memcpy(&m_packFrame.payloadAndCRC[0], &number, sizeof(uint16_t));
    memcpy(&m_packFrame.payloadAndCRC[2], &effect, sizeof(uint8_t));
    memcpy(&m_packFrame.payloadAndCRC[3], &r, sizeof(uint8_t));
    memcpy(&m_packFrame.payloadAndCRC[4], &g, sizeof(uint8_t));
    memcpy(&m_packFrame.payloadAndCRC[5], &b, sizeof(uint8_t));
    memcpy(&m_packFrame.payloadAndCRC[6], &cycle, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[10], &counts, sizeof(int));

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void MagicianGoApi::SetLightPrompt(uint32_t seqNum, int index){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetLightPromptId;
    uint16_t payloadLen = 1;
    memcpy(&m_packFrame.payloadAndCRC[0], &index, sizeof(uint8_t));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetBuzzerSound(uint32_t seqNum, uint8_t index, uint8_t tone, float beat){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetBuzzerSoundId;
    uint16_t payloadLen = 6;
    memcpy(&m_packFrame.payloadAndCRC[0], &index, sizeof(uint8_t));
    memcpy(&m_packFrame.payloadAndCRC[1], &tone, sizeof(uint8_t));
    memcpy(&m_packFrame.payloadAndCRC[2], &beat, sizeof(float));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetOriginPoint(uint32_t seqNum, uint8_t enable){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetOriginPointId;
    uint16_t payloadLen = 1;
    memcpy(&m_packFrame.payloadAndCRC[0], &enable, sizeof(uint8_t));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetUltrasoundData(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetUltrasoundDataId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetSwitchState(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetSwitchStateId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetSpeedometer(uint32_t seqNum, float x, float y, float yaw)
{
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetSpeedometerId;
    memcpy(&m_packFrame.payloadAndCRC[0], &x, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[4], &y, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[8], &yaw, sizeof(float));

    emit sendCmd_signal(m_packFrame, 3 * sizeof(float), true);
}

void MagicianGoApi::GetSpeedometer(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetSpeedometerId;
    uint16_t payloadLen = 0;
    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetStallProtection(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetStallProtectionId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetOffGround(uint32_t seqNum)
{
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetOffGroundId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetBatteryVoltage(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetBatteryVoltageId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetImuAngle(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetImuAngleId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetImuSpeed(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetImuSpeedId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetAlarmInfo(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetAlarmInfoId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::CleanAlarmInfo(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = CleanAlarmInfoId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetTraceAuto(uint32_t seqNum, uint8_t isTrace){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetTraceAutoId;
    uint16_t payloadLen = 1;
    memcpy(&m_packFrame.payloadAndCRC[0], &isTrace, sizeof(uint8_t));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetTraceLineInfo(uint32_t seqNum, uint8_t lineInfo){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetTraceLineInfoId;
    uint16_t payloadLen = 1;
    memcpy(&m_packFrame.payloadAndCRC[0], &lineInfo, sizeof(uint8_t));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetTraceSpeed(uint32_t seqNum, float speed){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetTraceSpeedId;
    uint16_t payloadLen = sizeof(float);
    memcpy(&m_packFrame.payloadAndCRC[0], &speed, sizeof(float));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetTraceLoop(uint32_t seqNum, bool enable){
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x03;
    m_packFrame.cmdSet = 0x03;
    m_packFrame.cmdId = SetTraceLoopId;
    quint8 subid = 0;
    uint16_t payloadLen = 2;
    memcpy(&m_packFrame.payloadAndCRC[0], &subid, sizeof(quint8));
    memcpy(&m_packFrame.payloadAndCRC[1], &enable, sizeof(bool));

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void MagicianGoApi::SetLineSensorState(uint32_t seqNum, uint8_t frontIsEnable, uint8_t backIsEnable){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetLineSensorStateId;
    uint16_t payloadLen = 2;
    memcpy(&m_packFrame.payloadAndCRC[0], &frontIsEnable, sizeof(uint8_t));
    memcpy(&m_packFrame.payloadAndCRC[1], &backIsEnable, sizeof(uint8_t));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetLineSensorState(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetLineSensorStateId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetLineSensorAdjust(uint32_t seqNum, uint8_t deviceNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetLineSensorAdjustId;
    uint16_t payloadLen = 1;
    memcpy(&m_packFrame.payloadAndCRC[0], &deviceNum, sizeof(uint8_t));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetLineSensorDefault(uint32_t seqNum, uint8_t deviceNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetLineSensorDefaultId;
    uint16_t payloadLen = 1;
    memcpy(&m_packFrame.payloadAndCRC[0], &deviceNum, sizeof(uint8_t));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetLineSensorData(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetLineSensorDataId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetTracePid(uint32_t seqNum, float p, float i, float d){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetTracePidId;
    uint16_t payloadLen = 12;
    memcpy(&m_packFrame.payloadAndCRC[0], &p, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[4], &i, sizeof(float));
    memcpy(&m_packFrame.payloadAndCRC[8], &d, sizeof(float));

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetCmdQueueStart(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetCmdQueueStartId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetCmdQueueStop(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetCmdQueueStopId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::SetCmdQueueForcelyStop(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = SetCmdQueueForcelyStopId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::CleanCmdQueue(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = CleanCmdQueueId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetQueuedCmdCurrentIndex(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetCmdQueueCurrentIndexId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetCmdQueueAvailableSpace(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetCmdQueueAvailableSpaceId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen,true);
}

void MagicianGoApi::GetImgToArmXY(uint32_t seqNum, float imgX, float imgY, uint8_t need_tranxy, uint8_t suck_apriltag){
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 1;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x05;
    m_packFrame.cmdId = GetImgToArmXYId;
    uint16_t payloadLen = 11;
    uint8_t subID = 13;
    memcpy(&m_packFrame.payloadAndCRC[0], &subID, sizeof (uint8_t));
    memcpy(&m_packFrame.payloadAndCRC[1], &imgX, sizeof (float));
    memcpy(&m_packFrame.payloadAndCRC[5], &imgY, sizeof (float));
    memcpy(&m_packFrame.payloadAndCRC[9], &need_tranxy, sizeof (uint8_t));
    memcpy(&m_packFrame.payloadAndCRC[10], &suck_apriltag, sizeof (uint8_t));

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

//回调函数
void MagicianGoApi::RunningState_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        if(packet.packet.rwType == 0) {
            uint8_t runningState;
            memcpy(&runningState, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
            resObj.insert("runningState", runningState);
        }else if(packet.packet.rwType == 1) {
            resObj.insert("result", "true");
        }
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::RunningMode_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        if(packet.packet.rwType == 0) {
            uint8_t runningMode;
            memcpy(&runningMode, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
            resObj.insert("runningMode", runningMode);
        }else if(packet.packet.rwType == 1) {
            resObj.insert("result", "true");
        }
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetBaseMode_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::BaseSpeedRatio_CB(DobotV3CmdInterCBParm packet)
{
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        if(packet.packet.rwType == 0) {
            uint8_t speedRatio;
            memcpy(&speedRatio, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
            resObj.insert("speedRatio", speedRatio);
        }else if(packet.packet.rwType == 1) {
            resObj.insert("result", "true");
        }
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetMoveSpeedTime_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } if (packet.error == DobotV3CmdInter_NOACK) {
        isTimeOut = false;
        resObj.insert("result", "true");
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }
    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetMoveSpeedDirect_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::MoveSpeed_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        if(packet.packet.rwType == 0) {
            float x;
            float y;
            float r;
            memcpy(&x, &packet.packet.payloadAndCRC[0], sizeof(float));
            memcpy(&y, &packet.packet.payloadAndCRC[4], sizeof(float));
            memcpy(&r, &packet.packet.payloadAndCRC[8], sizeof(float));
            resObj.insert("x", static_cast<double>(x));
            resObj.insert("y", static_cast<double>(y));
            resObj.insert("r", static_cast<double>(r));
        }else if(packet.packet.rwType == 1) {
            resObj.insert("result", "true");
        }
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetRotate_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        uint64_t index;
        memcpy(&index, &packet.packet.payloadAndCRC[0], sizeof (uint64_t));

        resObj.insert("index", static_cast<double>(index));
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetMoveDist_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        uint64_t index;
        memcpy(&index, &packet.packet.payloadAndCRC[0], sizeof (uint64_t));

        resObj.insert("index", static_cast<double>(index));
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetMovePos_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        uint64_t index;
        memcpy(&index, &packet.packet.payloadAndCRC[0], sizeof (uint64_t));

        resObj.insert("index", static_cast<double>(index));
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetArcRad_CB(DobotV3CmdInterCBParm packet)
{
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        uint64_t index;
        memcpy(&index, &packet.packet.payloadAndCRC[0], sizeof (uint64_t));

        resObj.insert("index", static_cast<double>(index));
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetArcCent_CB(DobotV3CmdInterCBParm packet)
{
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        uint64_t index;
        memcpy(&index, &packet.packet.payloadAndCRC[0], sizeof (uint64_t));

        resObj.insert("index", static_cast<double>(index));
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetCoordClosedLoop_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetIncrementClosedLoop_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        uint64_t index;
        memcpy(&index, &packet.packet.payloadAndCRC[0], sizeof (uint64_t));

        resObj.insert("index", static_cast<double>(index));
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetLightRGB_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetLightPrompt_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetBuzzerSound_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetOriginPoint_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::GetUltrasoundData_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        uint8_t front;
        uint8_t back;
        uint8_t left;
        uint8_t right;
        memcpy(&front, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        memcpy(&back, &packet.packet.payloadAndCRC[1], sizeof(uint8_t));
        memcpy(&left, &packet.packet.payloadAndCRC[2], sizeof(uint8_t));
        memcpy(&right, &packet.packet.payloadAndCRC[3], sizeof(uint8_t));
        resObj.insert("front", front);
        resObj.insert("back", back);
        resObj.insert("left", left);
        resObj.insert("right", right);
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::GetSwitchState_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        uint8_t front_left;
        uint8_t back_right;
        uint8_t rear_left;
        uint8_t rear_right;
        memcpy(&front_left, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        memcpy(&back_right, &packet.packet.payloadAndCRC[1], sizeof(uint8_t));
        memcpy(&rear_left, &packet.packet.payloadAndCRC[2], sizeof(uint8_t));
        memcpy(&rear_right, &packet.packet.payloadAndCRC[3], sizeof(uint8_t));
        resObj.insert("front_left", front_left);
        resObj.insert("back_right", back_right);
        resObj.insert("rear_left", rear_left);
        resObj.insert("rear_right", rear_right);
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetSpeedometer_CB(DobotV3CmdInterCBParm packet)
{
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::GetSpeedometer_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        float x;
        float y;
        float yaw;
        memcpy(&x, &packet.packet.payloadAndCRC[0], sizeof(float));
        memcpy(&y, &packet.packet.payloadAndCRC[4], sizeof(float));
        memcpy(&yaw, &packet.packet.payloadAndCRC[8], sizeof(float));
        resObj.insert("x", static_cast<double>(x));
        resObj.insert("y", static_cast<double>(y));
        resObj.insert("yaw", static_cast<double>(yaw));
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::GetStallProtection_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        uint8_t isHappened;
        memcpy(&isHappened, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        resObj.insert("isHappened", isHappened);
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::GetOffGround_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        uint8_t isHappened;
        memcpy(&isHappened, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        resObj.insert("isHappened", isHappened);
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::GetBatteryVoltage_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        float powerVoltage;
        float powerPercentage;
        uint8_t powerSourceStatus;

        memcpy(&powerVoltage, &packet.packet.payloadAndCRC[0], sizeof(float));
        memcpy(&powerPercentage, &packet.packet.payloadAndCRC[4], sizeof(float));
        memcpy(&powerSourceStatus, &packet.packet.payloadAndCRC[8], sizeof (uint8_t));
        resObj.insert("powerVoltage", static_cast<double>(powerVoltage));
        resObj.insert("powerPercentage", static_cast<double>(powerPercentage));
        resObj.insert("powerSourceStatus", powerSourceStatus);
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::GetImuAngle_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        float yaw;
        float roll;
        float pitch;
        memcpy(&yaw, &packet.packet.payloadAndCRC[0], sizeof(float));
        memcpy(&roll, &packet.packet.payloadAndCRC[4], sizeof(float));
        memcpy(&pitch, &packet.packet.payloadAndCRC[8], sizeof(float));
        resObj.insert("yaw", static_cast<double>(yaw));
        resObj.insert("roll", static_cast<double>(roll));
        resObj.insert("pitch", static_cast<double>(pitch));
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::GetImuSpeed_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        float ax, ay, az, gx, gy, gz;
        memcpy(&ax, &packet.packet.payloadAndCRC[0], sizeof(float));
        memcpy(&ay, &packet.packet.payloadAndCRC[4], sizeof(float));
        memcpy(&az, &packet.packet.payloadAndCRC[8], sizeof(float));
        memcpy(&gx, &packet.packet.payloadAndCRC[12], sizeof(float));
        memcpy(&gy, &packet.packet.payloadAndCRC[16], sizeof(float));
        memcpy(&gz, &packet.packet.payloadAndCRC[20], sizeof(float));
        resObj.insert("ax", static_cast<double>(ax));
        resObj.insert("ay", static_cast<double>(ay));
        resObj.insert("az", static_cast<double>(az));
        resObj.insert("gx", static_cast<double>(gx));
        resObj.insert("gy", static_cast<double>(gy));
        resObj.insert("gz", static_cast<double>(gz));
    }


    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::AlarmInfo_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;

        if (packet.packet.rwType == 0) {
            uint8_t warning[16];
            QJsonArray warningJson;
            for (int i = 0; i < 16; i++) {
                memcpy(&warning[i], &packet.packet.payloadAndCRC[i], sizeof(uint8_t));
                warningJson.insert(i, warning[i]);
            }
            resObj.insert("warning", warningJson);
        } else if (packet.packet.rwType == 1){
            resObj.insert("result", "true");
        }
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetTraceAuto_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetTraceLineInfo_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetTraceSpeed_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::LineSensorState_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;

        if(packet.packet.rwType == 0) {
            uint8_t frontIsEnable;
            uint8_t backIsEnable;
            memcpy(&frontIsEnable, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
            memcpy(&backIsEnable, &packet.packet.payloadAndCRC[1], sizeof(uint8_t));
            resObj.insert("frontIsEnable", frontIsEnable);
            resObj.insert("backIsEnable", backIsEnable);
        }else if(packet.packet.rwType == 1) {
            resObj.insert("result", "true");
        }
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}


void MagicianGoApi::SetLineSensorAdjust_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetLineSensorDefault_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::GetLineSensorData_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;

        uint16_t front;
        uint16_t back;
        memcpy(&front, &packet.packet.payloadAndCRC[0], sizeof(uint16_t));
        memcpy(&back, &packet.packet.payloadAndCRC[2], sizeof(uint16_t));
        resObj.insert("front", front);
        resObj.insert("back", back);
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetCmdQueueStart_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetCmdQueueStop_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetCmdQueueForcelyStop_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::CleanCmdQueue_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::GetQueuedCmdCurrentIndex_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        uint64_t queueCmdCurrentIndex;
        memcpy(&queueCmdCurrentIndex, &packet.packet.payloadAndCRC[0], sizeof(uint64_t));
        resObj.insert("queueCmdCurrentIndex", static_cast<double>(queueCmdCurrentIndex));
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::GetCmdQueueAvailableSpace_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        uint64_t space;
        memcpy(&space, &packet.packet.payloadAndCRC[0], sizeof(uint64_t));
        resObj.insert("space", static_cast<double>(space));
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetTracePid_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::SetTraceLoop_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {

        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void MagicianGoApi::GetImgToArmXY_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {

        isTimeOut = true;
    } else {

        isTimeOut = false;

        float armX;
        float armY;
        uint8_t okflag;
        memcpy(&armX, &packet.packet.payloadAndCRC[1], sizeof(float));
        memcpy(&armY, &packet.packet.payloadAndCRC[5], sizeof(float));
        memcpy(&okflag, &packet.packet.payloadAndCRC[9], sizeof (uint8_t));
        resObj.insert("armX", static_cast<double>(armX));
        resObj.insert("armY", static_cast<double>(armY));
        resObj.insert("okflag", okflag);
    }

    emit m_staticMagicianGoApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}






