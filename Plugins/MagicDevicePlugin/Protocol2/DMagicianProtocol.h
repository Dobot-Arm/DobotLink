#ifndef DMAGICIANPROTOCOL_H
#define DMAGICIANPROTOCOL_H

#include <QObject>
#include <QMap>
#include <QJsonObject>

#include "DP2Packet.h"

class DMagicianProtocol : public QObject
{
    Q_OBJECT
public:
    typedef QJsonObject (DMagicianProtocol::*DecodeFunction)(quint8, QByteArray);
    typedef void (DMagicianProtocol::*EncodeFunction)(PacketPayload &, QJsonObject);

    static DMagicianProtocol *getInstance();
    const QStringList getCommandList();

    /* parse params of payload for reading */
    QJsonObject parseParams(PacketPayload &payload);

    /* set payload for sending */
    bool setPayload(QString cmd, PacketPayload &payload, QJsonObject params);

private:
    explicit DMagicianProtocol(QObject *parent = nullptr);
    Q_DISABLE_COPY(DMagicianProtocol)

    QMap<QString, EncodeFunction> m_encodeFuncMap;
    QMap<int, DecodeFunction> m_decodeFuncMap;
    QMap<int, DecodeFunction> m_decodeDobotSensorFuncMap;
    QMap<int, DecodeFunction> m_decodeBoxFuncMap;

    QString m_Device;

    void _encodeFunctionInit();
    void _decodeFunctionInit();
    void _decodeDobotSensorFunctionInit();
    void _decodeBoxFuncInit();

    inline bool checkIntValue(const QJsonObject &obj, const QString &value);
    inline bool checkFloatValue(const QJsonObject &obj, const QString &value);
    inline bool checkBoolValue(const QJsonObject &obj, const QString &value);
    inline bool checkStringValue(const QJsonObject &obj, const QString &value);
    inline bool checkArrayValue(const QJsonObject &obj, const QString &value);
    inline QJsonObject addQueueIndexInfo(QJsonObject &resObj, QByteArray &params);


    /* DECODE */

    /* 参考 "dobot-protocol-V1.2.0" */
    QJsonObject decodeDeviceSN(quint8 rw, QByteArray params);           // 0
    QJsonObject decodeDeviceName(quint8 rw, QByteArray params);         // 1
    QJsonObject decodeDeviceVersion(quint8 rw, QByteArray params);      // 2
    QJsonObject decodeDeviceWithL(quint8 rw, QByteArray params);        // 3
    QJsonObject decodeDeviceTime(quint8 rw, QByteArray params);         // 4
    QJsonObject decodeDeviceID(quint8 rw, QByteArray params);           // 5
    QJsonObject decodeDeviceInfo(quint8 rw, QByteArray params);         // 6
    QJsonObject decodeProductName(quint8 rw, QByteArray params);        // 7
    QJsonObject decodeOLEDDisplay(quint8 rw, QByteArray params);        // 8
    QJsonObject decodeProgressBar(quint8 rw, QByteArray params);        // 9

    QJsonObject decodePose(quint8 rw, QByteArray params);               // 10
    QJsonObject decodeResetPose(quint8 rw, QByteArray params);          // 11
    QJsonObject decodePoseL(quint8 rw, QByteArray params);              // 13
    QJsonObject decodeCheckPoseLimit(quint8 rw, QByteArray params);     // 15

    QJsonObject decodeAlarmState(quint8 rw, QByteArray params);         // 20

    QJsonObject decodeHOMEParams(quint8 rw, QByteArray params);         // 30
    QJsonObject decodeHOMECmd(quint8 rw, QByteArray params);            // 31
    QJsonObject decodeAutoLeveling(quint8 rw, QByteArray params);       // 32
    QJsonObject decodeGoHomeStatus(quint8 rw, QByteArray params);       // 33

    QJsonObject decodeHHTTrigMode(quint8 rw, QByteArray params);        // 40
    QJsonObject decodeHHTTrigOutputEnabled(quint8 rw, QByteArray params);//41
    QJsonObject decodeHHTTrigOutput(quint8 rw, QByteArray params);      // 42

    QJsonObject decodeArmOrientation(quint8 rw, QByteArray params);     // 50
    QJsonObject decodeMotorMode(quint8 rw, QByteArray params);          // 51

    QJsonObject decodeEndEffectorParams(quint8 rw, QByteArray params);  // 60
    QJsonObject decodeEndEffectorLaser(quint8 rw, QByteArray params);   // 61
    QJsonObject decodeEndEffectorSuctionCup(quint8 rw, QByteArray params);//62
    QJsonObject decodeEndEffectorGripper(quint8 rw, QByteArray params); // 63
    QJsonObject decodeEndEffectorType(quint8 rw, QByteArray params);    // 64

    QJsonObject decodeServoAngle(quint8 rw, QByteArray params);         // 65
    QJsonObject decodeArmSpeedRatio(quint8 rw, QByteArray params);      // 66
    QJsonObject decodeLSpeedRatio(quint8 rw, QByteArray params);        // 67
    QJsonObject decodeConfigAndStatus(quint8 rw, QByteArray params);    // 68
    QJsonObject decodeRTStatus(quint8 rw, QByteArray params);           // 69

