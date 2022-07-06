#ifndef DM1PROTOCOL_H
#define DM1PROTOCOL_H

#include <QObject>
#include <QMap>
#include <QJsonObject>

#include "DP2Packet.h"

class DM1Protocol : public QObject
{
    Q_OBJECT

public:
    typedef QJsonObject (DM1Protocol::*DecodeFunction)(quint8, QByteArray);
    typedef void (DM1Protocol::*EncodeFunction)(PacketPayload &, QJsonObject);

    static DM1Protocol *getInstance();
    const QStringList getCommandList();

    /* parse params of payload for reading */
    QJsonObject parseParams(PacketPayload &payload);

    /* set payload for sending */
    bool setPayload(QString cmd, PacketPayload &payload, QJsonObject params);

private:
    explicit DM1Protocol(QObject *parent = nullptr);
    Q_DISABLE_COPY(DM1Protocol)

    QMap<QString, EncodeFunction> m_encodeFuncMap;
    QMap<int, DecodeFunction> m_decodeFuncMap;

    void _encodeFunctionInit();
    void _decodeFunctionInit();

    inline bool checkIntValue(const QJsonObject &obj, const QString &value);
    inline bool checkFloatValue(const QJsonObject &obj, const QString &value);
    inline bool checkBoolValue(const QJsonObject &obj, const QString &value);
    inline bool checkStringValue(const QJsonObject &obj, const QString &value, QString defaultValue = QString());
    inline bool checkArrayValue(const QJsonObject &obj, const QString &value);
    inline QJsonObject addQueueIndexInfo(QJsonObject &resObj, QByteArray &params);


    /* DECODE */

    /* 参考 "Dobot M1-protocol-V2.0" */
    QJsonObject decodeDeviceSN(quint8 rw, QByteArray params);           // 0
    QJsonObject decodeDeviceName(quint8 rw, QByteArray params);         // 1
    QJsonObject decodeDeviceVersion(quint8 rw, QByteArray params);      // 2
    QJsonObject decodeHardwareVersion(quint8 rw, QByteArray params);    // 5

    QJsonObject decodePose(quint8 rw, QByteArray params);               // 10
    QJsonObject decodeResetPose(quint8 rw, QByteArray params);          // 11

    QJsonObject decodeAlarmState(quint8 rw, QByteArray params);         // 20

    QJsonObject decodeServoPower(quint8 rw, QByteArray params);         // 28
    QJsonObject decodeRunState(quint8 rw, QByteArray params);           // 29

    QJsonObject decodeHOMECmd(quint8 rw, QByteArray params);            // 31
    QJsonObject decodeHOMEInitialPos(quint8 rw, QByteArray params);     // 34

    QJsonObject decodeHHTTrigMode(quint8 rw, QByteArray params);        // 40
    QJsonObject decodeHHTTrigOutputEnabled(quint8 rw, QByteArray params);//41
    QJsonObject decodeHHTTrigOutput(quint8 rw, QByteArray params);      // 42

    QJsonObject decodeArmOrientation(quint8 rw, QByteArray params);     // 50

    QJsonObject decodeEndEffectorParams(quint8 rw, QByteArray params);  // 60
    QJsonObject decodeEndEffectorLaser(quint8 rw, QByteArray params);   // 61
    QJsonObject decodeEndEffectorSuctionCup(quint8 rw, QByteArray params);//62
    QJsonObject decodeEndEffectorGripper(quint8 rw, QByteArray params); // 63

    QJsonObject decodeJOGJointParams(quint8 rw, QByteArray params);     // 70
    QJsonObject decodeJOGCoordinateParams(quint8 rw, QByteArray params);// 71
    QJsonObject decodeJOGCommonParams(quint8 rw, QByteArray params);    // 72
    QJsonObject decodeJOGCmd(quint8 rw, QByteArray params);             // 73

