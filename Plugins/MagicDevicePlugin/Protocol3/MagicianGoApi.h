#ifndef MOOZAPI_H
#define MOOZAPI_H

#include <QObject>
#include <QJsonObject>
#include "DobotType.h"
#include "DP3Packet.h"
extern "C" {
#include "DobotV3API.h"
}

Q_DECLARE_METATYPE(QJsonObject);

class MagicianGoApi : public QObject
{
    Q_OBJECT

public:

    static MagicianGoApi *instance();
    //处理发送的数据
    /* id: 0 */
    void SetRunningState(uint32_t seqNum, uint8_t runningState);
    void GetRunningState(uint32_t seqNum);

    /* id: 1 */
    void SetRunningMode(uint32_t seqNum, uint8_t runningMode);
    void GetRunningMode(uint32_t seqNum);

    /* id: 3 */
    void SetBaseMode(uint32_t seqNum, uint8_t baseMode);

    /* id: 4 */
    void SetBaseSpeedRatio(uint32_t seqNum, uint8_t speedRatio);
    void GetBaseSpeedRatio(uint32_t seqNum);

    void SetMoveSpeedTime(uint32_t seqNum, bool isAck,float time, float x, float y, float r);

    /* id: 9 */
    void SetMoveSpeedDirect(uint32_t seqNum, float dir, float speed);

    /* id: 10 */
    void SetMoveSpeed(uint32_t seqNum, float x, float y, float r);
    void GetMoveSpeed(uint32_t seqNum);

    /* id: 11 */
    void SetRotate(uint32_t seqNum, float r, float Vr);

    /* id: 12 */
    void SetMoveDist(uint32_t seqNum , float x, float y, float Vx, float Vy);

    /* id: 13 */
    void SetMovePos(uint32_t seqNum, float x, float y, float s);

    /* id: 15 */
    void SetArcRad(uint32_t seqNum , float velocity, float radius, float angle, int mode);

    /* id: 16 */
    void SetArcCent(uint32_t seqNum, float velocity, float x, float y, float angle, int mode);

    /* id: 17 */
    void SetCoordClosedLoop(uint32_t seqNum, uint8_t isEnable, float angle);

    /* id: 18 */
    void SetIncrementClosedLoop(uint32_t seqNum, float x, float y, float angle);

    /* id: 20 */
    void SetLightRGB(uint32_t seqNum, uint16_t number, uint8_t effect, uint8_t r, uint8_t g, uint8_t b, float cycle, int counts);

    /* id: 21 */
    void SetLightPrompt(uint32_t seqNum, int index);

    /* id: 25 */
    void SetBuzzerSound(uint32_t seqNum, uint8_t index, uint8_t tone, float beat);

    /* id: 26 */
    void SetOriginPoint(uint32_t seqNum, uint8_t enable);

    /* id: 30 */
    void GetUltrasoundData(uint32_t seqNum);

    /* id: 31 n*/
    void GetSwitchState(uint32_t seqNum);

    /* id: 33 add setfuction  */
    void SetSpeedometer(uint32_t seqNum, float x, float y, float yaw);
    void GetSpeedometer(uint32_t seqNum);

    /* id: 34 */
    void GetStallProtection(uint32_t seqNum);

    /* id: 35 */
    void GetOffGround(uint32_t seqNum);

    /* id: 36 */
    void GetBatteryVoltage(uint32_t seqNum);

    /* id: 37 */
    void GetImuAngle(uint32_t seqNum);

    /* id: 38 */
    void GetImuSpeed(uint32_t seqNum);

    /* id: 40 */
    void GetAlarmInfo(uint32_t seqNum);
    void CleanAlarmInfo(uint32_t seqNum);

    /* id: 50 */
    void SetTraceAuto(uint32_t seqNum, uint8_t isTrace);

    /* id: 51 */
    void SetTraceLineInfo(uint32_t seqNum, uint8_t lineInfo);

    /* id: 52 */
    void SetTraceSpeed(uint32_t seqNum, float speed);

    /* id: 53 */
    void SetLineSensorState(uint32_t seqNum, uint8_t frontIsEnable, uint8_t backIsEnable);
    void GetLineSensorState(uint32_t seqNum);

    /* id: 54 */
    void SetLineSensorAdjust(uint32_t seqNum, uint8_t deviceNum);

    /* id: 55 */
    void SetLineSensorDefault(uint32_t seqNum, uint8_t deviceNum);

    /* id: 56 */
    void GetLineSensorData(uint32_t seqNum);

    /* id: 57 */
    void SetTracePid(uint32_t seqNum, float p, float i, float d);

    /* id: 210 */
    void SetTraceLoop(uint32_t seqNum, bool enable);

    /* id: 217 */
    void GetImgToArmXY(uint32_t seqNum, float imgX, float imgY, uint8_t need_tranxy, uint8_t suck_apriltag);