    QJsonObject decodeJOGJointParams(quint8 rw, QByteArray params);     // 70
    QJsonObject decodeJOGCoordinateParams(quint8 rw, QByteArray params);// 71
    QJsonObject decodeJOGCommonParams(quint8 rw, QByteArray params);    // 72
    QJsonObject decodeJOGCmd(quint8 rw, QByteArray params);             // 73
    QJsonObject decodeJOGLParams(quint8 rw, QByteArray params);         // 74
    QJsonObject decodeJOGCmdTime(quint8 rw, QByteArray params);         // 75

    QJsonObject decodePTPJointParams(quint8 rw, QByteArray params);     // 80
    QJsonObject decodePTPCoordinateParams(quint8 rw, QByteArray params);// 81
    QJsonObject decodePTPJumpParams(quint8 rw, QByteArray params);      // 82
    QJsonObject decodePTPCommonParams(quint8 rw, QByteArray params);    // 83
    QJsonObject decodePTPCmd(quint8 rw, QByteArray params);             // 84
    QJsonObject decodePTPLParams(quint8 rw, QByteArray params);         // 85
    QJsonObject decodePTPWithLCmd(quint8 rw, QByteArray params);        // 86
    QJsonObject decodePTPJump2Params(quint8 rw, QByteArray params);     // 87
    QJsonObject decodePTPPOCmd(quint8 rw, QByteArray params);           // 88
    QJsonObject decodePTPPOWithLCmd(quint8 rw, QByteArray params);      // 89

    QJsonObject decodeCPParams(quint8 rw, QByteArray params);           // 90
    QJsonObject decodeCPCmd(quint8 rw, QByteArray params);              // 91
    QJsonObject decodeCPLECmd(quint8 rw, QByteArray params);            // 92
    QJsonObject decodeCPRHeadHoldEnable(quint8 rw, QByteArray params);  // 93
    QJsonObject decodeCPCommonParams(quint8 rw, QByteArray params);     // 94
    QJsonObject decodeCP2Cmd(quint8 rw, QByteArray params);             // 95

    QJsonObject decodeARCParams(quint8 rw, QByteArray params);          // 100
    QJsonObject decodeARCCmd(quint8 rw, QByteArray params);             // 101
    QJsonObject decodeCircleCmd(quint8 rw, QByteArray params);          // 102
    QJsonObject decodeARCCommonParams(quint8 rw, QByteArray params);    // 103

    QJsonObject decodeWAITCmd(quint8 rw, QByteArray params);            // 110

    QJsonObject decodeTRIGCmd(quint8 rw, QByteArray params);            // 120

    QJsonObject decodeIOMultiplexing(quint8 rw, QByteArray params);     // 130
    QJsonObject decodeIODO(quint8 rw, QByteArray params);               // 131
    QJsonObject decodeIOPWM(quint8 rw, QByteArray params);              // 132
    QJsonObject decodeIODI(quint8 rw, QByteArray params);               // 133
    QJsonObject decodeIOADC(quint8 rw, QByteArray params);              // 134
    QJsonObject decodeEMotor(quint8 rw, QByteArray params);             // 135
    QJsonObject decodeEMotorS(quint8 rw, QByteArray params);            // 136
    QJsonObject decodeColorSensor(quint8 rw, QByteArray params);        // 137
    QJsonObject decodeInfraredSensor(quint8 rw, QByteArray params);     // 138
    QJsonObject decodeIICInfo(quint8 rw, QByteArray params);            // 139

    QJsonObject decodeAngleSensorStaticError(quint8 rw, QByteArray params); // 140
    QJsonObject decodeAngleSensorCoef(quint8 rw, QByteArray params);        // 141
    QJsonObject decodeBaseDecoderStaticError(quint8 rw, QByteArray params); // 142
    QJsonObject decodeLRHandCalibrateValue(quint8 rw, QByteArray params);   // 143

    QJsonObject decodeWIFIConfigMode(quint8 rw, QByteArray params);         // 150
    QJsonObject decodeWIFISSID(quint8 rw, QByteArray params);               // 151
    QJsonObject decodeWIFIPassword(quint8 rw, QByteArray params);           // 152
    QJsonObject decodeWIFIIPAddress(quint8 rw, QByteArray params);          // 153
    QJsonObject decodeWIFINetmask(quint8 rw, QByteArray params);            // 154
    QJsonObject decodeWIFIGateway(quint8 rw, QByteArray params);            // 155
    QJsonObject decodeWIFIDNS(quint8 rw, QByteArray params);                // 156
    QJsonObject decodeWIFIConnectStatus(quint8 rw, QByteArray params);      // 157

    QJsonObject decodeLostStep(quint8 rw, QByteArray params);               // 170
    QJsonObject decodeLostStepCmd(quint8 rw, QByteArray params);            // 171
    QJsonObject decodeCollisionCheck(quint8 rw, QByteArray params);         // 172

    QJsonObject decodeDeviceStatus(quint8 rw, QByteArray params);           // 192
    QJsonObject decodeDeviceRunAPP(quint8 rw, QByteArray params);           // 193
    QJsonObject decodeUpgradeStatus(quint8 rw, QByteArray params);          // 194
    QJsonObject decodeRunUpgrade(quint8 rw, QByteArray params);             // 195
    QJsonObject decodeStartTransmit(quint8 rw, QByteArray params);          // 196
    QJsonObject decodeUpgradeVeify(quint8 rw, QByteArray params);           // 197
    QJsonObject decodeDeviceRestart(quint8 rw, QByteArray params);          // 198