    QJsonObject decodeInchMode(quint8 rw, QByteArray params);           // 75
    QJsonObject decodeInchParam(quint8 rw, QByteArray params);          // 76

    QJsonObject decodePTPJointParams(quint8 rw, QByteArray params);     // 80
    QJsonObject decodePTPCoordinateParams(quint8 rw, QByteArray params);// 81
    QJsonObject decodePTPJumpParams(quint8 rw, QByteArray params);      // 82
    QJsonObject decodePTPCommonParams(quint8 rw, QByteArray params);    // 83
    QJsonObject decodePTPCmd(quint8 rw, QByteArray params);             // 84
    QJsonObject decodePTPPOCmd(quint8 rw, QByteArray params);           // 88

    QJsonObject decodeCPParams(quint8 rw, QByteArray params);           // 90
    QJsonObject decodeCPCmd(quint8 rw, QByteArray params);              // 91
    QJsonObject decodeCPLECmd(quint8 rw, QByteArray params);            // 92

    QJsonObject decodeARCParams(quint8 rw, QByteArray params);          // 100
    QJsonObject decodeARCCmd(quint8 rw, QByteArray params);             // 101
    QJsonObject decodeCircleCmd(quint8 rw, QByteArray params);          // 102
    QJsonObject decodeMotivateCmd(quint8 rw, QByteArray params);        // 103
    QJsonObject decodeMotivateZCmd(quint8 rw, QByteArray params);       // 104
    QJsonObject decodeARCPOCmd(quint8 rw, QByteArray params);           // 108
    QJsonObject decodeCirclePOCmd(quint8 rw, QByteArray params);        // 109

    QJsonObject decodeWAITCmd(quint8 rw, QByteArray params);            // 110

    QJsonObject decodeTRIGCmd(quint8 rw, QByteArray params);            // 120

    QJsonObject decodeIODO(quint8 rw, QByteArray params);               // 131
    QJsonObject decodeIODI(quint8 rw, QByteArray params);               // 133
    QJsonObject decodeIOADC(quint8 rw, QByteArray params);              // 134
    QJsonObject decodeIODOs(quint8 rw, QByteArray params);              // 136
    QJsonObject decodeGetIODIs(quint8 rw, QByteArray params);           // 137
    QJsonObject decodeGetIOADCs(quint8 rw, QByteArray params);          // 138
    QJsonObject decodeGetIODACs(quint8 rw, QByteArray params);          // 139

    QJsonObject decodeRebootEncoderICPVL(quint8 rw, QByteArray params); // 141

    QJsonObject decodeFirmwareMode(quint8 rw, QByteArray params);       // 161

    QJsonObject decodeFirmwareReboot(quint8 rw, QByteArray params);     // 166
    QJsonObject decodeLanPortConfig(quint8 rw, QByteArray params);      // 167
    QJsonObject decodeFirmwareNotifyM4Mode(quint8 rw, QByteArray params);//168

    QJsonObject decodeSafeModeEnabled(quint8 rw, QByteArray params);    // 200
    QJsonObject decodeCollisionThreshold(quint8 rw, QByteArray params); // 201
    QJsonObject decodeBasicDynamicParams(quint8 rw, QByteArray params); // 202
    QJsonObject decodeLoadParams(quint8 rw, QByteArray params);         // 203
    QJsonObject decodeFeedforward(quint8 rw, QByteArray params);        // 204
    QJsonObject decodeMotivationMode(quint8 rw, QByteArray params);     // 205
    QJsonObject decodeTrajectory(quint8 rw, QByteArray params);         // 206
    QJsonObject decodeSafeStrategy(quint8 rw, QByteArray params);       // 208
    QJsonObject decodeSafeGuardMode(quint8 rw, QByteArray params);      // 210
    QJsonObject decodeSafeGuardStatus(quint8 rw, QByteArray params);    // 211

    QJsonObject decodeGetUserParams(quint8 rw, QByteArray params);      // 220