    /* id: 240 */
    void SetCmdQueueStart(uint32_t seqNum);

    /* id: 241 */
    void SetCmdQueueStop(uint32_t seqNum);

    /* id: 242 */
    void SetCmdQueueForcelyStop(uint32_t seqNum);

    /* id: 245 */
    void CleanCmdQueue(uint32_t seqNum);

    /* id: 246 */
    void GetQueuedCmdCurrentIndex(uint32_t seqNum);

    /* id: 247 */
    void GetCmdQueueAvailableSpace(uint32_t seqNum);

    //接受的回调函数
    static void RunningState_CB(DobotV3CmdInterCBParm packet);
    static void RunningMode_CB(DobotV3CmdInterCBParm packet);
    static void SetBaseMode_CB(DobotV3CmdInterCBParm packet);
    static void BaseSpeedRatio_CB(DobotV3CmdInterCBParm packet);
    static void MoveSpeed_CB(DobotV3CmdInterCBParm packet);
    static void SetRotate_CB(DobotV3CmdInterCBParm packet);
    static void SetMoveDist_CB(DobotV3CmdInterCBParm packet);
    static void SetMovePos_CB(DobotV3CmdInterCBParm packet);
    static void SetArcRad_CB(DobotV3CmdInterCBParm packet);
    static void SetArcCent_CB(DobotV3CmdInterCBParm packet);
    static void SetLightRGB_CB(DobotV3CmdInterCBParm packet);
    static void SetLightPrompt_CB(DobotV3CmdInterCBParm packet);
    static void SetBuzzerSound_CB(DobotV3CmdInterCBParm packet);
    static void SetOriginPoint_CB(DobotV3CmdInterCBParm packet);
    static void GetUltrasoundData_CB(DobotV3CmdInterCBParm packet);
    static void GetSwitchState_CB(DobotV3CmdInterCBParm packet);
    static void SetSpeedometer_CB(DobotV3CmdInterCBParm packet);
    static void GetSpeedometer_CB(DobotV3CmdInterCBParm packet);
    static void GetStallProtection_CB(DobotV3CmdInterCBParm packet);
    static void GetOffGround_CB(DobotV3CmdInterCBParm packet);
    static void GetBatteryVoltage_CB(DobotV3CmdInterCBParm packet);
    static void GetImuAngle_CB(DobotV3CmdInterCBParm packet);
    static void AlarmInfo_CB(DobotV3CmdInterCBParm packet);
    static void SetTraceAuto_CB(DobotV3CmdInterCBParm packet);
    static void SetTraceLineInfo_CB(DobotV3CmdInterCBParm packet);
    static void SetTraceSpeed_CB(DobotV3CmdInterCBParm packet);
    static void LineSensorState_CB(DobotV3CmdInterCBParm packet);
    static void SetLineSensorAdjust_CB(DobotV3CmdInterCBParm packet);
    static void SetLineSensorDefault_CB(DobotV3CmdInterCBParm packet);
    static void GetLineSensorData_CB(DobotV3CmdInterCBParm packet);
    static void SetCmdQueueStart_CB(DobotV3CmdInterCBParm packet);
    static void SetCmdQueueStop_CB(DobotV3CmdInterCBParm packet);
    static void SetCmdQueueForcelyStop_CB(DobotV3CmdInterCBParm packet);
    static void CleanCmdQueue_CB(DobotV3CmdInterCBParm packet);
    static void GetQueuedCmdCurrentIndex_CB(DobotV3CmdInterCBParm packet);
    static void GetCmdQueueAvailableSpace_CB(DobotV3CmdInterCBParm packet);
    static void SetMoveSpeedTime_CB(DobotV3CmdInterCBParm packet);
    static void SetMoveSpeedDirect_CB(DobotV3CmdInterCBParm packet);
    static void SetCoordClosedLoop_CB(DobotV3CmdInterCBParm packet);
    static void SetIncrementClosedLoop_CB(DobotV3CmdInterCBParm packet);
    static void GetImuSpeed_CB(DobotV3CmdInterCBParm packet);
    static void SetTracePid_CB(DobotV3CmdInterCBParm packet);
    static void SetTraceLoop_CB(DobotV3CmdInterCBParm packet);
    static void GetImgToArmXY_CB(DobotV3CmdInterCBParm packet);


signals:
    void receiveData_signal(bool isTimeOut, QJsonObject jsonObj, quint32 seqNum);
    void sendCmd_signal(DobotV3Format_Packet pPacket, quint16 payloadLen, bool isAck);

private:
    static MagicianGoApi *m_staticMagicianGoApi;
    DobotV3Format_Packet m_packFrame;
    MagicianGoApi();
    ~MagicianGoApi();
};

#endif // MOOZAPI_H