    QJsonObject decodeColorObj(quint8 rw, QByteArray params);               // 200

    QJsonObject decodeSeeedDistanceSensor(quint8 rw, QByteArray params);    // 210
    QJsonObject decodeSeeedTempSensor(quint8 rw, QByteArray params);        // 211
    QJsonObject decodeSeeedLightSensor(quint8 rw, QByteArray params);       // 212
    QJsonObject decodeSeeedColorSensor(quint8 rw, QByteArray params);       // 213
    QJsonObject decodeSeeedRGBLed(quint8 rw, QByteArray params);            // 214
    QJsonObject decodeMCUTemperature(quint8 rw, QByteArray params);         // 215
    QJsonObject decodeDobotSensor(quint8 rw, QByteArray params);            // 216
    QJsonObject decodeBoxFunc(quint8 rw, QByteArray params);                // 217

    QJsonObject decodeQueuedCmdStart(quint8 rw, QByteArray params);         // 240
    QJsonObject decodeQueuedCmdStop(quint8 rw, QByteArray params);          // 241
    QJsonObject decodeQueuedCmdForceStop(quint8 rw, QByteArray params);     // 242
    QJsonObject decodeQueuedCmdStartDownload(quint8 rw, QByteArray params); // 243
    QJsonObject decodeQueuedCmdStopDownload(quint8 rw, QByteArray params);  // 244
    QJsonObject decodeQueuedCmdClear(quint8 rw, QByteArray params);         // 245
    QJsonObject decodeQueuedCmdCurrentIndex(quint8 rw, QByteArray params);  // 246
    QJsonObject decodeQueuedCmdLeftSpace(quint8 rw, QByteArray params);     // 247
    QJsonObject decodeQueuedCmdMotionFinish(quint8 rw, QByteArray params);  // 248

    /* Dobot Sensor id:216 subid:1~53 */
    QJsonObject decodeSensorRGBLEDInit(quint8 rw, QByteArray params);       // 216-1
    QJsonObject decodeSensorSetRGBLEDVlaue(quint8 rw, QByteArray params);   // 216-2
    QJsonObject decodeSensorRGBLEDTurnOFFAll(quint8 rw, QByteArray params); // 216-3
    QJsonObject decodeSensorSetRGBLEDState(quint8 rw, QByteArray params);   // 216-4
    QJsonObject decodeSensorOledInit(quint8 rw, QByteArray params);         // 216-10
    QJsonObject decodeSensorOledClear(quint8 rw, QByteArray params);        // 216-11
    QJsonObject decodeSensorOledDisplay(quint8 rw, QByteArray params);      // 216-12
    QJsonObject decodeSensorKnobInit(quint8 rw, QByteArray params);         // 216-20
    QJsonObject decodeSensorGetKnob(quint8 rw, QByteArray params);          // 216-21
    QJsonObject decodeSensorVoiceInit(quint8 rw, QByteArray params);        // 216-22
    QJsonObject decodeSensorGetVoice(quint8 rw, QByteArray params);         // 216-23
    QJsonObject decodeSensorLightInit(quint8 rw, QByteArray params);        // 216-24
    QJsonObject decodeSensorGetLight(quint8 rw, QByteArray params);         // 216-25
    QJsonObject decodeSensorPEInit(quint8 rw, QByteArray params);           // 216-26
    QJsonObject decodeSensorGetPEState(quint8 rw, QByteArray params);       // 216-27
    QJsonObject decodeSensorUltrasonicInit(quint8 rw, QByteArray params);   // 216-28
    QJsonObject decodeSensorUltrasonicGet(quint8 rw, QByteArray params);    // 216-29
    QJsonObject decodeSensorSHT31Init(quint8 rw, QByteArray params);        // 216-30
    QJsonObject decodeSensorGetSHT31(quint8 rw, QByteArray params);         // 216-31
    QJsonObject decodeSensorColorInit(quint8 rw, QByteArray params);        // 216-32
    QJsonObject decodeSensorGetColor(quint8 rw, QByteArray params);         // 216-33
    QJsonObject decodeSensorGetColorRes(quint8 rw, QByteArray params);      // 216-34
    QJsonObject decodeSensorXBeeInit(quint8 rw, QByteArray params);         // 216-40
    QJsonObject decodeSensorXBeeSend(quint8 rw, QByteArray params);         // 216-41
    QJsonObject decodeSensorXBeeReceive(quint8 rw, QByteArray params);      // 216-42
    QJsonObject decodeSensorXBeeClear(quint8 rw, QByteArray params);        // 216-43
    QJsonObject decodeSensorSYNInit(quint8 rw, QByteArray params);          // 216-50
    QJsonObject decodeSensorSetSYN(quint8 rw, QByteArray params);           // 216-51
    QJsonObject decodeSensorSetSYNMusic(quint8 rw, QByteArray params);      // 216-52
    QJsonObject decodeSensorSendSYN(quint8 rw, QByteArray params);          // 216-53
    QJsonObject decodeSensorSetSYNCmd(quint8 rw, QByteArray params);        // 216-54