    QJsonObject decodeQueuedCmdStart(quint8 rw, QByteArray params);         // 240
    QJsonObject decodeQueuedCmdStop(quint8 rw, QByteArray params);          // 241
    QJsonObject decodeQueuedCmdForceStop(quint8 rw, QByteArray params);     // 242
    QJsonObject decodeQueuedCmdStartDownload(quint8 rw, QByteArray params); // 243
    QJsonObject decodeQueuedCmdStopDownload(quint8 rw, QByteArray params);  // 244
    QJsonObject decodeQueuedCmdClear(quint8 rw, QByteArray params);         // 245
    QJsonObject decodeQueuedCmdCurrentIndex(quint8 rw, QByteArray params);  // 246
    QJsonObject decodeQueuedCmdLeftSpace(quint8 rw, QByteArray params);     // 247

    QJsonObject decodeUserCoordinate(quint8 rw, QByteArray params);     // 250
    QJsonObject decodeToolCoordinate(quint8 rw, QByteArray params);     // 251


    /* ENCODE */

    /* id:0~5 */
    void encodeSetDeviceSN(PacketPayload &payload, QJsonObject params);             // 0
    void encodeGetDeviceSN(PacketPayload &payload, QJsonObject params);             // 0
    void encodeSetDeviceName(PacketPayload &payload, QJsonObject params);           // 1
    void encodeGetDeviceName(PacketPayload &payload, QJsonObject params);           // 1
    void encodeGetDeviceVersion(PacketPayload &payload, QJsonObject params);        // 2
    void encodeGetHardwareVersion(PacketPayload &payload, QJsonObject params);      // 5

    /* id:10~13 */
    void encodeGetPose(PacketPayload &payload, QJsonObject params);                 // 10
    void encodeResetPose(PacketPayload &payload, QJsonObject params);               // 11

    /* id:20 */
    void encodeGetAlarmsState(PacketPayload &payload, QJsonObject params);          // 20
    void encodeClearAllAlarmsState(PacketPayload &payload, QJsonObject params);     // 20

    /* id:28、29 */
    void encodeSetServoPower(PacketPayload &payload, QJsonObject params);           // 28
    void encodeGetServoPower(PacketPayload &payload, QJsonObject params);           // 28
    void encodeGetRunState(PacketPayload &payload, QJsonObject params);             // 29

    /* id:31、34 */
    void encodeSetHOMECmd(PacketPayload &payload, QJsonObject params);              // 31
    void encodeSetHOMEInitialPos(PacketPayload &payload, QJsonObject params);       // 34

    /* id:40~42 */
    void encodeSetHHTTrigMode(PacketPayload &payload, QJsonObject params);          // 40
    void encodeGetHHTTrigMode(PacketPayload &payload, QJsonObject params);          // 40
    void encodeSetHHTTrigOutputEnabled(PacketPayload &payload, QJsonObject params); // 41
    void encodeGetHHTTrigOutputEnabled(PacketPayload &payload, QJsonObject params); // 41
    void encodeGetHHTTrigOutput(PacketPayload &payload, QJsonObject params);        // 42

    void encodeSetArmOrientation(PacketPayload &payload, QJsonObject params);       // 50
    void encodeGetArmOrientation(PacketPayload &payload, QJsonObject params);       // 50

    /* id:60~63 */
    void encodeSetEndEffectorParams(PacketPayload &payload, QJsonObject params);    // 60
    void encodeGetEndEffectorParams(PacketPayload &payload, QJsonObject params);    // 60
    void encodeSetEndEffectorLaser(PacketPayload &payload, QJsonObject params);     // 61
    void encodeGetEndEffectorLaser(PacketPayload &payload, QJsonObject params);     // 61
    void encodeSetEndEffectorSuctionCup(PacketPayload &payload, QJsonObject params);// 62
    void encodeGetEndEffectorSuctionCup(PacketPayload &payload, QJsonObject params);// 62
    void encodeSetEndEffectorGripper(PacketPayload &payload, QJsonObject params);   // 63
    void encodeGetEndEffectorGripper(PacketPayload &payload, QJsonObject params);   // 63