    /* Dobot Ble id:216 subid:60~67 */
    QJsonObject decodeBleInf(quint8 rw, QByteArray params);                 // 216-60
    QJsonObject decodeBleConnect(quint8 rw, QByteArray params);             // 216-61
    QJsonObject decodeBleReadOneData(quint8 rw, QByteArray params);         // 216-63
    QJsonObject decodeBleWriteOneData(quint8 rw, QByteArray params);        // 216-64
    QJsonObject decodeSetBleMesh(quint8 rw, QByteArray params);             // 216-65
    QJsonObject decodeBleReadMeshData(quint8 rw, QByteArray params);        // 216-66
    QJsonObject decodeBleWriteMeshData(quint8 rw, QByteArray params);       // 216-67
    QJsonObject decodeBleClearMeshData(quint8 rw, QByteArray params);       // 216-68

    /* Dobot Sensor id:216 subid:70~72 手势传感器*/
    QJsonObject decodeSetSensorHandModel(quint8 rw, QByteArray params);     // 216-70
    QJsonObject decodeGetSensorHandModel(quint8 rw, QByteArray params);     // 216-71
    QJsonObject decodeIsSensorHandModel(quint8 rw, QByteArray params);      // 216-72

    /* Dobot Sensor id:216 subid:73~75 摇杆*/
    QJsonObject decodeIsJoystickButton(quint8 rw, QByteArray params);       // 216-73
    QJsonObject decodeGetJoystickPos(quint8 rw, QByteArray params);         // 216-74
    QJsonObject decodeGetJoystickButton(quint8 rw, QByteArray params);      // 216-75

    /* Dobot Sensor id:216 subid:76 人体红外*/
    QJsonObject decodeIsPirDetected(quint8 rw, QByteArray params);          // 216-76
    /* Dobot Sensor id:216 subid:77 读取红蓝按钮状态*/
    QJsonObject decodeGetButtonStatus(quint8 rw, QByteArray params);        // 216-77
    /* Dobot Sensor id:216 subid:78 读取红按钮状态*/
    QJsonObject decodeGetRedButtonStatus(quint8 rw, QByteArray params);     // 216-78
    /* Dobot Sensor id:216 subid:79 读取蓝按钮状态*/
    QJsonObject decodeGetBlueButtonStatus(quint8 rw, QByteArray params);    // 216-79

    /* GO相关 id:217 subid:10~20 */
    QJsonObject decodeSetStopPointParam(quint8 rw, QByteArray params);
    QJsonObject decodeSetStopPointServer(quint8 rw, QByteArray params);
    QJsonObject decodeGetStopPointState(quint8 rw, QByteArray params);
    QJsonObject decodeGetImgToArmXY(quint8 rw, QByteArray params);

    /* ENCODE */

    /* id:0~9 */
    void encodeSetDeviceSN(PacketPayload &payload, QJsonObject params);         // 0
    void encodeGetDeviceSN(PacketPayload &payload, QJsonObject params);         // 0
    void encodeSetDeviceName(PacketPayload &payload, QJsonObject params);       // 1
    void encodeGetDeviceName(PacketPayload &payload, QJsonObject params);       // 1
    void encodeGetDeviceVersion(PacketPayload &payload, QJsonObject params);    // 2
    void encodeSetDeviceWithL(PacketPayload &payload, QJsonObject params);      // 3
    void encodeGetDeviceWithL(PacketPayload &payload, QJsonObject params);      // 3
    void encodeGetDeviceTime(PacketPayload &payload, QJsonObject params);       // 4
    void encodeGetDeviceID(PacketPayload &payload, QJsonObject params);         // 5
    void encodeGetProductName(PacketPayload &payload, QJsonObject params);      // 7
    void encodeSetOLEDDisplay(PacketPayload &payload, QJsonObject params);      // 8
    void encodeSetProgressBar(PacketPayload &payload, QJsonObject params);      // 9

    /* id:10~13 */
    void encodeGetPose(PacketPayload &payload, QJsonObject params);             // 10
    void encodeResetPose(PacketPayload &payload, QJsonObject params);           // 11
    void encodeGetPoseL(PacketPayload &payload, QJsonObject params);            // 13
    void encodeCheckPoseLimit(PacketPayload &payload, QJsonObject params);      // 15

    /* id:20 */
    void encodeGetAlarmsState(PacketPayload &payload, QJsonObject params);      // 20
    void encodeClearAllAlarmsState(PacketPayload &payload, QJsonObject params); // 20

    /* id:30~33 */
    void encodeSetHOMEParams(PacketPayload &payload, QJsonObject params);       // 30
    void encodeGetHOMEParams(PacketPayload &payload, QJsonObject params);       // 30
    void encodeSetHOMECmd(PacketPayload &payload, QJsonObject params);          // 31
    void encodeSetAutoLeveling(PacketPayload &payload, QJsonObject params);     // 32
    void encodeGetAutoLeveling(PacketPayload &payload, QJsonObject params);     // 32
    void encodeGetGoHomeStatus(PacketPayload &payload, QJsonObject params);     // 33

    /* id:40~42 */
    void encodeSetHHTTrigMode(PacketPayload &payload, QJsonObject params);          // 40
    void encodeGetHHTTrigMode(PacketPayload &payload, QJsonObject params);          // 40
    void encodeSetHHTTrigOutputEnabled(PacketPayload &payload, QJsonObject params); // 41
    void encodeGetHHTTrigOutputEnabled(PacketPayload &payload, QJsonObject params); // 41
    void encodeGetHHTTrigOutput(PacketPayload &payload, QJsonObject params);        // 42

    /* id:60~64 */
    void encodeSetEndEffectorParams(PacketPayload &payload, QJsonObject params);    // 60
    void encodeGetEndEffectorParams(PacketPayload &payload, QJsonObject params);    // 60
    void encodeSetEndEffectorLaser(PacketPayload &payload, QJsonObject params);     // 61
    void encodeGetEndEffectorLaser(PacketPayload &payload, QJsonObject params);     // 61
    void encodeSetEndEffectorSuctionCup(PacketPayload &payload, QJsonObject params);// 62
    void encodeGetEndEffectorSuctionCup(PacketPayload &payload, QJsonObject params);// 62
    void encodeSetEndEffectorGripper(PacketPayload &payload, QJsonObject params);   // 63
    void encodeGetEndEffectorGripper(PacketPayload &payload, QJsonObject params);   // 63
    void encodeSetEndEffectorType(PacketPayload &payload, QJsonObject params);      // 64
    void encodeGetEndEffectorType(PacketPayload &payload, QJsonObject params);      // 64

    /* id:65~69 */
    void encodeSetServoAngle(PacketPayload &payload, QJsonObject params);           // 65
    void encodeGetServoAngle(PacketPayload &payload, QJsonObject params);           // 65
    void encodeSetArmSpeedRatio(PacketPayload &payload, QJsonObject params);        // 66
    void encodeGetArmSpeedRatio(PacketPayload &payload, QJsonObject params);        // 66
    void encodeSetLSpeedRatio(PacketPayload &payload, QJsonObject params);          // 67
    void encodeGetLSpeedRatio(PacketPayload &payload, QJsonObject params);          // 67
    void encodeGetConfigAndStatus(PacketPayload &payload, QJsonObject params);      // 68
    void encodeGetRTStatus(PacketPayload &payload, QJsonObject params);             // 69

    /* id:70~75*/
    void encodeSetJOGJointParams(PacketPayload &payload, QJsonObject params);       // 70
    void encodeGetJOGJointParams(PacketPayload &payload, QJsonObject params);       // 70
    void encodeSetJOGCoordinateParams(PacketPayload &payload, QJsonObject params);  // 71
    void encodeGetJOGCoordinateParams(PacketPayload &payload, QJsonObject params);  // 71
    void encodeSetJOGCommonParams(PacketPayload &payload, QJsonObject params);      // 72
    void encodeGetJOGCommonParams(PacketPayload &payload, QJsonObject params);      // 72
    void encodeSetJOGCmd(PacketPayload &payload, QJsonObject params);               // 73
    void encodeSetJOGLParams(PacketPayload &payload, QJsonObject params);           // 74
    void encodeGetJOGLParams(PacketPayload &payload, QJsonObject params);           // 74
    void encodeSetJOGCmdTime(PacketPayload &payload, QJsonObject params);           // 75

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
    void encodeSetPTPLParams(PacketPayload &payload, QJsonObject params);           // 85
    void encodeGetPTPLParams(PacketPayload &payload, QJsonObject params);           // 85
    void encodeSetPTPWithLCmd(PacketPayload &payload, QJsonObject params);          // 86
    void encodeSetPTPJump2Params(PacketPayload &payload, QJsonObject params);       // 87
    void encodeGetPTPJump2Params(PacketPayload &payload, QJsonObject params);       // 87
    void encodeSetPTPPOCmd(PacketPayload &payload, QJsonObject params);             // 88
    void encodeSetPTPPOWithLCmd(PacketPayload &payload, QJsonObject params);        // 89

    /* id:90~95 */
    void encodeSetCPParams(PacketPayload &payload, QJsonObject params);             // 90
    void encodeGetCPParams(PacketPayload &payload, QJsonObject params);             // 90
    void encodeSetCPCmd(PacketPayload &payload, QJsonObject params);                // 91
    void encodeSetCPLECmd(PacketPayload &payload, QJsonObject params);              // 92
    void encodeSetCPRHeadHoldEnable(PacketPayload &payload, QJsonObject params);    // 93
    void encodeGetCPRHeadHoldEnable(PacketPayload &payload, QJsonObject params);    // 93
    void encodeSetCPCommonParams(PacketPayload &payload, QJsonObject params);       // 94
    void encodeGetCPCommonParams(PacketPayload &payload, QJsonObject params);       // 94
    void encodeSetCP2Cmd(PacketPayload &payload, QJsonObject params);               // 95

    /* id:100~103 */
    void encodeSetARCParams(PacketPayload &payload, QJsonObject params);            // 100
    void encodeGetARCParams(PacketPayload &payload, QJsonObject params);            // 100
    void encodeSetARCCmd(PacketPayload &payload, QJsonObject params);               // 101
    void encodeSetCircleCmd(PacketPayload &payload, QJsonObject params);            // 102
    void encodeSetARCCommonParams(PacketPayload &payload, QJsonObject params);      // 103
    void encodeGetARCCommonParams(PacketPayload &payload, QJsonObject params);      // 103

    /* id:110 */
    void encodeSetWAITCmd(PacketPayload &payload, QJsonObject params);              // 110

    /* id:120 */
    void encodeSetTRIGCmd(PacketPayload &payload, QJsonObject params);              // 120