    /* id:70~76*/
    void encodeSetJOGJointParams(PacketPayload &payload, QJsonObject params);       // 70
    void encodeGetJOGJointParams(PacketPayload &payload, QJsonObject params);       // 70
    void encodeSetJOGCoordinateParams(PacketPayload &payload, QJsonObject params);  // 71
    void encodeGetJOGCoordinateParams(PacketPayload &payload, QJsonObject params);  // 71
    void encodeSetJOGCommonParams(PacketPayload &payload, QJsonObject params);      // 72
    void encodeGetJOGCommonParams(PacketPayload &payload, QJsonObject params);      // 72
    void encodeSetJOGCmd(PacketPayload &payload, QJsonObject params);               // 73
    void encodeSetInchMode(PacketPayload &payload, QJsonObject params);             // 75
    void encodeGetInchMode(PacketPayload &payload, QJsonObject params);             // 75
    void encodeSetInchParam(PacketPayload &payload, QJsonObject params);            // 76
    void encodeGetInchParam(PacketPayload &payload, QJsonObject params);            // 76

    /* id:80~89 */
    void encodeSetPTPJointParams(PacketPayload &payload, QJsonObject params);       // 80
    void encodeGetPTPJointParams(PacketPayload &payload, QJsonObject params);       // 80
    void encodeSetPTPCoordinateParams(PacketPayload &payload, QJsonObject params);  // 81
    void encodeGetPTPCoordinateParams(PacketPayload &payload, QJsonObject params);  // 81
    void encodeSetPTPJumpParams(PacketPayload &payload, QJsonObject params);        // 82
    void encodeGetPTPJumpParams(PacketPayload &payload, QJsonObject params);        // 82
    void encodeSetPTPCommonParams(PacketPayload &payload, QJsonObject params);      // 83
    void encodeGetPTPCommonParams(PacketPayload &payload, QJsonObject params);      // 83
    void encodeSetPTPCmd(PacketPayload &payload, QJsonObject params);               // 84
    void encodeSetPTPPOCmd(PacketPayload &payload, QJsonObject params);             // 88

    /* id:90~95 */
    void encodeSetCPParams(PacketPayload &payload, QJsonObject params);             // 90
    void encodeGetCPParams(PacketPayload &payload, QJsonObject params);             // 90
    void encodeSetCPCmd(PacketPayload &payload, QJsonObject params);                // 91
    void encodeSetCPLECmd(PacketPayload &payload, QJsonObject params);              // 92

    /* id:100~104、108~109 */
    void encodeSetARCParams(PacketPayload &payload, QJsonObject params);            // 100
    void encodeGetARCParams(PacketPayload &payload, QJsonObject params);            // 100
    void encodeSetARCCmd(PacketPayload &payload, QJsonObject params);               // 101
    void encodeSetCircleCmd(PacketPayload &payload, QJsonObject params);            // 102
    void encodeSetMotivateCmd(PacketPayload &payload, QJsonObject params);          // 103
    void encodeSetMotivateZCmd(PacketPayload &payload, QJsonObject params);         // 104

    void encodeSetARCPOCmd(PacketPayload &payload, QJsonObject params);             // 108
    void encodeSetCirclePOCmd(PacketPayload &payload, QJsonObject params);          // 109

    /* id:110 */
    void encodeSetWAITCmd(PacketPayload &payload, QJsonObject params);              // 110

    /* id:120 */
    void encodeSetTRIGCmd(PacketPayload &payload, QJsonObject params);              // 120