    /* id:130~139 */
    void encodeSetIOMultiplexing(PacketPayload &payload, QJsonObject params);       // 130
    void encodeGetIOMultiplexing(PacketPayload &payload, QJsonObject params);       // 130
    void encodeSetIODO(PacketPayload &payload, QJsonObject params);                 // 131
    void encodeGetIODO(PacketPayload &payload, QJsonObject params);                 // 131
    void encodeSetIOPWM(PacketPayload &payload, QJsonObject params);                // 132
    void encodeGetIOPWM(PacketPayload &payload, QJsonObject params);                // 132
    void encodeGetIODI(PacketPayload &payload, QJsonObject params);                 // 133
    void encodeGetIOADC(PacketPayload &payload, QJsonObject params);                // 134
    void encodeSetEMotor(PacketPayload &payload, QJsonObject params);               // 135
    void encodeSetEMotorS(PacketPayload &payload, QJsonObject params);              // 136
    void encodeSetColorSensor(PacketPayload &payload, QJsonObject params);          // 137
    void encodeGetColorSensor(PacketPayload &payload, QJsonObject params);          // 137
    void encodeSetInfraredSensor(PacketPayload &payload, QJsonObject params);       // 138
    void encodeGetInfraredSensor(PacketPayload &payload, QJsonObject params);       // 138
    void encodeSetIICInfo(PacketPayload &payload, QJsonObject params);              // 139
    void encodeGetIICInfo(PacketPayload &payload, QJsonObject params);              // 139

    /* id:140~143 */
    void encodeSetAngleSensorStaticError(PacketPayload &payload, QJsonObject params);//140
    void encodeGetAngleSensorStaticError(PacketPayload &payload, QJsonObject params);//140
    void encodeSetAngleSensorCoef(PacketPayload &payload, QJsonObject params);      // 141
    void encodeGetAngleSensorCoef(PacketPayload &payload, QJsonObject params);      // 141
    void encodeSetBaseDecoderStaticError(PacketPayload &payload, QJsonObject params);//142
    void encodeGetBaseDecoderStaticError(PacketPayload &payload, QJsonObject params);//142
    void encodeSetLRHandCalibrateValue(PacketPayload &payload, QJsonObject params); // 143
    void encodeGetLRHandCalibrateValue(PacketPayload &payload, QJsonObject params); // 143

    /* id:150~157 */
    void encodeSetWIFIConfigMode(PacketPayload &payload, QJsonObject params);       // 150
    void encodeGetWIFIConfigMode(PacketPayload &payload, QJsonObject params);       // 150
    void encodeSetWIFISSID(PacketPayload &payload, QJsonObject params);             // 151
    void encodeGetWIFISSID(PacketPayload &payload, QJsonObject params);             // 151
    void encodeSetWIFIPassword(PacketPayload &payload, QJsonObject params);         // 152
    void encodeGetWIFIPassword(PacketPayload &payload, QJsonObject params);         // 152
    void encodeSetWIFIIPAddress(PacketPayload &payload, QJsonObject params);        // 153
    void encodeGetWIFIIPAddress(PacketPayload &payload, QJsonObject params);        // 153
    void encodeSetWIFINetmask(PacketPayload &payload, QJsonObject params);          // 154
    void encodeGetWIFINetmask(PacketPayload &payload, QJsonObject params);          // 154
    void encodeSetWIFIGateway(PacketPayload &payload, QJsonObject params);          // 155
    void encodeGetWIFIGateway(PacketPayload &payload, QJsonObject params);          // 155
    void encodeSetWIFIDNS(PacketPayload &payload, QJsonObject params);              // 156
    void encodeGetWIFIDNS(PacketPayload &payload, QJsonObject params);              // 156
    void encodeGetWIFIConnectStatus(PacketPayload &payload, QJsonObject params);    // 157

    /* id:170~172 */
    void encodeSetLostStepValue(PacketPayload &payload, QJsonObject params);        // 170
    void encodeSetLostStepCmd(PacketPayload &payload, QJsonObject params);          // 171
    void encodeSetCollisionCheck(PacketPayload &payload, QJsonObject params);       // 172
    void encodeGetCollisionCheck(PacketPayload &payload, QJsonObject params);       // 172

    /* id:192~198 */
    void encodeGetDeviceStatus(PacketPayload &payload, QJsonObject params);         // 192
    void encodeSetDeviceRunAPP(PacketPayload &payload, QJsonObject params);         // 193
    void encodeSetUpgradeStatus(PacketPayload &payload, QJsonObject params);        // 194
    void encodeGetUpgradeStatus(PacketPayload &payload, QJsonObject params);        // 194
    void encodeSetRunUpgrade(PacketPayload &payload, QJsonObject params);           // 195
    void encodeSetStartTransmit(PacketPayload &payload, QJsonObject params);        // 196
    void encodeSetUpgradeVeify(PacketPayload &payload, QJsonObject params);         // 197
    void encodeSetDeviceRestart(PacketPayload &payload, QJsonObject params);        // 198

    /* id:200 */
    void encodeGetColorObjExist(PacketPayload &payload, QJsonObject params);        // 200
    void encodeGetColorObjCoordinate(PacketPayload &payload, QJsonObject params);   // 200

    /* id:210~215 */
    void encodeGetSeeedDistanceSensor(PacketPayload &payload, QJsonObject params);  // 210
    void encodeSetSeeedTempSensor(PacketPayload &payload, QJsonObject params);      // 211
    void encodeGetSeeedTempSensor(PacketPayload &payload, QJsonObject params);      // 211
    void encodeSetSeeedLightSensor(PacketPayload &payload, QJsonObject params);     // 212
    void encodeGetSeeedLightSensor(PacketPayload &payload, QJsonObject params);     // 212
    void encodeSetSeeedColorSensor(PacketPayload &payload, QJsonObject params);     // 213
    void encodeGetSeeedColorSensor(PacketPayload &payload, QJsonObject params);     // 213
    void encodeSetSeeedRGBLed(PacketPayload &payload, QJsonObject params);          // 214
    void encodeGetMCUTemperature(PacketPayload &payload, QJsonObject params);       // 215

    /* id:240~246 */
    void encodeSetQueuedCmdStart(PacketPayload &payload, QJsonObject params);       // 240
    void encodeSetQueuedCmdStop(PacketPayload &payload, QJsonObject params);        // 241
    void encodeSetQueuedCmdForceStop(PacketPayload &payload, QJsonObject params);   // 242
    void encodeSetQueuedCmdStartDownload(PacketPayload &payload, QJsonObject params);//243
    void encodeSetQueuedCmdStopDownload(PacketPayload &payload, QJsonObject params);// 244
    void encodeSetQueuedCmdClear(PacketPayload &payload, QJsonObject params);       // 245
    void encodeGetQueuedCmdCurrentIndex(PacketPayload &payload, QJsonObject params);// 246
    void encodeGetQueuedCmdLeftSpace(PacketPayload &payload, QJsonObject params);   // 247

    /* Dobot Sensor id:216 subid:1~53 */
    void encodeSensorRGBLEDInit(PacketPayload &payload, QJsonObject params);        // 216-1
    void encodeSensorSetRGBLEDVlaue(PacketPayload &payload, QJsonObject params);    // 216-2
    void encodeSensorRGBLEDTurnOFFAll(PacketPayload &payload, QJsonObject params);  // 216-3
    void encodeSensorSetRGBLEDState(PacketPayload &payload, QJsonObject params);    // 216-4
    void encodeSensorOledInit(PacketPayload &payload, QJsonObject params);          // 216-10
    void encodeSensorOledClear(PacketPayload &payload, QJsonObject params);         // 216-11
    void encodeSensorOledDisplay(PacketPayload &payload, QJsonObject params);       // 216-12
    void encodeSensorKnobInit(PacketPayload &payload, QJsonObject params);          // 216-20
    void encodeSensorGetKnob(PacketPayload &payload, QJsonObject params);           // 216-21
    void encodeSensorVoiceInit(PacketPayload &payload, QJsonObject params);         // 216-22
    void encodeSensorGetVoice(PacketPayload &payload, QJsonObject params);          // 216-23
    void encodeSensorLightInit(PacketPayload &payload, QJsonObject params);         // 216-24
    void encodeSensorGetLight(PacketPayload &payload, QJsonObject params);          // 216-25
    void encodeSensorPEInit(PacketPayload &payload, QJsonObject params);            // 216-26
    void encodeSensorGetPEState(PacketPayload &payload, QJsonObject params);        // 216-27
    void encodeSensorUltrasonicInit(PacketPayload &payload, QJsonObject params);    // 216-28
    void encodeSensorUltrasonicGet(PacketPayload &payload, QJsonObject params);     // 216-29
    void encodeSensorSHT31Init(PacketPayload &payload, QJsonObject params);         // 216-30
    void encodeSensorGetSHT31(PacketPayload &payload, QJsonObject params);          // 216-31
    void encodeSensorColorInit(PacketPayload &payload, QJsonObject params);         // 216-32
    void encodeSensorGetColor(PacketPayload &payload, QJsonObject params);          // 216-33
    void encodeSensorGetColorRes(PacketPayload &payload, QJsonObject params);       // 216-34
    void encodeSensorXBeeInit(PacketPayload &payload, QJsonObject params);          // 216-40
    void encodeSensorXBeeSend(PacketPayload &payload, QJsonObject params);          // 216-41
    void encodeSensorXBeeReceive(PacketPayload &payload, QJsonObject params);       // 216-42
    void encodeSensorXBeeClear(PacketPayload &payload, QJsonObject params);         // 216-43
    void encodeSensorSYNInit(PacketPayload &payload, QJsonObject params);           // 216-50
    void encodeSensorSetSYN(PacketPayload &payload, QJsonObject params);            // 216-51
    void encodeSensorSetSYNMusic(PacketPayload &payload, QJsonObject params);       // 216-52
    void encodeSensorSendSYN(PacketPayload &payload, QJsonObject params);           // 216-53
    void encodeSensorSetSYNCmd(PacketPayload &payload, QJsonObject params);         // 216-54

    /* Dobot Ble id:216 subid:60~68 */
    void encodeSetBleInf(PacketPayload &payload, QJsonObject params);               // 216-60
    void encodeGetBleInf(PacketPayload &payload, QJsonObject params);               // 216-60
    void encodeBleConnect(PacketPayload &payload, QJsonObject params);              // 216-61
    void encodeBleReadOneData(PacketPayload &payload, QJsonObject params);          // 216-63
    void encodeBleWriteOneData(PacketPayload &payload, QJsonObject params);         // 216-64
    void encodeSetBleMesh(PacketPayload &payload, QJsonObject params);              // 216-65
    void encodeBleReadMeshData(PacketPayload &payload, QJsonObject params);         // 216-66
    void encodeBleWriteMeshData(PacketPayload &payload, QJsonObject params);        // 216-67
    void encodeBleClearMeshData(PacketPayload &payload, QJsonObject params);        // 216-68