    /* id:130~139 */
    void encodeSetIODO(PacketPayload &payload, QJsonObject params);                 // 131
    void encodeGetIODO(PacketPayload &payload, QJsonObject params);                 // 131
    void encodeGetIODI(PacketPayload &payload, QJsonObject params);                 // 133
    void encodeGetIOADC(PacketPayload &payload, QJsonObject params);                // 134
    void encodeSetIODOs(PacketPayload &payload, QJsonObject params);                   // 136
    void encodeGetIODOs(PacketPayload &payload, QJsonObject params);                   // 136
    void encodeGetIODIs(PacketPayload &payload, QJsonObject params);                   // 137
    void encodeGetIOADCs(PacketPayload &payload, QJsonObject params);                  // 138
    void encodeIODACs(PacketPayload &payload, QJsonObject params);                     // 139

    /* id:141 */
    void encodeSetRebootEncoderICPVL(PacketPayload &payload, QJsonObject params);      //141

    /* id:161 */
    void encodeSetFirmwareMode(PacketPayload &payload, QJsonObject params);         //161
    void encodeGetFirmwareMode(PacketPayload &payload, QJsonObject params);         //161

    /* id:166~168 */
    void encodeSetFirmwareReboot(PacketPayload &payload, QJsonObject params);       // 166
    void encodeSetLanPortConfig(PacketPayload &payload, QJsonObject params);        // 167
    void encodeGetLanPortConfig(PacketPayload &payload, QJsonObject params);        // 167
    void encodeSetFirmwareNotifyM4Mode(PacketPayload &payload, QJsonObject params); // 168
    void encodeGetFirmwareNotifyM4Mode(PacketPayload &payload, QJsonObject params); // 168

    /* 200 201 203~206 208 210 211 220 */
    void encodeSetSafeModeEnabled(PacketPayload &payload, QJsonObject params);      // 200
    void encodeGetSafeModeEnabled(PacketPayload &payload, QJsonObject params);      // 200
    void encodeSetCollisionThreshold(PacketPayload &payload, QJsonObject params);   // 201
    void encodeGetCollisionThreshold(PacketPayload &payload, QJsonObject params);   // 201
    void encodeSetBasicDynamicParams(PacketPayload &payload, QJsonObject params);   // 202
    void encodeGetBasicDynamicParams(PacketPayload &payload, QJsonObject params);   // 202
    void encodeSetLoadParams(PacketPayload &payload, QJsonObject params);           // 203
    void encodeGetLoadParams(PacketPayload &payload, QJsonObject params);           // 203
    void encodeSetFeedforward(PacketPayload &payload, QJsonObject params);          // 204
    void encodeGetFeedforward(PacketPayload &payload, QJsonObject params);          // 204
    void encodeSetMotivationMode(PacketPayload &payload, QJsonObject params);       // 205
    void encodeGetMotivationMode(PacketPayload &payload, QJsonObject params);       // 205
    void encodeGetTrajectory(PacketPayload &payload, QJsonObject params);           // 206
    void encodeSetSafeStrategy(PacketPayload &payload, QJsonObject params);         // 208
    void encodeGetSafeStrategy(PacketPayload &payload, QJsonObject params);         // 208
    void encodeSetSafeGuardMode(PacketPayload &payload, QJsonObject params);        // 210
    void encodeGetSafeGuardMode(PacketPayload &payload, QJsonObject params);        // 210
    void encodeGetSafeGuardStatus(PacketPayload &payload, QJsonObject params);      // 211
    void encodeGetUserParams(PacketPayload &payload, QJsonObject params);           // 220

    /* id:240~247 */
    void encodeSetQueuedCmdStart(PacketPayload &payload, QJsonObject params);       // 240
    void encodeSetQueuedCmdStop(PacketPayload &payload, QJsonObject params);        // 241
    void encodeSetQueuedCmdForceStop(PacketPayload &payload, QJsonObject params);   // 242
    void encodeSetQueuedCmdStartDownload(PacketPayload &payload, QJsonObject params);//243
    void encodeSetQueuedCmdStopDownload(PacketPayload &payload, QJsonObject params);// 244
    void encodeSetQueuedCmdClear(PacketPayload &payload, QJsonObject params);       // 245
    void encodeGetQueuedCmdCurrentIndex(PacketPayload &payload, QJsonObject params);// 246
    void encodeGetQueuedCmdLeftSpace(PacketPayload &payload, QJsonObject params);   // 247