    /* Dobot Sensor id:216 subid:70~72 传感器*/
    void encodeSetSensorHandModel(PacketPayload &payload, QJsonObject params);      // 216-70
    void encodeGetSensorHandModel(PacketPayload &payload, QJsonObject params);      // 216-71
    void encodeIsSensorHandModel(PacketPayload &payload, QJsonObject params);       // 216-72

    /* Dobot Sensor id:216 subid:73~75 摇杆*/
    void encodeIsJoystickButton(PacketPayload &payload, QJsonObject params);        // 216-73
    void encodeGetJoystickPos(PacketPayload &payload, QJsonObject params);          // 216-74
    void encodeGetJoystickButton(PacketPayload &payload, QJsonObject params);       // 216-75

    /* Dobot Sensor id:216 subid:76 人体红外*/
    void encodeIsPirDetected(PacketPayload &payload, QJsonObject params);           // 216-76
    /* Dobot Sensor id:216 subid:77 读取红蓝按钮状态*/
    void encodeGetButtonStatus(PacketPayload &payload, QJsonObject params);         // 216-77
    /* Dobot Sensor id:216 subid:78 读取红按钮状态*/
    void encodeGetRedButtonStatus(PacketPayload &payload, QJsonObject params);      // 216-78
    /* Dobot Sensor id:216 subid:79 读取蓝按钮状态*/
    void encodeGetBlueButtonStatus(PacketPayload &payload, QJsonObject params);     // 216-79

    /* Dobot Ble id:217 subid:10~20 GO相关*/
    void encodeSetStopPointParam(PacketPayload &payload, QJsonObject params);       // 217-10
    void encodeSetStopPointServer(PacketPayload &payload, QJsonObject params);      // 217-11
    void encodeGetStopPointState(PacketPayload &payload, QJsonObject params);       // 217-12
    void encodeGetImgToArmXY(PacketPayload &payload, QJsonObject params);           // 217-20

public:
#pragma pack(push, 1)
    struct PoseStruct {
        float x;
        float y;
        float z;
        float r;
        float jointAngle[4];
    };

    struct PositionStruct {
        float x;
        float y;
        float z;
        float r;
    };

    struct VelocityStruct {
        float velocity;
        float acceleration;
    };

    struct VelocityArrayStruct {
        float velocity[4];
        float acceleration[4];
    };

    struct VelocityRatioStruct {
        float velocityRatio;
        float accelerationRatio;
    };

    struct PTPJump2ParamsStruct {
        float startJumpHeight;
        float endJumpHeight;
        float zLimit;
    };

    /* COLOR */
    struct ColorStruct {
        quint8 red;
        quint8 green;
        quint8 blue;
    };

    struct SeeedColorStruct {
        quint16 red;
        quint16 green;
        quint16 blue;
        quint16 cct;
    };

    struct GetButtonStatusStruct
    {
        quint8 redBtn;
        quint8 blueBtn;
    };

    struct AngleSensorStruct
    {
        float rearArmAngle;
        float frontArmAngle;
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
        quint8 isRealTimeTrack;
    };

    struct CP2CmdStruct {
        quint8 cpMode;
        float x;
        float y;
        float z;
    };

    /* PTP */
    struct PTPCmdStruct {
        quint8 ptpMode;
        float x;
        float y;
        float z;
        float r;
    };

    struct PTPCoordinateParamsStruct {
        float xyzVelocity;
        float rVelocity;
        float xyzAcceleration;
        float rAcceleration;
    };

    struct PTPWithLCmdStruct {
        quint8 ptpMode;
        float x;
        float y;
        float z;
        float r;
        float l;
    };

    struct POCmdStruct {
        quint8 ratio;
        quint16 port;
        quint8 level;
    };

    struct TRIGCmdStruct {
        quint8 port;
        quint8 mode;
        quint8 condition;
        quint16 threshold;
    };

    struct IOPWMStruct {
        quint8 port;
        float frequency;
        float dutyCycle;
    };

    struct EMotorStruct {
        quint8 index;
        quint8 isEnabled;
        int speed;
    };

    struct EMotorSStruct {
        quint8 index;
        quint8 isEnabled;
        int speed;
        quint32 distance;
    };

    /* Collision */
    struct CollisionCheckStruct {
        quint8 enable;
        float threshold;
    };

    /* WIFI */
    struct WIFIIPAddressStruct {
        quint8 dhcp;
        quint8 address[4];
    };

    struct WifiAddressStruct {
        quint8 address[4];
    };

    struct ColorObjCoordinateStruct {
        quint8 color;
        quint16 coordinate;
    };

    struct StopPointState{
        uint8_t subid;
        bool isstop;
    };

    struct GetImgToArmXY {
        uint8_t subid;
        float armX;
        float armY;
        uint8_t okflag;

    };

    struct SensorHandModel {
        quint8 subid;
        quint8 res;
    };

    struct JoystickPos {
        quint8 x;
        quint8 y;
        quint8 z;
    };


#pragma pack(pop)
};

#endif // DMAGICIANPROTOCOL_H