    /* 250~251 */
    void encodeSetUserCoordinate (PacketPayload &payload, QJsonObject params);      // 250
    void encodeGetUserCoordinate (PacketPayload &payload, QJsonObject params);      // 250
    void encodeSetToolCoordinate (PacketPayload &payload, QJsonObject params);      // 251
    void encodeGetToolCoordinate (PacketPayload &payload, QJsonObject params);      // 251

public:
#pragma pack(push, 1)
    /* POSE */
    struct PositionStruct {
        float x;
        float y;
        float z;
        float r;
    };

    struct PoseStruct {
        PositionStruct coord;
        float jointAngle[4];
    };

    /* VELOCITY */
    struct VelocityArrayStruct {
        float velocity[4];
        float acceleration[4];
    };

    struct VelocityRatioStruct {
        float velocityRatio;
        float accelerationRatio;
    };

    /* InchParams */
    struct InchParamsStruct {
        float distanceMM;
        float distanceANG;
    };

    /* Collision Threshold */
    struct TorDiffParamsStruct {
        float torDiffJ1;
        float torDiffJ2;
        float torDiffJ3;
        float torDiffJ4;
    };

    /* ARC */
    struct ARCCmdStruct {
        PositionStruct cirPoint;
        PositionStruct toPoint;
    };

    struct ARCParamsStruct {
        float xyzVelocity;
        float rVelocity;
        float xyzAcceleration;
        float rAcceleration;
    };

    /* Circle */
    struct CircleCmdStruct {
        PositionStruct cirPoint;
        PositionStruct toPoint;
        quint32 count;
    };

    /* CP */
    struct CPCmdStruct {
        quint8 cpMode;
        float x;
        float y;
        float z;
        union {
            float velocity;
            float power;
        };
    };

    struct CPParamsStruct {
        float targetAcc;
        float junctionVel;
        union {
            float acc;
            float period;
        };
        quint8 realTimeTrack;
    };

    /* PTP */
    struct PTPCmdStruct {
        quint8 ptpMode;
        PositionStruct coord;
    };

    struct PTPCoordinateParamsStruct {
        float xyzVelocity;
        float rVelocity;
        float xyzAcceleration;
        float rAcceleration;
    };

    struct ParallelOutputStruct {
        quint8 ratio;
        quint16 port;
        quint8 level;
    };

    /* Dynamic */
    struct BasicDynamicParamsStruct {
        float ZZ1;
        float FS1;
        float FV1;
        float ZZ2;
        float MX2;
        float MY2;
        float IA2;
        float FS2;
        float FV2;
    };

    /* TRIG */
    struct TRIGCmdStruct {
        quint8 port;
        quint8 mode;
        quint8 condition;
        quint16 threshold;
    };

    /* LAN */
    struct LanPortInfoStruct {
        quint8 status;
        quint8 isdhcp;
        quint8 addr[16];
        quint8 mask[16];
        quint8 gateway[16];
        quint8 dns[16];
    };

    /* IO */
    typedef struct tagIODOS{
        uint8_t address;        //I/O地址（取值范围1~22）
        uint8_t number;         //操作IO的数量
        uint8_t value[22];      //输出电平 0：低电平 1：高电平
    }IODOS;

    /* FirmwareMode */
    struct FirmwareModeStruct{
        quint8 mode;
        quint8 ctrl;
    };

    /* UserParams */
    struct tagUserParams{
        float params[8];
    };


#pragma pack(pop)
};

#endif // DM1PROTOCOL_H
