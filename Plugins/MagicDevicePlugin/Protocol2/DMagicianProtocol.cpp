#include "DMagicianProtocol.h"

#include <QDataStream>
#include <QJsonArray>
#include <QDateTime>
#include <QTime>
//#include <QtEndian>
#include <QDebug>

DMagicianProtocol *DMagicianProtocol::getInstance()
{
    static DMagicianProtocol *instance = nullptr;
    if (instance == nullptr) {
        instance = new DMagicianProtocol();
    }
    return instance;
}

DMagicianProtocol::DMagicianProtocol(QObject *parent) : QObject(parent)
{
    _encodeFunctionInit();
    _decodeFunctionInit();
    _decodeDobotSensorFunctionInit();
    _decodeBoxFuncInit();
}

void DMagicianProtocol::_encodeFunctionInit()
{
    /* id:0~7 */
    m_encodeFuncMap.insert("SetDeviceSN", &DMagicianProtocol::encodeSetDeviceSN);
    m_encodeFuncMap.insert("GetDeviceSN", &DMagicianProtocol::encodeGetDeviceSN);
    m_encodeFuncMap.insert("SetDeviceName", &DMagicianProtocol::encodeSetDeviceName);
    m_encodeFuncMap.insert("GetDeviceName", &DMagicianProtocol::encodeGetDeviceName);
    m_encodeFuncMap.insert("GetDeviceVersion", &DMagicianProtocol::encodeGetDeviceVersion);
    m_encodeFuncMap.insert("SetDeviceWithL", &DMagicianProtocol::encodeSetDeviceWithL);
    m_encodeFuncMap.insert("GetDeviceWithL", &DMagicianProtocol::encodeGetDeviceWithL);
    m_encodeFuncMap.insert("GetDeviceTime", &DMagicianProtocol::encodeGetDeviceTime);
    m_encodeFuncMap.insert("GetDeviceID", &DMagicianProtocol::encodeGetDeviceID);
    m_encodeFuncMap.insert("GetProductName", &DMagicianProtocol::encodeGetProductName);
    m_encodeFuncMap.insert("SetOLEDDisplay", &DMagicianProtocol::encodeSetOLEDDisplay);
    m_encodeFuncMap.insert("SetProgressBar", &DMagicianProtocol::encodeSetProgressBar);

    /* id:10~15 */
    m_encodeFuncMap.insert("GetPose", &DMagicianProtocol::encodeGetPose);
    m_encodeFuncMap.insert("ResetPose", &DMagicianProtocol::encodeResetPose);
    m_encodeFuncMap.insert("GetPoseL", &DMagicianProtocol::encodeGetPoseL);
    m_encodeFuncMap.insert("CheckPoseLimit", &DMagicianProtocol::encodeCheckPoseLimit);

    /* id:20 */
    m_encodeFuncMap.insert("GetAlarmsState", &DMagicianProtocol::encodeGetAlarmsState);
    m_encodeFuncMap.insert("ClearAllAlarmsState", &DMagicianProtocol::encodeClearAllAlarmsState);

    /* id:30~33 */
    m_encodeFuncMap.insert("SetHOMEParams", &DMagicianProtocol::encodeSetHOMEParams);
    m_encodeFuncMap.insert("GetHOMEParams", &DMagicianProtocol::encodeGetHOMEParams);
    m_encodeFuncMap.insert("SetHOMECmd", &DMagicianProtocol::encodeSetHOMECmd);
    m_encodeFuncMap.insert("SetAutoLeveling", &DMagicianProtocol::encodeSetAutoLeveling);
    m_encodeFuncMap.insert("GetAutoLeveling", &DMagicianProtocol::encodeGetAutoLeveling);
    m_encodeFuncMap.insert("GetGoHomeStatus", &DMagicianProtocol::encodeGetGoHomeStatus);

    /* id:40~42 */
    m_encodeFuncMap.insert("SetHHTTrigMode", &DMagicianProtocol::encodeSetHHTTrigMode);
    m_encodeFuncMap.insert("GetHHTTrigMode", &DMagicianProtocol::encodeGetHHTTrigMode);
    m_encodeFuncMap.insert("SetHHTTrigOutputEnabled", &DMagicianProtocol::encodeSetHHTTrigOutputEnabled);
    m_encodeFuncMap.insert("GetHHTTrigOutputEnabled", &DMagicianProtocol::encodeGetHHTTrigOutputEnabled);
    m_encodeFuncMap.insert("GetHHTTrigOutput", &DMagicianProtocol::encodeGetHHTTrigOutput);

    /* id:62~69 */
    m_encodeFuncMap.insert("SetEndEffectorParams", &DMagicianProtocol::encodeSetEndEffectorParams);
    m_encodeFuncMap.insert("GetEndEffectorParams", &DMagicianProtocol::encodeGetEndEffectorParams);
    m_encodeFuncMap.insert("SetEndEffectorLaser", &DMagicianProtocol::encodeSetEndEffectorLaser);
    m_encodeFuncMap.insert("GetEndEffectorLaser", &DMagicianProtocol::encodeGetEndEffectorLaser);
    m_encodeFuncMap.insert("SetEndEffectorSuctionCup", &DMagicianProtocol::encodeSetEndEffectorSuctionCup);
    m_encodeFuncMap.insert("GetEndEffectorSuctionCup", &DMagicianProtocol::encodeGetEndEffectorSuctionCup);
    m_encodeFuncMap.insert("SetEndEffectorGripper", &DMagicianProtocol::encodeSetEndEffectorGripper);
    m_encodeFuncMap.insert("GetEndEffectorGripper", &DMagicianProtocol::encodeGetEndEffectorGripper);
    m_encodeFuncMap.insert("SetEndEffectorType", &DMagicianProtocol::encodeSetEndEffectorType);
    m_encodeFuncMap.insert("GetEndEffectorType", &DMagicianProtocol::encodeGetEndEffectorType);
    m_encodeFuncMap.insert("SetServoAngle", &DMagicianProtocol::encodeSetServoAngle);
    m_encodeFuncMap.insert("GetServoAngle", &DMagicianProtocol::encodeGetServoAngle);
    m_encodeFuncMap.insert("SetArmSpeedRatio", &DMagicianProtocol::encodeSetArmSpeedRatio);
    m_encodeFuncMap.insert("GetArmSpeedRatio", &DMagicianProtocol::encodeGetArmSpeedRatio);
    m_encodeFuncMap.insert("SetLSpeedRatio", &DMagicianProtocol::encodeSetLSpeedRatio);
    m_encodeFuncMap.insert("GetLSpeedRatio", &DMagicianProtocol::encodeGetLSpeedRatio);
    m_encodeFuncMap.insert("GetConfigAndStatus", &DMagicianProtocol::encodeGetConfigAndStatus);
    m_encodeFuncMap.insert("GetRTStatus", &DMagicianProtocol::encodeGetRTStatus);

    /* id:70~75 */
    m_encodeFuncMap.insert("SetJOGJointParams", &DMagicianProtocol::encodeSetJOGJointParams);
    m_encodeFuncMap.insert("GetJOGJointParams", &DMagicianProtocol::encodeGetJOGJointParams);
    m_encodeFuncMap.insert("SetJOGCoordinateParams", &DMagicianProtocol::encodeSetJOGCoordinateParams);
    m_encodeFuncMap.insert("GetJOGCoordinateParams", &DMagicianProtocol::encodeGetJOGCoordinateParams);
    m_encodeFuncMap.insert("SetJOGCommonParams", &DMagicianProtocol::encodeSetJOGCommonParams);
    m_encodeFuncMap.insert("GetJOGCommonParams", &DMagicianProtocol::encodeGetJOGCommonParams);
    m_encodeFuncMap.insert("SetJOGCmd", &DMagicianProtocol::encodeSetJOGCmd);
    m_encodeFuncMap.insert("SetJOGLParams", &DMagicianProtocol::encodeSetJOGLParams);
    m_encodeFuncMap.insert("GetJOGLParams", &DMagicianProtocol::encodeGetJOGLParams);
    m_encodeFuncMap.insert("SetJOGCmdTime", &DMagicianProtocol::encodeSetJOGCmdTime);

    /* id:80~89 */
    m_encodeFuncMap.insert("SetPTPJointParams", &DMagicianProtocol::encodeSetPTPJointParams);
    m_encodeFuncMap.insert("GetPTPJointParams", &DMagicianProtocol::encodeGetPTPJointParams);
    m_encodeFuncMap.insert("SetPTPCoordinateParams", &DMagicianProtocol::encodeSetPTPCoordinateParams);
    m_encodeFuncMap.insert("GetPTPCoordinateParams", &DMagicianProtocol::encodeGetPTPCoordinateParams);
    m_encodeFuncMap.insert("SetPTPJumpParams", &DMagicianProtocol::encodeSetPTPJumpParams);
    m_encodeFuncMap.insert("GetPTPJumpParams", &DMagicianProtocol::encodeGetPTPJumpParams);
    m_encodeFuncMap.insert("SetPTPCommonParams", &DMagicianProtocol::encodeSetPTPCommonParams);
    m_encodeFuncMap.insert("GetPTPCommonParams", &DMagicianProtocol::encodeGetPTPCommonParams);
    m_encodeFuncMap.insert("SetPTPCmd", &DMagicianProtocol::encodeSetPTPCmd);
    m_encodeFuncMap.insert("SetPTPLParams", &DMagicianProtocol::encodeSetPTPLParams);
    m_encodeFuncMap.insert("GetPTPLParams", &DMagicianProtocol::encodeGetPTPLParams);
    m_encodeFuncMap.insert("SetPTPWithLCmd", &DMagicianProtocol::encodeSetPTPWithLCmd);
    m_encodeFuncMap.insert("SetPTPJump2Params", &DMagicianProtocol::encodeSetPTPJump2Params);
    m_encodeFuncMap.insert("GetPTPJump2Params", &DMagicianProtocol::encodeGetPTPJump2Params);
    m_encodeFuncMap.insert("SetPTPPOCmd", &DMagicianProtocol::encodeSetPTPPOCmd);
    m_encodeFuncMap.insert("SetPTPPOWithLCmd", &DMagicianProtocol::encodeSetPTPPOWithLCmd);

    /* id:90~95 */
    m_encodeFuncMap.insert("SetCPParams", &DMagicianProtocol::encodeSetCPParams);
    m_encodeFuncMap.insert("GetCPParams", &DMagicianProtocol::encodeGetCPParams);
    m_encodeFuncMap.insert("SetCPCmd", &DMagicianProtocol::encodeSetCPCmd);
    m_encodeFuncMap.insert("SetCPLECmd", &DMagicianProtocol::encodeSetCPLECmd);
    m_encodeFuncMap.insert("SetCPRHeadHoldEnable", &DMagicianProtocol::encodeSetCPRHeadHoldEnable);
    m_encodeFuncMap.insert("GetCPRHeadHoldEnable", &DMagicianProtocol::encodeGetCPRHeadHoldEnable);
    m_encodeFuncMap.insert("SetCPCommonParams", &DMagicianProtocol::encodeSetCPCommonParams);
    m_encodeFuncMap.insert("GetCPCommonParams", &DMagicianProtocol::encodeGetCPCommonParams);
    m_encodeFuncMap.insert("SetCP2Cmd", &DMagicianProtocol::encodeSetCP2Cmd);

    /* id:100~103 */
    m_encodeFuncMap.insert("SetARCParams", &DMagicianProtocol::encodeSetARCParams);
    m_encodeFuncMap.insert("GetARCParams", &DMagicianProtocol::encodeGetARCParams);
    m_encodeFuncMap.insert("SetARCCmd", &DMagicianProtocol::encodeSetARCCmd);
    m_encodeFuncMap.insert("SetCircleCmd", &DMagicianProtocol::encodeSetCircleCmd);
    m_encodeFuncMap.insert("SetARCCommonParams", &DMagicianProtocol::encodeSetARCCommonParams);
    m_encodeFuncMap.insert("GetARCCommonParams", &DMagicianProtocol::encodeGetARCCommonParams);

    /* id:110 */
    m_encodeFuncMap.insert("SetWAITCmd", &DMagicianProtocol::encodeSetWAITCmd);

    /* id:120 */
    m_encodeFuncMap.insert("SetTRIGCmd", &DMagicianProtocol::encodeSetTRIGCmd);

    /* id:130~139 */
    m_encodeFuncMap.insert("SetIOMultiplexing", &DMagicianProtocol::encodeSetIOMultiplexing);
    m_encodeFuncMap.insert("GetIOMultiplexing", &DMagicianProtocol::encodeGetIOMultiplexing);
    m_encodeFuncMap.insert("SetIODO", &DMagicianProtocol::encodeSetIODO);
    m_encodeFuncMap.insert("GetIODO", &DMagicianProtocol::encodeGetIODO);
    m_encodeFuncMap.insert("SetIOPWM", &DMagicianProtocol::encodeSetIOPWM);
    m_encodeFuncMap.insert("GetIOPWM", &DMagicianProtocol::encodeGetIOPWM);
    m_encodeFuncMap.insert("GetIODI", &DMagicianProtocol::encodeGetIODI);
    m_encodeFuncMap.insert("GetIOADC", &DMagicianProtocol::encodeGetIOADC);
    m_encodeFuncMap.insert("SetEMotor", &DMagicianProtocol::encodeSetEMotor);
    m_encodeFuncMap.insert("SetEMotorS", &DMagicianProtocol::encodeSetEMotorS);
    m_encodeFuncMap.insert("SetColorSensor", &DMagicianProtocol::encodeSetColorSensor);
    m_encodeFuncMap.insert("GetColorSensor", &DMagicianProtocol::encodeGetColorSensor);
    m_encodeFuncMap.insert("SetInfraredSensor", &DMagicianProtocol::encodeSetInfraredSensor);
    m_encodeFuncMap.insert("GetInfraredSensor", &DMagicianProtocol::encodeGetInfraredSensor);
    m_encodeFuncMap.insert("SetIICInfo", &DMagicianProtocol::encodeSetIICInfo);
    m_encodeFuncMap.insert("GetIICInfo", &DMagicianProtocol::encodeGetIICInfo);

    /* id:140~143 */
    m_encodeFuncMap.insert("SetAngleSensorStaticError", &DMagicianProtocol::encodeSetAngleSensorStaticError);
    m_encodeFuncMap.insert("GetAngleSensorStaticError", &DMagicianProtocol::encodeGetAngleSensorStaticError);
    m_encodeFuncMap.insert("SetAngleSensorCoef", &DMagicianProtocol::encodeSetAngleSensorCoef);
    m_encodeFuncMap.insert("GetAngleSensorCoef", &DMagicianProtocol::encodeGetAngleSensorCoef);
    m_encodeFuncMap.insert("SetBaseDecoderStaticError", &DMagicianProtocol::encodeSetBaseDecoderStaticError);
    m_encodeFuncMap.insert("GetBaseDecoderStaticError", &DMagicianProtocol::encodeGetBaseDecoderStaticError);
    m_encodeFuncMap.insert("SetLRHandCalibrateValue", &DMagicianProtocol::encodeSetLRHandCalibrateValue);
    m_encodeFuncMap.insert("GetLRHandCalibrateValue", &DMagicianProtocol::encodeGetLRHandCalibrateValue);

    /* id:150~157 */
    m_encodeFuncMap.insert("SetWIFIConfigMode", &DMagicianProtocol::encodeSetWIFIConfigMode);
    m_encodeFuncMap.insert("GetWIFIConfigMode", &DMagicianProtocol::encodeGetWIFIConfigMode);
    m_encodeFuncMap.insert("SetWIFISSID", &DMagicianProtocol::encodeSetWIFISSID);
    m_encodeFuncMap.insert("GetWIFISSID", &DMagicianProtocol::encodeGetWIFISSID);
    m_encodeFuncMap.insert("SetWIFIPassword", &DMagicianProtocol::encodeSetWIFIPassword);
    m_encodeFuncMap.insert("GetWIFIPassword", &DMagicianProtocol::encodeGetWIFIPassword);
    m_encodeFuncMap.insert("SetWIFIIPAddress", &DMagicianProtocol::encodeSetWIFIIPAddress);
    m_encodeFuncMap.insert("GetWIFIIPAddress", &DMagicianProtocol::encodeGetWIFIIPAddress);
    m_encodeFuncMap.insert("SetWIFINetmask", &DMagicianProtocol::encodeSetWIFINetmask);
    m_encodeFuncMap.insert("GetWIFINetmask", &DMagicianProtocol::encodeGetWIFINetmask);
    m_encodeFuncMap.insert("SetWIFIGateway", &DMagicianProtocol::encodeSetWIFIGateway);
    m_encodeFuncMap.insert("GetWIFIGateway", &DMagicianProtocol::encodeGetWIFIGateway);
    m_encodeFuncMap.insert("SetWIFIDNS", &DMagicianProtocol::encodeSetWIFIDNS);
    m_encodeFuncMap.insert("GetWIFIDNS", &DMagicianProtocol::encodeGetWIFIDNS);
    m_encodeFuncMap.insert("GetWIFIConnectStatus", &DMagicianProtocol::encodeGetWIFIConnectStatus);

    /* id:170~171 */
    m_encodeFuncMap.insert("SetLostStepValue", &DMagicianProtocol::encodeSetLostStepValue);
    m_encodeFuncMap.insert("SetLostStepCmd", &DMagicianProtocol::encodeSetLostStepCmd);
    m_encodeFuncMap.insert("SetCollisionCheck", &DMagicianProtocol::encodeSetCollisionCheck);
    m_encodeFuncMap.insert("GetCollisionCheck", &DMagicianProtocol::encodeGetCollisionCheck);

    /* id:192~198 */
    m_encodeFuncMap.insert("GetDeviceStatus", &DMagicianProtocol::encodeGetDeviceStatus);
    m_encodeFuncMap.insert("SetDeviceRunAPP", &DMagicianProtocol::encodeSetDeviceRunAPP);
    m_encodeFuncMap.insert("SetUpgradeStatus", &DMagicianProtocol::encodeSetUpgradeStatus);
    m_encodeFuncMap.insert("GetUpgradeStatus", &DMagicianProtocol::encodeGetUpgradeStatus);
    m_encodeFuncMap.insert("SetRunUpgrade", &DMagicianProtocol::encodeSetRunUpgrade);
    m_encodeFuncMap.insert("SetStartTransmit", &DMagicianProtocol::encodeSetStartTransmit);
    m_encodeFuncMap.insert("SetUpgradeVeify", &DMagicianProtocol::encodeSetUpgradeVeify);
    m_encodeFuncMap.insert("SetDeviceRestart", &DMagicianProtocol::encodeSetDeviceRestart);

    /* id:200 */
    m_encodeFuncMap.insert("GetColorObjExist", &DMagicianProtocol::encodeGetColorObjExist);
    m_encodeFuncMap.insert("GetColorObjCoordinate", &DMagicianProtocol::encodeGetColorObjCoordinate);

    /* id:210~215 */
    m_encodeFuncMap.insert("GetSeeedDistanceSensor", &DMagicianProtocol::encodeGetSeeedDistanceSensor);
    m_encodeFuncMap.insert("SetSeeedTempSensor", &DMagicianProtocol::encodeSetSeeedTempSensor);
    m_encodeFuncMap.insert("GetSeeedTempSensor", &DMagicianProtocol::encodeGetSeeedTempSensor);
    m_encodeFuncMap.insert("SetSeeedLightSensor", &DMagicianProtocol::encodeSetSeeedLightSensor);
    m_encodeFuncMap.insert("GetSeeedLightSensor", &DMagicianProtocol::encodeGetSeeedLightSensor);
    m_encodeFuncMap.insert("SetSeeedColorSensor", &DMagicianProtocol::encodeSetSeeedColorSensor);
    m_encodeFuncMap.insert("GetSeeedColorSensor", &DMagicianProtocol::encodeGetSeeedColorSensor);
    m_encodeFuncMap.insert("SetSeeedRGBLed", &DMagicianProtocol::encodeSetSeeedRGBLed);
    m_encodeFuncMap.insert("GetMCUTemperature", &DMagicianProtocol::encodeGetMCUTemperature);

    /* id:240~246 */
    m_encodeFuncMap.insert("QueuedCmdStart", &DMagicianProtocol::encodeSetQueuedCmdStart);
    m_encodeFuncMap.insert("QueuedCmdStop", &DMagicianProtocol::encodeSetQueuedCmdStop);
    m_encodeFuncMap.insert("QueuedCmdForceStop", &DMagicianProtocol::encodeSetQueuedCmdForceStop);
    m_encodeFuncMap.insert("QueuedCmdStartDownload", &DMagicianProtocol::encodeSetQueuedCmdStartDownload);
    m_encodeFuncMap.insert("QueuedCmdStopDownload", &DMagicianProtocol::encodeSetQueuedCmdStopDownload);
    m_encodeFuncMap.insert("QueuedCmdClear", &DMagicianProtocol::encodeSetQueuedCmdClear);
    m_encodeFuncMap.insert("GetQueuedCmdCurrentIndex", &DMagicianProtocol::encodeGetQueuedCmdCurrentIndex);
    m_encodeFuncMap.insert("GetQueuedCmdLeftSpace", &DMagicianProtocol::encodeGetQueuedCmdLeftSpace);

    /* Dobot Sensor id:216 subid:1~53 */
    m_encodeFuncMap.insert("SensorRGBLEDInit", &DMagicianProtocol::encodeSensorRGBLEDInit);
    m_encodeFuncMap.insert("SensorSetRGBLEDVlaue", &DMagicianProtocol::encodeSensorSetRGBLEDVlaue);
    m_encodeFuncMap.insert("SensorRGBLEDTurnOFFAll", &DMagicianProtocol::encodeSensorRGBLEDTurnOFFAll);
    m_encodeFuncMap.insert("SensorSetRGBLEDState", &DMagicianProtocol::encodeSensorSetRGBLEDState);
    m_encodeFuncMap.insert("SensorOledInit", &DMagicianProtocol::encodeSensorOledInit);
    m_encodeFuncMap.insert("SensorOledClear", &DMagicianProtocol::encodeSensorOledClear);
    m_encodeFuncMap.insert("SensorOledDisplay", &DMagicianProtocol::encodeSensorOledDisplay);
    m_encodeFuncMap.insert("SensorKnobInit", &DMagicianProtocol::encodeSensorKnobInit);
    m_encodeFuncMap.insert("SensorGetKnob", &DMagicianProtocol::encodeSensorGetKnob);
    m_encodeFuncMap.insert("SensorVoiceInit", &DMagicianProtocol::encodeSensorVoiceInit);
    m_encodeFuncMap.insert("SensorGetVoice", &DMagicianProtocol::encodeSensorGetVoice);
    m_encodeFuncMap.insert("SensorLightInit", &DMagicianProtocol::encodeSensorLightInit);
    m_encodeFuncMap.insert("SensorGetLight", &DMagicianProtocol::encodeSensorGetLight);
    m_encodeFuncMap.insert("SensorPEInit", &DMagicianProtocol::encodeSensorPEInit);
    m_encodeFuncMap.insert("SensorGetPEState", &DMagicianProtocol::encodeSensorGetPEState);
    m_encodeFuncMap.insert("SensorUltrasonicInit", &DMagicianProtocol::encodeSensorUltrasonicInit);
    m_encodeFuncMap.insert("SensorUltrasonicGet", &DMagicianProtocol::encodeSensorUltrasonicGet);
    m_encodeFuncMap.insert("SensorSHT31Init", &DMagicianProtocol::encodeSensorSHT31Init);
    m_encodeFuncMap.insert("SensorGetSHT31", &DMagicianProtocol::encodeSensorGetSHT31);
    m_encodeFuncMap.insert("SensorColorInit", &DMagicianProtocol::encodeSensorColorInit);
    m_encodeFuncMap.insert("SensorGetColor", &DMagicianProtocol::encodeSensorGetColor);
    m_encodeFuncMap.insert("SensorGetColorRes", &DMagicianProtocol::encodeSensorGetColorRes);
    m_encodeFuncMap.insert("SensorXBeeInit", &DMagicianProtocol::encodeSensorXBeeInit);
    m_encodeFuncMap.insert("SensorXBeeSend", &DMagicianProtocol::encodeSensorXBeeSend);
    m_encodeFuncMap.insert("SensorXBeeReceive", &DMagicianProtocol::encodeSensorXBeeReceive);
    m_encodeFuncMap.insert("SensorXBeeClear", &DMagicianProtocol::encodeSensorXBeeClear);
    m_encodeFuncMap.insert("SensorSYNInit", &DMagicianProtocol::encodeSensorSYNInit);
    m_encodeFuncMap.insert("SensorSetSYN", &DMagicianProtocol::encodeSensorSetSYN);
    m_encodeFuncMap.insert("SensorSetSYNMusic", &DMagicianProtocol::encodeSensorSetSYNMusic);
    m_encodeFuncMap.insert("SensorSendSYN", &DMagicianProtocol::encodeSensorSendSYN);
    m_encodeFuncMap.insert("SensorSetSYNCmd", &DMagicianProtocol::encodeSensorSetSYNCmd);

    /* Dobot Ble id:216 subid:60~68 */
    m_encodeFuncMap.insert("SetBleInf", &DMagicianProtocol::encodeSetBleInf);
    m_encodeFuncMap.insert("GetBleInf", &DMagicianProtocol::encodeGetBleInf);
    m_encodeFuncMap.insert("BleConnect", &DMagicianProtocol::encodeBleConnect);
    m_encodeFuncMap.insert("BleReadOneData", &DMagicianProtocol::encodeBleReadOneData);
    m_encodeFuncMap.insert("BleWriteOneData", &DMagicianProtocol::encodeBleWriteOneData);
    m_encodeFuncMap.insert("SetBleMesh", &DMagicianProtocol::encodeSetBleMesh);
    m_encodeFuncMap.insert("BleReadMeshData", &DMagicianProtocol::encodeBleReadMeshData);
    m_encodeFuncMap.insert("BleWriteMeshData", &DMagicianProtocol::encodeBleWriteMeshData);
    m_encodeFuncMap.insert("BleClearMeshData", &DMagicianProtocol::encodeBleClearMeshData);

    /* Dobot Sesnor id:216 subid:70~72 */
    m_encodeFuncMap.insert("SetSensorHandModel", &DMagicianProtocol::encodeSetSensorHandModel);
    m_encodeFuncMap.insert("GetSensorHandModel", &DMagicianProtocol::encodeGetSensorHandModel);
    m_encodeFuncMap.insert("IsSensorHandModel", &DMagicianProtocol::encodeIsSensorHandModel);

    /* Dobot Sesnor id:216 subid:73~75 */
    m_encodeFuncMap.insert("IsJoystickButton", &DMagicianProtocol::encodeIsJoystickButton);
    m_encodeFuncMap.insert("GetJoystickButton", &DMagicianProtocol::encodeGetJoystickButton);
    m_encodeFuncMap.insert("GetJoystickPos", &DMagicianProtocol::encodeGetJoystickPos);

    /* Dobot Sesnor id:216 subid:76 */
    m_encodeFuncMap.insert("IsPirDetected", &DMagicianProtocol::encodeIsPirDetected);
    /* Dobot Sesnor id:216 subid:77 */
    m_encodeFuncMap.insert("GetButtonStatus", &DMagicianProtocol::encodeGetButtonStatus);
    /* Dobot Sesnor id:216 subid:78 */
    m_encodeFuncMap.insert("GetRedButtonStatus", &DMagicianProtocol::encodeGetRedButtonStatus);
    /* Dobot Sesnor id:216 subid:79 */
    m_encodeFuncMap.insert("GetBlueButtonStatus", &DMagicianProtocol::encodeGetBlueButtonStatus);

    //216-90~95
    m_encodeFuncMap.insert("BluetoothSetMaster", &DMagicianProtocol::encodeBleSetMaster);
    m_encodeFuncMap.insert("BluetoothSetSlave", &DMagicianProtocol::encodeBleSetSlave);
    m_encodeFuncMap.insert("BluetoothRecvData", &DMagicianProtocol::encodeBleRecvData);
    m_encodeFuncMap.insert("BluetoothSendData", &DMagicianProtocol::encodeBleSendData);
    m_encodeFuncMap.insert("BluetoothClearCacheData", &DMagicianProtocol::encodeBleClearCacheData);
    m_encodeFuncMap.insert("BluetoothGetMacAddress", &DMagicianProtocol::encodeBleGetMacAddress);

    /* GO id:217 subid:10~20 */
    m_encodeFuncMap.insert("SetStopPointParam", &DMagicianProtocol::encodeSetStopPointParam);
    m_encodeFuncMap.insert("SetStopPointServer", &DMagicianProtocol::encodeSetStopPointServer);
    m_encodeFuncMap.insert("GetStopPointState", &DMagicianProtocol::encodeGetStopPointState);
    m_encodeFuncMap.insert("GetImgToArmXY", &DMagicianProtocol::encodeGetImgToArmXY);
}

void DMagicianProtocol::_decodeFunctionInit()
{
    m_decodeFuncMap.insert(0, &DMagicianProtocol::decodeDeviceSN);
    m_decodeFuncMap.insert(1, &DMagicianProtocol::decodeDeviceName);
    m_decodeFuncMap.insert(2, &DMagicianProtocol::decodeDeviceVersion);
    m_decodeFuncMap.insert(3, &DMagicianProtocol::decodeDeviceWithL);
    m_decodeFuncMap.insert(4, &DMagicianProtocol::decodeDeviceTime);
    m_decodeFuncMap.insert(5, &DMagicianProtocol::decodeDeviceID);
    m_decodeFuncMap.insert(6, &DMagicianProtocol::decodeDeviceInfo);
    m_decodeFuncMap.insert(7, &DMagicianProtocol::decodeProductName);
    m_decodeFuncMap.insert(8, &DMagicianProtocol::decodeOLEDDisplay);
    m_decodeFuncMap.insert(9, &DMagicianProtocol::decodeProgressBar);

    m_decodeFuncMap.insert(10, &DMagicianProtocol::decodePose);
    m_decodeFuncMap.insert(11, &DMagicianProtocol::decodeResetPose);
    m_decodeFuncMap.insert(13, &DMagicianProtocol::decodePoseL);
    m_decodeFuncMap.insert(15, &DMagicianProtocol::decodeCheckPoseLimit);

    m_decodeFuncMap.insert(20, &DMagicianProtocol::decodeAlarmState);

    m_decodeFuncMap.insert(30, &DMagicianProtocol::decodeHOMEParams);
    m_decodeFuncMap.insert(31, &DMagicianProtocol::decodeHOMECmd);
    m_decodeFuncMap.insert(32, &DMagicianProtocol::decodeAutoLeveling);
    m_decodeFuncMap.insert(33, &DMagicianProtocol::decodeGoHomeStatus);

    m_decodeFuncMap.insert(40, &DMagicianProtocol::decodeHHTTrigMode);
    m_decodeFuncMap.insert(41, &DMagicianProtocol::decodeHHTTrigOutputEnabled);
    m_decodeFuncMap.insert(42, &DMagicianProtocol::decodeHHTTrigOutput);

    m_decodeFuncMap.insert(50, &DMagicianProtocol::decodeArmOrientation);
    m_decodeFuncMap.insert(51, &DMagicianProtocol::decodeMotorMode);

    m_decodeFuncMap.insert(60, &DMagicianProtocol::decodeEndEffectorParams);
    m_decodeFuncMap.insert(61, &DMagicianProtocol::decodeEndEffectorLaser);
    m_decodeFuncMap.insert(62, &DMagicianProtocol::decodeEndEffectorSuctionCup);
    m_decodeFuncMap.insert(63, &DMagicianProtocol::decodeEndEffectorGripper);
    m_decodeFuncMap.insert(64, &DMagicianProtocol::decodeEndEffectorType);

    m_decodeFuncMap.insert(65, &DMagicianProtocol::decodeServoAngle);
    m_decodeFuncMap.insert(66, &DMagicianProtocol::decodeArmSpeedRatio);
    m_decodeFuncMap.insert(67, &DMagicianProtocol::decodeLSpeedRatio);
    m_decodeFuncMap.insert(68, &DMagicianProtocol::decodeConfigAndStatus);
    m_decodeFuncMap.insert(69, &DMagicianProtocol::decodeRTStatus);

    m_decodeFuncMap.insert(70, &DMagicianProtocol::decodeJOGJointParams);
    m_decodeFuncMap.insert(71, &DMagicianProtocol::decodeJOGCoordinateParams);
    m_decodeFuncMap.insert(72, &DMagicianProtocol::decodeJOGCommonParams);
    m_decodeFuncMap.insert(73, &DMagicianProtocol::decodeJOGCmd);
    m_decodeFuncMap.insert(74, &DMagicianProtocol::decodeJOGLParams);
    m_decodeFuncMap.insert(75, &DMagicianProtocol::decodeJOGCmdTime);

    m_decodeFuncMap.insert(80, &DMagicianProtocol::decodePTPJointParams);
    m_decodeFuncMap.insert(81, &DMagicianProtocol::decodePTPCoordinateParams);
    m_decodeFuncMap.insert(82, &DMagicianProtocol::decodePTPJumpParams);
    m_decodeFuncMap.insert(83, &DMagicianProtocol::decodePTPCommonParams);
    m_decodeFuncMap.insert(84, &DMagicianProtocol::decodePTPCmd);
    m_decodeFuncMap.insert(85, &DMagicianProtocol::decodePTPLParams);
    m_decodeFuncMap.insert(86, &DMagicianProtocol::decodePTPWithLCmd);
    m_decodeFuncMap.insert(87, &DMagicianProtocol::decodePTPJump2Params);
    m_decodeFuncMap.insert(88, &DMagicianProtocol::decodePTPPOCmd);
    m_decodeFuncMap.insert(89, &DMagicianProtocol::decodePTPPOWithLCmd);

    m_decodeFuncMap.insert(90, &DMagicianProtocol::decodeCPParams);
    m_decodeFuncMap.insert(91, &DMagicianProtocol::decodeCPCmd);
    m_decodeFuncMap.insert(92, &DMagicianProtocol::decodeCPLECmd);
    m_decodeFuncMap.insert(93, &DMagicianProtocol::decodeCPRHeadHoldEnable);
    m_decodeFuncMap.insert(94, &DMagicianProtocol::decodeCPCommonParams);
    m_decodeFuncMap.insert(95, &DMagicianProtocol::decodeCP2Cmd);

    m_decodeFuncMap.insert(100, &DMagicianProtocol::decodeARCParams);
    m_decodeFuncMap.insert(101, &DMagicianProtocol::decodeARCCmd);
    m_decodeFuncMap.insert(102, &DMagicianProtocol::decodeCircleCmd);
    m_decodeFuncMap.insert(103, &DMagicianProtocol::decodeARCCommonParams);

    m_decodeFuncMap.insert(110, &DMagicianProtocol::decodeWAITCmd);

    m_decodeFuncMap.insert(120, &DMagicianProtocol::decodeTRIGCmd);

    m_decodeFuncMap.insert(130, &DMagicianProtocol::decodeIOMultiplexing);
    m_decodeFuncMap.insert(131, &DMagicianProtocol::decodeIODO);
    m_decodeFuncMap.insert(132, &DMagicianProtocol::decodeIOPWM);
    m_decodeFuncMap.insert(133, &DMagicianProtocol::decodeIODI);
    m_decodeFuncMap.insert(134, &DMagicianProtocol::decodeIOADC);
    m_decodeFuncMap.insert(135, &DMagicianProtocol::decodeEMotor);
    m_decodeFuncMap.insert(136, &DMagicianProtocol::decodeEMotorS);
    m_decodeFuncMap.insert(137, &DMagicianProtocol::decodeColorSensor);
    m_decodeFuncMap.insert(138, &DMagicianProtocol::decodeInfraredSensor);
    m_decodeFuncMap.insert(139, &DMagicianProtocol::decodeIICInfo);

    m_decodeFuncMap.insert(140, &DMagicianProtocol::decodeAngleSensorStaticError);
    m_decodeFuncMap.insert(141, &DMagicianProtocol::decodeAngleSensorCoef);
    m_decodeFuncMap.insert(142, &DMagicianProtocol::decodeBaseDecoderStaticError);
    m_decodeFuncMap.insert(143, &DMagicianProtocol::decodeLRHandCalibrateValue);

    m_decodeFuncMap.insert(150, &DMagicianProtocol::decodeWIFIConfigMode);
    m_decodeFuncMap.insert(151, &DMagicianProtocol::decodeWIFISSID);
    m_decodeFuncMap.insert(152, &DMagicianProtocol::decodeWIFIPassword);
    m_decodeFuncMap.insert(153, &DMagicianProtocol::decodeWIFIIPAddress);
    m_decodeFuncMap.insert(154, &DMagicianProtocol::decodeWIFINetmask);
    m_decodeFuncMap.insert(155, &DMagicianProtocol::decodeWIFIGateway);
    m_decodeFuncMap.insert(156, &DMagicianProtocol::decodeWIFIDNS);
    m_decodeFuncMap.insert(157, &DMagicianProtocol::decodeWIFIConnectStatus);

    m_decodeFuncMap.insert(170, &DMagicianProtocol::decodeLostStep);
    m_decodeFuncMap.insert(171, &DMagicianProtocol::decodeLostStepCmd);
    m_decodeFuncMap.insert(172, &DMagicianProtocol::decodeCollisionCheck);

    m_decodeFuncMap.insert(192, &DMagicianProtocol::decodeDeviceStatus);
    m_decodeFuncMap.insert(193, &DMagicianProtocol::decodeDeviceRunAPP);
    m_decodeFuncMap.insert(194, &DMagicianProtocol::decodeUpgradeStatus);
    m_decodeFuncMap.insert(195, &DMagicianProtocol::decodeRunUpgrade);
    m_decodeFuncMap.insert(196, &DMagicianProtocol::decodeStartTransmit);
    m_decodeFuncMap.insert(197, &DMagicianProtocol::decodeUpgradeVeify);
    m_decodeFuncMap.insert(198, &DMagicianProtocol::decodeDeviceRestart);

    m_decodeFuncMap.insert(200, &DMagicianProtocol::decodeColorObj);

    m_decodeFuncMap.insert(210, &DMagicianProtocol::decodeSeeedDistanceSensor);
    m_decodeFuncMap.insert(211, &DMagicianProtocol::decodeSeeedTempSensor);
    m_decodeFuncMap.insert(212, &DMagicianProtocol::decodeSeeedLightSensor);
    m_decodeFuncMap.insert(213, &DMagicianProtocol::decodeSeeedColorSensor);
    m_decodeFuncMap.insert(214, &DMagicianProtocol::decodeSeeedRGBLed);
    m_decodeFuncMap.insert(215, &DMagicianProtocol::decodeMCUTemperature);
    m_decodeFuncMap.insert(216, &DMagicianProtocol::decodeDobotSensor);
    m_decodeFuncMap.insert(217, &DMagicianProtocol::decodeBoxFunc);

    m_decodeFuncMap.insert(240, &DMagicianProtocol::decodeQueuedCmdStart);
    m_decodeFuncMap.insert(241, &DMagicianProtocol::decodeQueuedCmdStop);
    m_decodeFuncMap.insert(242, &DMagicianProtocol::decodeQueuedCmdForceStop);
    m_decodeFuncMap.insert(243, &DMagicianProtocol::decodeQueuedCmdStartDownload);
    m_decodeFuncMap.insert(244, &DMagicianProtocol::decodeQueuedCmdStopDownload);
    m_decodeFuncMap.insert(245, &DMagicianProtocol::decodeQueuedCmdClear);
    m_decodeFuncMap.insert(246, &DMagicianProtocol::decodeQueuedCmdCurrentIndex);
    m_decodeFuncMap.insert(247, &DMagicianProtocol::decodeQueuedCmdLeftSpace);
    m_decodeFuncMap.insert(248, &DMagicianProtocol::decodeQueuedCmdMotionFinish);
}

void DMagicianProtocol::_decodeDobotSensorFunctionInit()
{
    m_decodeDobotSensorFuncMap.insert(1, &DMagicianProtocol::decodeSensorRGBLEDInit);
    m_decodeDobotSensorFuncMap.insert(2, &DMagicianProtocol::decodeSensorSetRGBLEDVlaue);
    m_decodeDobotSensorFuncMap.insert(3, &DMagicianProtocol::decodeSensorRGBLEDTurnOFFAll);
    m_decodeDobotSensorFuncMap.insert(4, &DMagicianProtocol::decodeSensorSetRGBLEDState);
    m_decodeDobotSensorFuncMap.insert(10, &DMagicianProtocol::decodeSensorOledInit);
    m_decodeDobotSensorFuncMap.insert(11, &DMagicianProtocol::decodeSensorOledClear);
    m_decodeDobotSensorFuncMap.insert(12, &DMagicianProtocol::decodeSensorOledDisplay);
    m_decodeDobotSensorFuncMap.insert(20, &DMagicianProtocol::decodeSensorKnobInit);
    m_decodeDobotSensorFuncMap.insert(21, &DMagicianProtocol::decodeSensorGetKnob);
    m_decodeDobotSensorFuncMap.insert(22, &DMagicianProtocol::decodeSensorVoiceInit);
    m_decodeDobotSensorFuncMap.insert(23, &DMagicianProtocol::decodeSensorGetVoice);
    m_decodeDobotSensorFuncMap.insert(24, &DMagicianProtocol::decodeSensorLightInit);
    m_decodeDobotSensorFuncMap.insert(25, &DMagicianProtocol::decodeSensorGetLight);
    m_decodeDobotSensorFuncMap.insert(26, &DMagicianProtocol::decodeSensorPEInit);
    m_decodeDobotSensorFuncMap.insert(27, &DMagicianProtocol::decodeSensorGetPEState);
    m_decodeDobotSensorFuncMap.insert(28, &DMagicianProtocol::decodeSensorUltrasonicInit);
    m_decodeDobotSensorFuncMap.insert(29, &DMagicianProtocol::decodeSensorUltrasonicGet);
    m_decodeDobotSensorFuncMap.insert(30, &DMagicianProtocol::decodeSensorSHT31Init);
    m_decodeDobotSensorFuncMap.insert(31, &DMagicianProtocol::decodeSensorGetSHT31);
    m_decodeDobotSensorFuncMap.insert(32, &DMagicianProtocol::decodeSensorColorInit);
    m_decodeDobotSensorFuncMap.insert(33, &DMagicianProtocol::decodeSensorGetColor);
    m_decodeDobotSensorFuncMap.insert(34, &DMagicianProtocol::decodeSensorGetColorRes);
    m_decodeDobotSensorFuncMap.insert(40, &DMagicianProtocol::decodeSensorXBeeInit);
    m_decodeDobotSensorFuncMap.insert(41, &DMagicianProtocol::decodeSensorXBeeSend);
    m_decodeDobotSensorFuncMap.insert(42, &DMagicianProtocol::decodeSensorXBeeReceive);
    m_decodeDobotSensorFuncMap.insert(43, &DMagicianProtocol::decodeSensorXBeeClear);
    m_decodeDobotSensorFuncMap.insert(50, &DMagicianProtocol::decodeSensorSYNInit);
    m_decodeDobotSensorFuncMap.insert(51, &DMagicianProtocol::decodeSensorSetSYN);
    m_decodeDobotSensorFuncMap.insert(52, &DMagicianProtocol::decodeSensorSetSYNMusic);
    m_decodeDobotSensorFuncMap.insert(53, &DMagicianProtocol::decodeSensorSendSYN);
    m_decodeDobotSensorFuncMap.insert(54, &DMagicianProtocol::decodeSensorSetSYNCmd);

    /* Dobot Ble id:216 subid:60~68 */
    m_decodeDobotSensorFuncMap.insert(60, &DMagicianProtocol::decodeBleInf);
    m_decodeDobotSensorFuncMap.insert(61, &DMagicianProtocol::decodeBleConnect);
    m_decodeDobotSensorFuncMap.insert(63, &DMagicianProtocol::decodeBleReadOneData);
    m_decodeDobotSensorFuncMap.insert(64, &DMagicianProtocol::decodeBleWriteOneData);
    m_decodeDobotSensorFuncMap.insert(65, &DMagicianProtocol::decodeSetBleMesh);
    m_decodeDobotSensorFuncMap.insert(66, &DMagicianProtocol::decodeBleReadMeshData);
    m_decodeDobotSensorFuncMap.insert(67, &DMagicianProtocol::decodeBleWriteMeshData);
    m_decodeDobotSensorFuncMap.insert(68, &DMagicianProtocol::decodeBleClearMeshData);

    /* Dobot hand id:216 subid:70~72 */
    m_decodeDobotSensorFuncMap.insert(70, &DMagicianProtocol::decodeSetSensorHandModel);
    m_decodeDobotSensorFuncMap.insert(71, &DMagicianProtocol::decodeGetSensorHandModel);
    m_decodeDobotSensorFuncMap.insert(72, &DMagicianProtocol::decodeIsSensorHandModel);

    /* Dobot hand id:216 subid:73~75 */
    m_decodeDobotSensorFuncMap.insert(73, &DMagicianProtocol::decodeIsJoystickButton);
    m_decodeDobotSensorFuncMap.insert(74, &DMagicianProtocol::decodeGetJoystickPos);
    m_decodeDobotSensorFuncMap.insert(75, &DMagicianProtocol::decodeGetJoystickButton);

    /* Dobot hand id:216 subid:76 */
    m_decodeDobotSensorFuncMap.insert(76, &DMagicianProtocol::decodeIsPirDetected);
    /* Dobot hand id:216 subid:77 */
    m_decodeDobotSensorFuncMap.insert(77, &DMagicianProtocol::decodeGetButtonStatus);
    /* Dobot hand id:216 subid:78 */
    m_decodeDobotSensorFuncMap.insert(78, &DMagicianProtocol::decodeGetRedButtonStatus);
    /* Dobot hand id:216 subid:79 */
    m_decodeDobotSensorFuncMap.insert(79, &DMagicianProtocol::decodeGetBlueButtonStatus);

    //216-90~95
    m_decodeDobotSensorFuncMap.insert(90, &DMagicianProtocol::decodeBleSetMaster);
    m_decodeDobotSensorFuncMap.insert(91, &DMagicianProtocol::decodeBleSetSlave);
    m_decodeDobotSensorFuncMap.insert(92, &DMagicianProtocol::decodeBleRecvData);
    m_decodeDobotSensorFuncMap.insert(93, &DMagicianProtocol::decodeBleSendData);
    m_decodeDobotSensorFuncMap.insert(94, &DMagicianProtocol::decodeBleClearCacheData);
    m_decodeDobotSensorFuncMap.insert(95, &DMagicianProtocol::decodeBleGetMacAddress);
}

void DMagicianProtocol::_decodeBoxFuncInit() {
    m_decodeBoxFuncMap.insert(10, &DMagicianProtocol::decodeSetStopPointServer);
    m_decodeBoxFuncMap.insert(11, &DMagicianProtocol::decodeSetStopPointParam);
    m_decodeBoxFuncMap.insert(12, &DMagicianProtocol::decodeGetStopPointState);
    m_decodeBoxFuncMap.insert(20, &DMagicianProtocol::decodeGetImgToArmXY);
}

bool DMagicianProtocol::checkIntValue(const QJsonObject &obj, const QString &value)
{
    if (!obj.contains(value)) {
        qDebug().noquote() << QString("'%1':missing, use (0)").arg(value);
        return false;
    } else if (!obj.value(value).isDouble()) {
        qWarning().noquote() << QString("'%1':type error").arg(value);
        return false;
    }
    return true;
}

bool DMagicianProtocol::checkFloatValue(const QJsonObject &obj, const QString &value)
{
    if (!obj.contains(value)) {
        qDebug().noquote() << QString("'%1':missing, use (0.0)").arg(value);
        return false;
    } else if (!obj.value(value).isDouble()) {
        qWarning().noquote() << QString("'%1':type error").arg(value);
        return false;
    }
    return true;
}

bool DMagicianProtocol::checkBoolValue(const QJsonObject &obj, const QString &value)
{
    if (!obj.contains(value)) {
        qDebug().noquote() << QString("'%1':missing, use (false)").arg(value);
        return false;
    } else if (!obj.value(value).isBool()) {
        qWarning().noquote() << QString("'%1':type error").arg(value);
        return false;
    }
    return true;
}

bool DMagicianProtocol::checkStringValue(const QJsonObject &obj, const QString &value)
{
    if (!obj.contains(value)) {
        qDebug().noquote() << QString("'%1':missing, use ('')").arg(value);
        return false;
    } else if (!obj.value(value).isString()) {
        qWarning().noquote() << QString("'%1':type error").arg(value);
        return false;
    }
    return true;
}

bool DMagicianProtocol::checkArrayValue(const QJsonObject &obj, const QString &value)
{
    if (!obj.contains(value)) {
        qDebug().noquote() << QString("'%1':missing, use ([])").arg(value);
        return false;
    } else if (!obj.value(value).isArray()) {
        qWarning().noquote() << QString("'%1':type error").arg(value);
        return false;
    }
    return true;
}

QJsonObject DMagicianProtocol::addQueueIndexInfo(QJsonObject &resObj, QByteArray &params)
{
    if (params.size() == sizeof (quint64)) {
        quint64 queuedCmdIndex = 0;
        memcpy(&queuedCmdIndex, params, sizeof (quint64));

        QJsonObject paramsObj;
        paramsObj.insert("queuedCmdIndex", static_cast<double>(queuedCmdIndex));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

const QStringList DMagicianProtocol::getCommandList()
{
    return m_encodeFuncMap.keys();
}

QJsonObject DMagicianProtocol::parseParams(PacketPayload &payload)
{
    QJsonObject resObj;

    DecodeFunction function = m_decodeFuncMap.value(payload.p_cmdID);

    if (function) {
        resObj = (this->*function)(payload.p_ctrl.c_rw, payload.p_params);
    } else {
        qDebug() << "No matching parseParams function.";
    }
    return resObj;
}

bool DMagicianProtocol::setPayload(QString cmd, PacketPayload &payload, QJsonObject params)
{
    QByteArray res;

    EncodeFunction function = m_encodeFuncMap.value(cmd);
    if (function) {
        (this->*function)(payload, params);
        return true;
    } else {
        qDebug() << "No matching encode function with: " << cmd << " params:" << params;
    }
    return false;
}


/**********************************************************************************
 * *** DECODE *** DECODE *** DECODE *** DECODE *** DECODE *** DECODE *** DECODE ***
 *********************************************************************************/

//![0]
QJsonObject DMagicianProtocol::decodeDeviceSN(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetDeviceSN");
    } else {
        resObj.insert("cmd", "SetDeviceSN");
    }

    if (params.size() > 0) {
        char *sn = params.data();

        QJsonObject paramsObj;
        paramsObj.insert("deviceSN", sn);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![1]
QJsonObject DMagicianProtocol::decodeDeviceName(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetDeviceName");
    } else {
        resObj.insert("cmd", "SetDeviceName");
    }

    if (params.size() > 0) {
        char *devName = params.data();

        QJsonObject paramsObj;
        paramsObj.insert("deviceName", devName);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![2]
QJsonObject DMagicianProtocol::decodeDeviceVersion(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetDeviceVersion");
    }
    QJsonObject paramsObj;
    paramsObj.insert("majorVersion", params.at(0));
    paramsObj.insert("minorVersion", params.at(1));
    paramsObj.insert("revision", params.at(2));

    if (m_Device == "Magician") {
        paramsObj.insert("hwVersion", params.at(13));
        qDebug() << "this is magician ";
    } else {
        paramsObj.insert("hwVersion", params.at(3));
        qDebug() << "this is box/lite ";
    }
    resObj.insert("params", paramsObj);
    return resObj;
}

//![3]
QJsonObject DMagicianProtocol::decodeDeviceWithL(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetDeviceWithL");
    } else {
        resObj.insert("cmd", "SetDeviceWithL");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 isEnabled = 0;
        memcpy(&isEnabled, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("isEnabled", isEnabled > 0 ? true : false);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![4]
QJsonObject DMagicianProtocol::decodeDeviceTime(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetDeviceTime");
    }

    if (params.size() == sizeof (quint32)) {
        quint32 gSystick = 0;
        memcpy(&gSystick, params, sizeof (quint32));

        QJsonObject paramsObj;
        paramsObj.insert("gSystick", static_cast<double>(gSystick));

        QTime t = QTime::fromMSecsSinceStartOfDay(static_cast<int>(gSystick));
        paramsObj.insert("passtime", t.toString("hh:mm:ss.z"));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![5]
QJsonObject DMagicianProtocol::decodeDeviceID(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetDeviceID");
    }

    if (params.size() == 3 * sizeof (quint32)) {
        QDataStream out(&params, QIODevice::ReadOnly);
        QJsonArray idArrary;
        for (int i = 0; i < 3; ++i) {
            quint8 i_id = 0;
            out >> i_id;
            idArrary.append(i_id);
        }

        QJsonObject paramsObj;
        paramsObj.insert("deviceID", idArrary);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![6]
QJsonObject DMagicianProtocol::decodeDeviceInfo(quint8 rw, QByteArray params)
{
#pragma pack(push, 1)
    struct DeviceInfoStruct {
        quint64 runTime;
        quint32 powerOn;
        quint32 powerOff;
    };
#pragma pack(pop)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetDeviceInfo");
    }

    if (params.size() == sizeof (DeviceInfoStruct)) {
        DeviceInfoStruct devInfo;
        memcpy(&devInfo, params, sizeof (DeviceInfoStruct));

        QJsonObject paramsObj;
        paramsObj.insert("deviceRunTime", static_cast<double>(devInfo.runTime));
        paramsObj.insert("devicePowerOn", static_cast<double>(devInfo.powerOn));
        paramsObj.insert("devicePowerOff", static_cast<double>(devInfo.powerOff));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![7]
QJsonObject DMagicianProtocol::decodeProductName(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetProductName");
    }

    if (params.size() > 0) {
        char *productName = params.data();

        QJsonObject paramsObj;
        paramsObj.insert("productName", productName);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![8]
QJsonObject DMagicianProtocol::decodeOLEDDisplay(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetOLEDDisplay");
    }

    return resObj;
}

//![9]
QJsonObject DMagicianProtocol::decodeProgressBar(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetProgressBar");
    }

    return resObj;
}

//![10]
QJsonObject DMagicianProtocol::decodePose(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetPose");
    }

    if (params.size() == sizeof (PoseStruct)) {
        PoseStruct pose;
        memcpy(&pose, params, sizeof (PoseStruct));

        QJsonObject paramsObj;
        paramsObj.insert("x", static_cast<double>(pose.x));
        paramsObj.insert("y", static_cast<double>(pose.y));
        paramsObj.insert("z", static_cast<double>(pose.z));
        paramsObj.insert("r", static_cast<double>(pose.r));

        QJsonArray jointAngleArray;
        for (int i = 0; i < 4; ++i) {
            jointAngleArray.append(static_cast<double>(pose.jointAngle[i]));
        }
        paramsObj.insert("jointAngle", jointAngleArray);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![11]
QJsonObject DMagicianProtocol::decodeResetPose(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "ResetPose");
    }
    return resObj;
}

//![13]
QJsonObject DMagicianProtocol::decodePoseL(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetPoseL");
    }

    if (params.size() == sizeof (float)) {
        float positionL;
        memcpy(&positionL, params, sizeof (float));

        QJsonObject paramsObj;
        paramsObj.insert("positionL", static_cast<double>(positionL));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![15]
QJsonObject DMagicianProtocol::decodeCheckPoseLimit(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "CheckPoseLimit");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 isLimited;
        memcpy(&isLimited, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("isLimited", isLimited > 0 ? true : false);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![20]
QJsonObject DMagicianProtocol::decodeAlarmState(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetAlarmsState");
    } else {
        resObj.insert("cmd", "ClearAllAlarmsState");
    }

    if (params.size() > 0) {
        QDataStream out(&params, QIODevice::ReadOnly);

        QJsonArray stateArr;
        for (int i = 0; i < params.size(); ++i) {
            quint8 state = 0;
            out >> state;
            stateArr.append(state);
        }

        QJsonObject paramsObj;
        paramsObj.insert("state", stateArr);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![30]
QJsonObject DMagicianProtocol::decodeHOMEParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetHOMEParams");
    } else {
        resObj.insert("cmd", "SetHOMEParams");
    }

    if (params.size() == sizeof (PositionStruct)) {
        PositionStruct homePose;
        memcpy(&homePose, params, sizeof (PositionStruct));

        QJsonObject paramsObj;
        paramsObj.insert("x", static_cast<double>(homePose.x));
        paramsObj.insert("y", static_cast<double>(homePose.y));
        paramsObj.insert("z", static_cast<double>(homePose.z));
        paramsObj.insert("r", static_cast<double>(homePose.r));
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![31]
QJsonObject DMagicianProtocol::decodeHOMECmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetHOMECmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![32]
QJsonObject DMagicianProtocol::decodeAutoLeveling(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetAutoLeveling");
        if (params.size() == sizeof (float)) {
            float precision;
            memcpy(&precision, params, sizeof (float));

            QJsonObject paramsObj;
            paramsObj.insert("precision", static_cast<double>(precision));
            resObj.insert("params", paramsObj);
        }
    } else {
        resObj.insert("cmd", "SetAutoLeveling");
        if (params.size() == sizeof (uint64_t)) {
            uint64_t queuedCmdIndex;
            memcpy(&queuedCmdIndex, params, sizeof (uint64_t));

            QJsonObject paramsObj;
            paramsObj.insert("queuedCmdIndex", (qint64)queuedCmdIndex);
            resObj.insert("params", paramsObj);
        }
    }


    return resObj;
}

//![33]
QJsonObject DMagicianProtocol::decodeGoHomeStatus(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetGoHomeStatus");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 isFinished = 0;
        memcpy(&isFinished, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("isFinished", isFinished > 0 ? true : false);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![40]
QJsonObject DMagicianProtocol::decodeHHTTrigMode(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetHHTTrigMode");
    } else {
        resObj.insert("cmd", "SetHHTTrigMode");
    }

    if (params.size() > 0){
        quint8 mode = 0;
        memcpy(&mode, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("mode", mode);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![41]
QJsonObject DMagicianProtocol::decodeHHTTrigOutputEnabled(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetHHTTrigOutputEnabled");
    } else {
        resObj.insert("cmd", "SetHHTTrigOutputEnabled");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 isEnabled = 0;
        memcpy(&isEnabled, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("isEnabled", isEnabled > 0 ? true : false);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![42]
QJsonObject DMagicianProtocol::decodeHHTTrigOutput(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetHHTTrigOutput");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 isTriggered;
        memcpy(&isTriggered, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("isTriggered", isTriggered > 0 ? true : false);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![50]
QJsonObject DMagicianProtocol::decodeArmOrientation(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetArmOrientation");
    } else {
        resObj.insert("cmd", "SetArmOrientation");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 armOrientation = 0;
        memcpy(&armOrientation, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("armOrientation", armOrientation);
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![51]
QJsonObject DMagicianProtocol::decodeMotorMode(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetMotorMode");
    } else {
        resObj.insert("cmd", "SetMotorMode");
    }
    return resObj;
}

//![60]
QJsonObject DMagicianProtocol::decodeEndEffectorParams(quint8 rw, QByteArray params)
{
    struct CoordinateOffsetStruct {
        float xOffset;
        float yOffset;
        float zOffset;
    };

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetEndEffectorParams");
    } else {
        resObj.insert("cmd", "SetEndEffectorParams");
    }

    if (params.size() == sizeof (CoordinateOffsetStruct)) {
        CoordinateOffsetStruct coordinateOffset;
        memcpy(&coordinateOffset, params, sizeof (CoordinateOffsetStruct));

        QJsonObject paramsObj;
        paramsObj.insert("xOffset", static_cast<double>(coordinateOffset.xOffset));
        paramsObj.insert("yOffset", static_cast<double>(coordinateOffset.yOffset));
        paramsObj.insert("zOffset", static_cast<double>(coordinateOffset.zOffset));
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![61]
QJsonObject DMagicianProtocol::decodeEndEffectorLaser(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetEndEffectorLaser");
    } else {
        resObj.insert("cmd", "SetEndEffectorLaser");
    }

    if (params.size() == 2 * sizeof (quint8)) {
        quint8 isEnabled = 0, isOn = 0;
        QDataStream out(&params, QIODevice::ReadOnly);
        out >> isEnabled >> isOn;

        QJsonObject paramsObj;
        paramsObj.insert("isEnabled", isEnabled > 0 ? true : false);
        paramsObj.insert("isOn", isOn > 0 ? true : false);
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![62]
QJsonObject DMagicianProtocol::decodeEndEffectorSuctionCup(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetEndEffectorSuctionCup");
    } else {
        resObj.insert("cmd", "SetEndEffectorSuctionCup");
    }

    if (params.size() == 2 * sizeof (quint8)) {
        quint8 isEnabled = 0, isSuck = 0;
        QDataStream out(&params, QIODevice::ReadOnly);
        out >> isEnabled >> isSuck;

        QJsonObject paramsObj;
        paramsObj.insert("isEnabled", isEnabled > 0 ? true : false);
        paramsObj.insert("isOn", isSuck > 0 ? true : false);
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![63]
QJsonObject DMagicianProtocol::decodeEndEffectorGripper(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetEndEffectorGripper");
    } else {
        resObj.insert("cmd", "SetEndEffectorGripper");
    }

    if (params.size() == 2 * sizeof (quint8)) {
        quint8 isEnabled = 0, isGriped = 0;
        QDataStream out(&params, QIODevice::ReadOnly);
        out >> isEnabled >> isGriped;

        QJsonObject paramsObj;
        paramsObj.insert("isEnabled", isEnabled > 0 ? true : false);
        paramsObj.insert("isOn", isGriped > 0 ? true : false);
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![64]
QJsonObject DMagicianProtocol::decodeEndEffectorType(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetEndEffectorType");
    } else {
        resObj.insert("cmd", "SetEndEffectorType");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 type = 0;
        QDataStream out(&params, QIODevice::ReadOnly);
        out >> type;

        QJsonObject paramsObj;
        paramsObj.insert("type", type);
        resObj.insert("params", paramsObj);
    }

    return resObj;
}

//![65]
QJsonObject DMagicianProtocol::decodeServoAngle(quint8 rw, QByteArray params)
{
#pragma pack(push, 1)
    struct ServoAngleStruct {
        quint8 index;
        float angle;
    };
#pragma pack(pop)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetServoAngle");
    } else {
        resObj.insert("cmd", "SetServoAngle");
    }

    if (params.size() == sizeof (ServoAngleStruct)) {
        ServoAngleStruct servoAngle;
        memcpy(&servoAngle, params, sizeof (ServoAngleStruct));

        QJsonObject paramsObj;
        paramsObj.insert("index", servoAngle.index);
        paramsObj.insert("angle", static_cast<double>(servoAngle.angle));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![66]
QJsonObject DMagicianProtocol::decodeArmSpeedRatio(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetArmSpeedRatio");
    } else {
        resObj.insert("cmd", "SetArmSpeedRatio");
    }

    if (params.size() == 2 * sizeof (quint8)) {
        quint8 type = 0, value = 0;
        QDataStream out(&params, QIODevice::ReadOnly);
        out >> type >> value;

        QJsonObject paramsObj;
        paramsObj.insert("type", type);
        paramsObj.insert("value", value);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![67]
QJsonObject DMagicianProtocol::decodeLSpeedRatio(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetLSpeedRatio");
    } else {
        resObj.insert("cmd", "SetLSpeedRatio");
    }

    if (params.size() == 2 * sizeof (quint8)) {
        quint8 type = 0;
        quint8 value = 0;
        QDataStream out(&params, QIODevice::ReadOnly);
        out >> type >> value;

        QJsonObject paramsObj;
        paramsObj.insert("type", type);
        paramsObj.insert("value", value);
        resObj.insert("params", paramsObj);
    }

    return resObj;
}

//![68]
QJsonObject DMagicianProtocol::decodeConfigAndStatus(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetConfigAndStatus");
    }

    return resObj;
}

//![69]
QJsonObject DMagicianProtocol::decodeRTStatus(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetRTStatus");
    }

    return resObj;
}

//![70]
QJsonObject DMagicianProtocol::decodeJOGJointParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetJOGJointParams");
    } else {
        resObj.insert("cmd", "SetJOGJointParams");
    }

    if (params.size() == sizeof (VelocityArrayStruct)) {
        VelocityArrayStruct JOGLParams;
        memcpy(&JOGLParams, params, sizeof (VelocityArrayStruct));

        QJsonArray velocityArray, accelerationArray;
        for (int i = 0; i < 4; ++i) {
            velocityArray.append(static_cast<double>(JOGLParams.velocity[i]));
            accelerationArray.append(static_cast<double>(JOGLParams.acceleration[i]));
        }

        QJsonObject paramsObj;
        paramsObj.insert("velocity", velocityArray);
        paramsObj.insert("acceleration", accelerationArray);
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![71]
QJsonObject DMagicianProtocol::decodeJOGCoordinateParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetJOGCoordinateParams");
    } else {
        resObj.insert("cmd", "SetJOGCoordinateParams");
    }

    if (params.size() == sizeof (VelocityArrayStruct)) {
        VelocityArrayStruct jogCoordinateParams;
        memcpy(&jogCoordinateParams, params, sizeof (VelocityArrayStruct));

        QJsonArray velocityArray, accelerationArray;
        for (int i = 0; i < 4; ++i) {
            velocityArray.append(static_cast<double>(jogCoordinateParams.velocity[i]));
            accelerationArray.append(static_cast<double>(jogCoordinateParams.acceleration[i]));
        }

        QJsonObject paramsObj;
        paramsObj.insert("velocity", velocityArray);
        paramsObj.insert("acceleration", accelerationArray);
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![72]
QJsonObject DMagicianProtocol::decodeJOGCommonParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetJOGCommonParams");
    } else {
        resObj.insert("cmd", "SetJOGCommonParams");
    }

    if (params.size() == sizeof (VelocityRatioStruct)) {
        VelocityRatioStruct jogParams;
        memcpy(&jogParams, params, sizeof (VelocityRatioStruct));

        QJsonObject paramsObj;
        paramsObj.insert("velocityRatio", static_cast<double>(jogParams.velocityRatio));
        paramsObj.insert("accelerationRatio", static_cast<double>(jogParams.accelerationRatio));
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![73]
QJsonObject DMagicianProtocol::decodeJOGCmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetJOGCmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![74]
QJsonObject DMagicianProtocol::decodeJOGLParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetJOGLParams");
    } else {
        resObj.insert("cmd", "SetJOGLParams");
    }

    if (params.size() == sizeof (VelocityStruct)) {
        VelocityStruct jogParams;
        memcpy(&jogParams, params, sizeof (VelocityStruct));

        QJsonObject paramsObj;
        paramsObj.insert("velocity", static_cast<double>(jogParams.velocity));
        paramsObj.insert("acceleration", static_cast<double>(jogParams.acceleration));
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![75]
QJsonObject DMagicianProtocol::decodeJOGCmdTime(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetJOGCmdTime");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![80]
QJsonObject DMagicianProtocol::decodePTPJointParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetPTPJointParams");
    } else {
        resObj.insert("cmd", "SetPTPJointParams");
    }

    if (params.size() == sizeof (VelocityArrayStruct)) {
        VelocityArrayStruct ptpJointParams;
        memcpy(&ptpJointParams, params, sizeof (VelocityArrayStruct));

        QJsonArray velocityArray, accelerationArray;
        for (int i = 0; i < 4; i++) {
            velocityArray.append(static_cast<double>(ptpJointParams.velocity[i]));
            accelerationArray.append(static_cast<double>(ptpJointParams.acceleration[i]));
        }

        QJsonObject paramsObj;
        paramsObj.insert("velocity", velocityArray);
        paramsObj.insert("acceleration", accelerationArray);
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![81]
QJsonObject DMagicianProtocol::decodePTPCoordinateParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetPTPCoordinateParams");
    } else {
        resObj.insert("cmd", "SetPTPCoordinateParams");
    }

    if (params.size() == sizeof (PTPCoordinateParamsStruct)) {
        PTPCoordinateParamsStruct ptpCoordinateParams;
        memcpy(&ptpCoordinateParams, params, sizeof (PTPCoordinateParamsStruct));

        QJsonObject paramsObj;
        paramsObj.insert("xyzVelocity", static_cast<double>(ptpCoordinateParams.xyzVelocity));
        paramsObj.insert("rVelocity", static_cast<double>(ptpCoordinateParams.rVelocity));
        paramsObj.insert("xyzAcceleration", static_cast<double>(ptpCoordinateParams.xyzAcceleration));
        paramsObj.insert("rAcceleration", static_cast<double>(ptpCoordinateParams.rAcceleration));
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![82]
QJsonObject DMagicianProtocol::decodePTPJumpParams(quint8 rw, QByteArray params)
{
    struct PTPJumpParamsStruct {
        float jumpHeight;
        float zLimit;
    };

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetPTPJumpParams");
    } else {
        resObj.insert("cmd", "SetPTPJumpParams");
    }

    if (params.size() == sizeof (PTPJumpParamsStruct)) {
        PTPJumpParamsStruct ptpParams;
        memcpy(&ptpParams, params, sizeof (PTPJumpParamsStruct));

        QJsonObject paramsObj;
        paramsObj.insert("jumpHeight", static_cast<double>(ptpParams.jumpHeight));
        paramsObj.insert("zLimit", static_cast<double>(ptpParams.zLimit));
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![83]
QJsonObject DMagicianProtocol::decodePTPCommonParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetPTPCommonParams");
    } else {
        resObj.insert("cmd", "SetPTPCommonParams");
    }

    if (params.size() == sizeof (VelocityRatioStruct)) {
        VelocityRatioStruct ptpParams;
        memcpy(&ptpParams, params, sizeof (VelocityRatioStruct));

        QJsonObject paramsObj;
        paramsObj.insert("velocityRatio", static_cast<double>(ptpParams.velocityRatio));
        paramsObj.insert("accelerationRatio", static_cast<double>(ptpParams.accelerationRatio));
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![84]
QJsonObject DMagicianProtocol::decodePTPCmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetPTPCmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![85]
QJsonObject DMagicianProtocol::decodePTPLParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetPTPLParams");
    } else {
        resObj.insert("cmd", "SetPTPLParams");
    }

    if (params.size() == sizeof (VelocityStruct)) {
        VelocityStruct ptpParams;
        memcpy(&ptpParams, params, sizeof (VelocityStruct));

        QJsonObject paramsObj;
        paramsObj.insert("velocity", static_cast<double>(ptpParams.velocity));
        paramsObj.insert("acceleration", static_cast<double>(ptpParams.acceleration));
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![86]
QJsonObject DMagicianProtocol::decodePTPWithLCmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetPTPWithLCmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![87]
QJsonObject DMagicianProtocol::decodePTPJump2Params(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetPTPJump2Params");
    } else {
        resObj.insert("cmd", "SetPTPJump2Params");
    }

    if (params.size() == sizeof (PTPJump2ParamsStruct)) {
        PTPJump2ParamsStruct ptpParams;
        memcpy(&ptpParams, params, sizeof (PTPJump2ParamsStruct));

        QJsonObject paramsObj;
        paramsObj.insert("startJumpHeight", static_cast<double>(ptpParams.startJumpHeight));
        paramsObj.insert("endJumpHeight", static_cast<double>(ptpParams.endJumpHeight));
        paramsObj.insert("zLimit", static_cast<double>(ptpParams.zLimit));
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![88]
QJsonObject DMagicianProtocol::decodePTPPOCmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetPTPPOCmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![89]
QJsonObject DMagicianProtocol::decodePTPPOWithLCmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetPTPPOWithLCmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![90]
QJsonObject DMagicianProtocol::decodeCPParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetCPParams");
    } else {
        resObj.insert("cmd", "SetCPParams");
    }

    if (params.size() == sizeof (CPParamsStruct)) {
        CPParamsStruct cpParams;
        memcpy(&cpParams, params, sizeof (CPParamsStruct));

        QJsonObject paramsObj;
        paramsObj.insert("targetAcc", static_cast<double>(cpParams.targetAcc));
        paramsObj.insert("junctionVel", static_cast<double>(cpParams.junctionVel));

        if (cpParams.isRealTimeTrack > 0) {
            paramsObj.insert("period", static_cast<double>(cpParams.acc));
            paramsObj.insert("isRealTimeTrack", true);
        } else {
            paramsObj.insert("acc", static_cast<double>(cpParams.acc));
            paramsObj.insert("isRealTimeTrack", false);
        }
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![91]
QJsonObject DMagicianProtocol::decodeCPCmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetCPCmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![92]
QJsonObject DMagicianProtocol::decodeCPLECmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetCPLECmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![93]
QJsonObject DMagicianProtocol::decodeCPRHeadHoldEnable(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetCPRHoldEnable");
    } else {
        resObj.insert("cmd", "SetCPRHoldEnable");
    }
    return resObj;
}

//![94]
QJsonObject DMagicianProtocol::decodeCPCommonParams(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetCPCommonParams");
    } else {
        resObj.insert("cmd", "SetCPCommonParams");
    }
    return resObj;
}

//![95]
QJsonObject DMagicianProtocol::decodeCP2Cmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetCP2Cmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![100]
QJsonObject DMagicianProtocol::decodeARCParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetARCParams");
    } else {
        resObj.insert("cmd", "SetARCParams");
    }

    if (params.size() == sizeof (ARCParamsStruct)) {
        ARCParamsStruct arcParams;
        memcpy(&arcParams, params, sizeof (ARCParamsStruct));

        QJsonObject paramsObj;
        paramsObj.insert("xyzVelocity", static_cast<double>(arcParams.xyzVelocity));
        paramsObj.insert("rVelocity", static_cast<double>(arcParams.rVelocity));
        paramsObj.insert("xyzAcceleration", static_cast<double>(arcParams.xyzAcceleration));
        paramsObj.insert("rAcceleration", static_cast<double>(arcParams.rAcceleration));
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![101]
QJsonObject DMagicianProtocol::decodeARCCmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetARCCmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![102]
QJsonObject DMagicianProtocol::decodeCircleCmd(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetCircleCmd");
    }
    return resObj;
}

//![103]
QJsonObject DMagicianProtocol::decodeARCCommonParams(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetARCCommonParams");
    } else {
        resObj.insert("cmd", "SetARCCommonParams");
    }
    return resObj;
}

//![110]
QJsonObject DMagicianProtocol::decodeWAITCmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetWAITCmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![120]
QJsonObject DMagicianProtocol::decodeTRIGCmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetTRIGCmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![130]
QJsonObject DMagicianProtocol::decodeIOMultiplexing(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetIOMultiplexing");
    } else {
        resObj.insert("cmd", "SetIOMultiplexing");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 port = 0;
        memcpy(&port, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("port", port);
        resObj.insert("params", paramsObj);
    } else if (params.size() == 2 * sizeof (quint8)) {
        quint8 port = 0, multiplex = 0;
        QDataStream out(&params, QIODevice::ReadOnly);
        out >> port >> multiplex;

        QJsonObject paramsObj;
        paramsObj.insert("port", port);
        paramsObj.insert("multiplex", multiplex);
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![131]
QJsonObject DMagicianProtocol::decodeIODO(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetIODO");
    } else {
        resObj.insert("cmd", "SetIODO");
    }

    if (params.size() == 2 * sizeof (quint8)) {
        quint8 port = 0, level = 0;
        QDataStream out(&params, QIODevice::ReadOnly);
        out >> port >> level;

        QJsonObject paramsObj;
        paramsObj.insert("port", port);
        paramsObj.insert("level", level);
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![132]
QJsonObject DMagicianProtocol::decodeIOPWM(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetIOPWM");
    } else {
        resObj.insert("cmd", "SetIOPWM");
    }

    if (params.size() == sizeof (IOPWMStruct)) {
        IOPWMStruct iopwm;
        memcpy(&iopwm, params, sizeof (IOPWMStruct));

        QJsonObject paramsObj;
        paramsObj.insert("port", iopwm.port);
        paramsObj.insert("dutyCycle", static_cast<double>(iopwm.dutyCycle));
        paramsObj.insert("frequency", static_cast<double>(iopwm.frequency));
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![133]
QJsonObject DMagicianProtocol::decodeIODI(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetIODI");
    }

    if (params.size() == 2 * sizeof (quint8)) {
        quint8 port = 0, level = 0;
        QDataStream out(&params, QIODevice::ReadOnly);
        out >> port >> level;

        QJsonObject paramsObj;
        paramsObj.insert("port", port);
        paramsObj.insert("level", level);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![134]
QJsonObject DMagicianProtocol::decodeIOADC(quint8 rw, QByteArray params)
{
#pragma pack(push, 1)
    struct IOADCStruct {
        quint8 port;
        quint16 value;
    };
#pragma pack(pop)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetIOADC");
    }

    if (params.size() == sizeof (IOADCStruct)) {
        IOADCStruct ioadc;
        memcpy(&ioadc, params, sizeof (IOADCStruct));

        QJsonObject paramsObj;
        paramsObj.insert("port", ioadc.port);
        paramsObj.insert("value", ioadc.value);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![135]
QJsonObject DMagicianProtocol::decodeEMotor(quint8 rw, QByteArray params)
{
    QJsonObject resObj;
    QJsonObject paramsObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetEMotor");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![136]
QJsonObject DMagicianProtocol::decodeEMotorS(quint8 rw, QByteArray params)
{
    QJsonObject resObj;
    QJsonObject paramsObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetEMotorS");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![137]
QJsonObject DMagicianProtocol::decodeColorSensor(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetColorSensor");
    } else {
        resObj.insert("cmd", "SetColorSensor");
    }

    if (params.size() == sizeof (ColorStruct)) {
        ColorStruct color;
        memcpy(&color, params, sizeof (ColorStruct));

        QJsonObject paramsObj;
        paramsObj.insert("red", color.red);
        paramsObj.insert("green", color.green);
        paramsObj.insert("blue", color.blue);
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![138]
QJsonObject DMagicianProtocol::decodeInfraredSensor(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetInfraredSensor");
    } else {
        resObj.insert("cmd", "SetInfraredSensor");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 status = 0;
        memcpy(&status, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("status", status);
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![139]
QJsonObject DMagicianProtocol::decodeIICInfo(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetIICInfo");
    } else {
        resObj.insert("cmd", "SetIICInfo");
    }

    return resObj;
}

//![140]
QJsonObject DMagicianProtocol::decodeAngleSensorStaticError(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetAngleSensorStaticError");
    } else {
        resObj.insert("cmd", "SetAngleSensorStaticError");
    }

    if (params.size() == sizeof (AngleSensorStruct)) {
        AngleSensorStruct angleParams;
        memcpy(&angleParams, params, sizeof (AngleSensorStruct));

        QJsonObject paramsObj;
        paramsObj.insert("rearArmAngleError", static_cast<double>(angleParams.rearArmAngle));
        paramsObj.insert("frontArmAngleError", static_cast<double>(angleParams.frontArmAngle));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![141]
QJsonObject DMagicianProtocol::decodeAngleSensorCoef(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetAngleSensorCoef");
    } else {
        resObj.insert("cmd", "SetAngleSensorCoef");
    }

    if (params.size() == sizeof (AngleSensorStruct)) {
        AngleSensorStruct angleParams;
        memcpy(&angleParams, params, sizeof (AngleSensorStruct));

        QJsonObject paramsObj;
        paramsObj.insert("rearArmAngle", static_cast<double>(angleParams.rearArmAngle));
        paramsObj.insert("frontArmAngle", static_cast<double>(angleParams.frontArmAngle));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![142]
QJsonObject DMagicianProtocol::decodeBaseDecoderStaticError(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "SetBaseDecoderStaticError");
    } else {
        resObj.insert("cmd", "GetBaseDecoderStaticError");
    }

    if (params.size() == sizeof (float)) {
        float baseDecoderError = 0;
        memcpy(&baseDecoderError, params, sizeof (float));

        QJsonObject paramsObj;
        paramsObj.insert("baseDecoderError", baseDecoderError);
        resObj.insert("params", paramsObj);
    }

    return resObj;
}

//![143]
QJsonObject DMagicianProtocol::decodeLRHandCalibrateValue(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetLRHandCalibrateValue");
    } else {
        resObj.insert("cmd", "SetLRHandCalibrateValue");
    }
    return resObj;
}

//![150]
QJsonObject DMagicianProtocol::decodeWIFIConfigMode(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetWIFIConfigMode");
    } else {
        resObj.insert("cmd", "SetWIFIConfigMode");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 isEnable = 0;
        memcpy(&isEnable, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("enable", isEnable > 0 ? true : false);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![151]
QJsonObject DMagicianProtocol::decodeWIFISSID(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetWIFISSID");
    } else {
        resObj.insert("cmd", "SetWIFISSID");
    }

    if (params.size() > 0) {
        char* ssid = params.data();

        QJsonObject paramsObj;
        paramsObj.insert("ssid", ssid);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![152]
QJsonObject DMagicianProtocol::decodeWIFIPassword(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetWIFIPassword");
    } else {
        resObj.insert("cmd", "SetWIFIPassword");
    }

    if (params.size() > 0) {
        char* password = params.data();

        QJsonObject paramsObj;
        paramsObj.insert("password", password);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![153]
QJsonObject DMagicianProtocol::decodeWIFIIPAddress(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetWIFIIPAddress");
    } else {
        resObj.insert("cmd", "SetWIFIIPAddress");
    }

    if (params.size() == sizeof (WIFIIPAddressStruct)) {
        WIFIIPAddressStruct wifiIpAddrParams;
        memcpy(&wifiIpAddrParams, params, sizeof (WIFIIPAddressStruct));

        QJsonArray wifiIpAddrParamsArray;
        for (int i = 0; i < 4; i++) {
            wifiIpAddrParamsArray.append(static_cast<double>(wifiIpAddrParams.address[i]));
        }

        QJsonObject paramsObj;
        paramsObj.insert("address", wifiIpAddrParamsArray);
        paramsObj.insert("dhcp", wifiIpAddrParams.dhcp);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![154]
QJsonObject DMagicianProtocol::decodeWIFINetmask(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetWIFINetmask");
    } else {
        resObj.insert("cmd", "SetWIFINetmask");
    }

    if (params.size() == sizeof (WifiAddressStruct)) {
        WifiAddressStruct wifiNetmask;
        memcpy(&wifiNetmask, params, sizeof (WifiAddressStruct));

        QJsonArray wifiNetmaskParamsArray;
        for (int i = 0; i < 4; i++) {
            wifiNetmaskParamsArray.append(static_cast<double>(wifiNetmask.address[i]));
        }

        QJsonObject paramsObj;
        paramsObj.insert("address", wifiNetmaskParamsArray);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![155]
QJsonObject DMagicianProtocol::decodeWIFIGateway(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetWIFIGateway");
    } else {
        resObj.insert("cmd", "SetWIFIGateway");
    }

    if (params.size() == sizeof (WifiAddressStruct)) {
        WifiAddressStruct wifiGateWay;
        memcpy(&wifiGateWay, params, sizeof (WifiAddressStruct));

        QJsonArray wifiGateWayArray;
        for (int i = 0; i < 4; i++) {
            wifiGateWayArray.append(static_cast<double>(wifiGateWay.address[i]));
        }

        QJsonObject paramsObj;
        paramsObj.insert("address", wifiGateWayArray);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![156]
QJsonObject DMagicianProtocol::decodeWIFIDNS(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetWIFIDNS");
    } else {
        resObj.insert("cmd", "SetWIFIDNS");
    }

    if (params.size() == sizeof (WifiAddressStruct)) {
        WifiAddressStruct wifiDns;
        memcpy(&wifiDns, params, sizeof (WifiAddressStruct));

        QJsonArray wifiDnsArray;
        for (int i = 0; i < 4; i++) {
            wifiDnsArray.append(static_cast<double>(wifiDns.address[i]));
        }

        QJsonObject paramsObj;
        paramsObj.insert("address", wifiDnsArray);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![157]
QJsonObject DMagicianProtocol::decodeWIFIConnectStatus(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetWIFIConnectStatus");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 isConnected = 0;
        memcpy(&isConnected, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("isConnected", isConnected?true:false);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![170]
QJsonObject DMagicianProtocol::decodeLostStep(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetLostStepParams");
    }
    return resObj;
}

//![171]
QJsonObject DMagicianProtocol::decodeLostStepCmd(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetLostStepCmd");
    }
    return resObj;
}

//![172]
QJsonObject DMagicianProtocol::decodeCollisionCheck(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetCollisionCheck");
    } else {
        resObj.insert("cmd", "SetCollisionCheck");
    }

    if (params.size() == sizeof (CollisionCheckStruct)) {
        CollisionCheckStruct collision;
        memcpy(&collision, params, sizeof (CollisionCheckStruct));

        QJsonObject paramsObj;
        paramsObj.insert("isEnabled", collision.enable > 0 ? true : false);
        paramsObj.insert("threshold", static_cast<double>(collision.threshold));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![192]
QJsonObject DMagicianProtocol::decodeDeviceStatus(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetDeviceStatus");
    } else {
        resObj.insert("cmd", "SetDeviceStatus");
    }
    return resObj;
}

//![193]
QJsonObject DMagicianProtocol::decodeDeviceRunAPP(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetDeviceRunAPP");
    } else {
        resObj.insert("cmd", "SetDeviceRunAPP");
    }
    return resObj;
}

//![194]
QJsonObject DMagicianProtocol::decodeUpgradeStatus(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetUpgradeStatus");
    } else {
        resObj.insert("cmd", "SetUpgradeStatus");
    }
    if (params.size() == sizeof (quint8)) {
        quint8 latest;
        memcpy(&latest, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("latest", latest > 0 ? true : false);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![195]
QJsonObject DMagicianProtocol::decodeRunUpgrade(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetRunUpgrade");
    } else {
        resObj.insert("cmd", "SetRunUpgrade");
    }
    return resObj;
}

//![196]
QJsonObject DMagicianProtocol::decodeStartTransmit(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetStartTransmit");
    } else {
        resObj.insert("cmd", "SetStartTransmit");
    }
    return resObj;
}

//![197]
QJsonObject DMagicianProtocol::decodeUpgradeVeify(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetUpgradeVeify");
    } else {
        resObj.insert("cmd", "SetUpgradeVeify");
    }
    return resObj;
}

//![198]
QJsonObject DMagicianProtocol::decodeDeviceRestart(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetRestartDeviceCmd");
    }

    return resObj;
}

//![200]
QJsonObject DMagicianProtocol::decodeColorObj(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if(rw == 1) {
        resObj.insert("cmd", "GetColorObjExist");
        quint8 status;
        memcpy(&status, params, sizeof (quint8));
        QJsonObject paramsObj;
        paramsObj.insert("status", status);

        resObj.insert("params", paramsObj);
    }
    if(rw == 0) {
        resObj.insert("cmd", "GetColorObjCoordinate");
        ColorObjCoordinateStruct coorstruct;
        memcpy(&coorstruct, params, sizeof (ColorObjCoordinateStruct));
        QJsonObject paramsObj;
        paramsObj.insert("color", coorstruct.color);
        paramsObj.insert("coordinate", coorstruct.coordinate);
        resObj.insert("params", paramsObj);

    }
    return  resObj;
}



//![210]
QJsonObject DMagicianProtocol::decodeSeeedDistanceSensor(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetSeeedDistanceSensor");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 distance = 0;
        memcpy(&distance, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("distance", distance);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![211]
QJsonObject DMagicianProtocol::decodeSeeedTempSensor(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetSeeedTempSensor");
    } else {
        resObj.insert("cmd", "SetSeeedTempSensor");
    }

    if (params.size() == 2 * sizeof (quint8)) {
        quint8 temperature = 0, humidity = 0;
        QDataStream out(&params, QIODevice::ReadOnly);
        out >> temperature >> humidity;

        QJsonObject paramsObj;
        paramsObj.insert("temperature", temperature);
        paramsObj.insert("humidity", humidity);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![212]
QJsonObject DMagicianProtocol::decodeSeeedLightSensor(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetSeeedLightSensor");
    } else {
        resObj.insert("cmd", "SetSeeedLightSensor");
    }

    if (params.size() == sizeof (quint32)) {
        quint32 intensity = 0;
        memcpy(&intensity, params, sizeof (quint32));

        QJsonObject paramsObj;
        paramsObj.insert("intensity", static_cast<double>(intensity));
        resObj.insert("params", paramsObj);
    }

    return resObj;
}

//![213]
QJsonObject DMagicianProtocol::decodeSeeedColorSensor(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetSeeedColorSensor");
    } else {
        resObj.insert("cmd", "SetSeeedColorSensor");
    }

    if (params.size() == sizeof (SeeedColorStruct)) {
        SeeedColorStruct color;
        memcpy(&color, params, sizeof (SeeedColorStruct));

        QJsonObject paramsObj;
        paramsObj.insert("red", color.red);
        paramsObj.insert("green", color.green);
        paramsObj.insert("blue", color.blue);
        paramsObj.insert("cct", color.cct);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![214]
QJsonObject DMagicianProtocol::decodeSeeedRGBLed(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetSeeedRGBLed");
    }

    return resObj;
}

//![215]
QJsonObject DMagicianProtocol::decodeMCUTemperature(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetMCUTemperature");
    }

    return resObj;
}

//![216]
QJsonObject DMagicianProtocol::decodeDobotSensor(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (params.count() > 0) {
        quint8 subid = static_cast<quint8>(params[0]);
        DecodeFunction function = m_decodeDobotSensorFuncMap.value(subid);

        if (function) {
            resObj = (this->*function)(rw, params.mid(1));
        } else {
            qDebug() << "No matching parseDobotSensorParams function.";
        }
    }
    return resObj;
}

//![217]
QJsonObject DMagicianProtocol::decodeBoxFunc(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (params.count() > 0) {
        uint8_t subid =  static_cast<uint8_t>(params[0]);
        DecodeFunction function = m_decodeBoxFuncMap.value(subid);

        if (function) {
            resObj = (this->*function)(rw, params.mid(0));
        } else {
            qDebug() << "No matching BoxParams function.";
        }
    }
    return resObj;
}

//![240]
QJsonObject DMagicianProtocol::decodeQueuedCmdStart(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "QueuedCmdStart");
    }
    return resObj;
}

//![241]
QJsonObject DMagicianProtocol::decodeQueuedCmdStop(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "QueuedCmdStop");
    }
    return resObj;
}

//![242]
QJsonObject DMagicianProtocol::decodeQueuedCmdForceStop(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "QueuedCmdForceStop");
    }
    return resObj;
}

//![243]
QJsonObject DMagicianProtocol::decodeQueuedCmdStartDownload(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "QueuedCmdStartDownload");
    }
    return resObj;
}

//![244]
QJsonObject DMagicianProtocol::decodeQueuedCmdStopDownload(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "QueuedCmdStopDownload");
    }
    return resObj;
}

//![245]
QJsonObject DMagicianProtocol::decodeQueuedCmdClear(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "QueuedCmdClear");
    }
    return resObj;
}

//![246]
QJsonObject DMagicianProtocol::decodeQueuedCmdCurrentIndex(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetQueuedCmdCurrentIndex");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![247]
QJsonObject DMagicianProtocol::decodeQueuedCmdLeftSpace(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetQueuedCmdLeftSpace");
    }

    QJsonObject paramsObj;
    if (params.size() == sizeof (quint64)) {
        quint64 leftSpace = 0;
        memcpy(&leftSpace, params, sizeof (quint64));

        paramsObj.insert("leftSpace", static_cast<double>(leftSpace));
        resObj.insert("params", paramsObj);
    }
    else if (params.size() == sizeof (quint32))
    {
        quint32 leftSpace = 0;
        memcpy(&leftSpace, params, sizeof (quint32));

        paramsObj.insert("leftSpace", static_cast<double>(leftSpace));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![248]
QJsonObject DMagicianProtocol::decodeQueuedCmdMotionFinish(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetQueuedCmdMotionFinish");
    }
    return resObj;
}

//![216-1]
QJsonObject DMagicianProtocol::decodeSensorRGBLEDInit(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorRGBLEDInit");
    }
    return resObj;
}

//![216-2]
QJsonObject DMagicianProtocol::decodeSensorSetRGBLEDVlaue(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorSetRGBLEDVlaue");
    }
    return resObj;
}

//![216-3]
QJsonObject DMagicianProtocol::decodeSensorRGBLEDTurnOFFAll(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorRGBLEDTurnOFFAll");
    }
    return resObj;
}

//![216-4]
QJsonObject DMagicianProtocol::decodeSensorSetRGBLEDState(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorSetRGBLEDState");
    }
    return resObj;
}

//![216-10]
QJsonObject DMagicianProtocol::decodeSensorOledInit(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorOledInit");
    }
    return resObj;
}

//![216-11]
QJsonObject DMagicianProtocol::decodeSensorOledClear(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorOledClear");
    }
    return resObj;
}

//![216-12]
QJsonObject DMagicianProtocol::decodeSensorOledDisplay(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorOledDisplay");
    }
    return resObj;
}

//![216-20]
QJsonObject DMagicianProtocol::decodeSensorKnobInit(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorKnobInit");
    }
    return resObj;
}

//![216-21]
QJsonObject DMagicianProtocol::decodeSensorGetKnob(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "SensorGetKnob");
    }

    if (params.size() == sizeof (quint16)) {
        quint16 value = 0;
        memcpy(&value, params, sizeof (quint16));

        QJsonObject paramsObj;
        paramsObj.insert("value", value);
        resObj.insert("params", paramsObj);
    }

    return resObj;
}

//![216-22]
QJsonObject DMagicianProtocol::decodeSensorVoiceInit(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorVoiceInit");
    }
    return resObj;
}

//![216-23]
QJsonObject DMagicianProtocol::decodeSensorGetVoice(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "SensorGetVoice");
    }

    if (params.size() == sizeof (quint16)) {
        quint16 value = 0;
        memcpy(&value, params, sizeof (quint16));

        QJsonObject paramsObj;
        paramsObj.insert("value", value);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-24]
QJsonObject DMagicianProtocol::decodeSensorLightInit(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorLightInit");
    }
    return resObj;
}

//![216-25]
QJsonObject DMagicianProtocol::decodeSensorGetLight(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "SensorGetLight");
    }

    if (params.size() == sizeof (quint16)) {
        quint16 value = 0;
        memcpy(&value, params, sizeof (quint16));

        QJsonObject paramsObj;
        paramsObj.insert("value", value);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-26]
QJsonObject DMagicianProtocol::decodeSensorPEInit(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorPEInit");
    }
    return resObj;
}

//![216-27]
QJsonObject DMagicianProtocol::decodeSensorGetPEState(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "SensorGetPEState");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 value = 0;
        memcpy(&value, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("value", value > 0 ? true : false);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-28]
QJsonObject DMagicianProtocol::decodeSensorUltrasonicInit(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "SensorUltrasonicInit");
    }
    return resObj;
}

//![216-29]
QJsonObject DMagicianProtocol::decodeSensorUltrasonicGet(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "SensorUltrasonicGet");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 value = 0;
        memcpy(&value, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("value", value);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-30]
QJsonObject DMagicianProtocol::decodeSensorSHT31Init(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "SensorSHT31Init");
    }
    return resObj;
}

//![216-31]
QJsonObject DMagicianProtocol::decodeSensorGetSHT31(quint8 rw, QByteArray params)
{
    struct SHT31Struct {
        quint8 tem;
        quint8 hum;
    };

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "SensorGetSHT31");
    }

    if (params.size() == sizeof (SHT31Struct)) {
        SHT31Struct sht;
        memcpy(&sht, params, sizeof (SHT31Struct));

        QJsonObject paramsObj;
        paramsObj.insert("tem", sht.tem);
        paramsObj.insert("hum", sht.hum);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-32]
QJsonObject DMagicianProtocol::decodeSensorColorInit(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "SensorColorInit");
    }
    return resObj;
}

//![216-33]
QJsonObject DMagicianProtocol::decodeSensorGetColor(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "SensorGetColor");
    }

    if (params.size() == sizeof (quint16)) {
        quint16 value = 0;
        memcpy(&value, params, sizeof (quint16));

        QJsonObject paramsObj;
        paramsObj.insert("value", value);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-34]
QJsonObject DMagicianProtocol::decodeSensorGetColorRes(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "SensorGetColorRes");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 res = 0;
        memcpy(&res, params, sizeof (quint8));

        QString color;
        if (res == 0) {
            color = "none";
        } else if (res == 1) {
            color = "red";
        } else if (res == 2) {
            color = "green";
        } else if (res == 3) {
            color = "blue";
        } else if (res == 4) {
            color = "yellow";
        } else if (res == 5) {
            color = "black";
        } else if (res == 6) {
            color = "white";
        }

        QJsonObject paramsObj;
        paramsObj.insert("color", color);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-40]
QJsonObject DMagicianProtocol::decodeSensorXBeeInit(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorXBeeInit");
    }
    return resObj;
}

//![216-41]
QJsonObject DMagicianProtocol::decodeSensorXBeeSend(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorXBeeSend");
    }
    return resObj;
}

//![216-42]
QJsonObject DMagicianProtocol::decodeSensorXBeeReceive(quint8 rw, QByteArray params)
{
    QJsonObject resObj;
    QJsonObject paramsObj;

    if (rw == 0) {
        resObj.insert("cmd", "SensorXBeeReceive");
    }

    if (params.size() > 0) {
        quint8 length = 0;
        memcpy(&length, params, sizeof (quint8));

        QByteArray textArr = params.mid(1, length);
        char *text = textArr.data();
        paramsObj.insert("text", text);
    } else {
        paramsObj.insert("text", "");
    }

    resObj.insert("params", paramsObj);
    return resObj;
}

//![216-43]
QJsonObject DMagicianProtocol::decodeSensorXBeeClear(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "SensorXBeeClear");
    }
    return resObj;
}

//![216-50]
QJsonObject DMagicianProtocol::decodeSensorSYNInit(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorSYNInit");
    }
    return resObj;
}

//![216-51]
QJsonObject DMagicianProtocol::decodeSensorSetSYN(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorSetSYN");
    }
    return resObj;
}

//![216-52]
QJsonObject DMagicianProtocol::decodeSensorSetSYNMusic(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorSetSYNMusic");
    }
    return resObj;
}

//![216-53]
QJsonObject DMagicianProtocol::decodeSensorSendSYN(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorSendSYN");
    }
    return resObj;
}

//![216-54]
QJsonObject DMagicianProtocol::decodeSensorSetSYNCmd(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SensorSetSYNCmd");
    }
    return resObj;
}

//![216-60]
QJsonObject DMagicianProtocol::decodeBleInf(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetBleInf");
        if (params.size() >= 1) {
            QString state;
            state.append(params.mid(0, 1).toHex());

            QJsonObject paramsObj;
            paramsObj.insert("state", state.toInt());
            resObj.insert("params", paramsObj);
        }
    } else if (rw == 0) {
        resObj.insert("cmd", "GetBleInf");
        if (params.size() >= 2) {
            char *name = params.mid(2).data();

            QJsonObject paramsObj;
            paramsObj.insert("name", name);
            resObj.insert("params", paramsObj);
        }
    }

    return resObj;
}

//![216-61]
QJsonObject DMagicianProtocol::decodeBleConnect(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "BleConnect");
        if (params.size() >= 1) {
            QString state;
            state.append(params.mid(0, 1).toHex());

            QJsonObject paramsObj;
            paramsObj.insert("state", state.toInt());
            resObj.insert("params", paramsObj);
        }
    }
    return resObj;
}

//![216-63]
QJsonObject DMagicianProtocol::decodeBleReadOneData(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "BleReadOneData");
    }
    if (params.size() >= 1) {
        quint8 length = 0;
        memcpy(&length, params, sizeof (quint8));
        QByteArray dataArr = params.mid(1, length);
        quint8 data = 0;
        memcpy(&data, dataArr, sizeof (quint8));
        QJsonObject paramsObj;
        paramsObj.insert("data", data);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-64]
QJsonObject DMagicianProtocol::decodeBleWriteOneData(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "BleWriteOneData");
    }
    return resObj;
}

//![216-65]
QJsonObject DMagicianProtocol::decodeSetBleMesh(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetBleMesh");
        if (params.size() >= 1) {
            quint8 subid;
            memcpy(&subid, params, sizeof (quint8));
            QJsonObject paramsObj;
            resObj.insert("status", paramsObj);
        }
    }
    return resObj;
}

//![216-66]
QJsonObject DMagicianProtocol::decodeBleReadMeshData(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "BleReadMeshData");
        if (params.size() >= 1) {
            QString state;
            state.append(params.mid(0, 1).toHex());

            QJsonObject paramsObj;
            paramsObj.insert("state", state.toInt());

            QString t_devID;
            t_devID.append(params.mid(1, 1).toHex());

            QByteArray dataArr = params.mid(3);

            paramsObj.insert("devID", t_devID.toInt());
            paramsObj.insert("data", QString(dataArr));
            resObj.insert("params", paramsObj);
        }
    }
    return resObj;
}

//![216-67]
QJsonObject DMagicianProtocol::decodeBleWriteMeshData(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "BleWriteMeshData");
        if (params.size() >= 1) {
            QString state;
            state.append(params.mid(0, 1).toHex());

            QJsonObject paramsObj;
            paramsObj.insert("state", state.toInt());
            resObj.insert("params", paramsObj);
        }
    }
    return resObj;
}

//![216-68]
QJsonObject DMagicianProtocol::decodeBleClearMeshData(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if(rw == 1){
        resObj.insert("cmd", "BleClearMeshData");
        if (params.size() == sizeof (quint8)){
            quint8 subid;
            memcpy(&subid, params, sizeof (quint8));
            if(subid == 68){
                resObj.insert("result",true);
            }
        }
    } else {
        resObj.insert("result", false);
    }
    return resObj;
}

//![216-70]
QJsonObject DMagicianProtocol::decodeSetSensorHandModel(quint8 rw, QByteArray params)
{
    Q_UNUSED(rw);
    QJsonObject resObj;

    resObj.insert("cmd", "SetSensorHandModel");
    if (params.size() == sizeof (quint8)){
        quint8 initstate;
        memcpy(&initstate, params, sizeof (quint8));
        QJsonObject paramsObj;
        if (initstate == 1) {
            paramsObj.insert("InitState", true);
        } else {
            paramsObj.insert("InitState", false);
        }
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-71]
QJsonObject DMagicianProtocol::decodeGetSensorHandModel(quint8 rw, QByteArray params)
{
    Q_UNUSED(rw);
    QJsonObject resObj;

    resObj.insert("cmd", "GetSensorHandModel");
    if (params.size() == sizeof (quint8)) {
        quint8 data;
        memcpy(&data, params, sizeof (quint8));
        QJsonObject paramsObj;
        paramsObj.insert("data", data);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-72]
QJsonObject DMagicianProtocol::decodeIsSensorHandModel(quint8 rw, QByteArray params)
{
    Q_UNUSED(rw);
    QJsonObject resObj;

    resObj.insert("cmd", "IsSensorHandModel");
    if (params.size() == sizeof (quint8)) {
        quint8 data;
        memcpy(&data, params, sizeof (quint8));
        QJsonObject paramsObj;
        paramsObj.insert("data", data);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-73]
QJsonObject DMagicianProtocol::decodeIsJoystickButton(quint8 rw, QByteArray params)
{
    Q_UNUSED(rw);
    QJsonObject resObj;

    resObj.insert("cmd", "IsJoystickButton");
    if (params.size() == sizeof (quint8)) {
        quint8 data;
        memcpy(&data, params, sizeof (quint8));
        QJsonObject paramsObj;
        paramsObj.insert("res", data);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-74]
QJsonObject DMagicianProtocol::decodeGetJoystickPos(quint8 rw, QByteArray params)
{
    Q_UNUSED(rw);
    QJsonObject resObj;

    resObj.insert("cmd", "GetJoystickPos");
    if (params.size() == sizeof (JoystickPos)) {
        JoystickPos data;
        memcpy(&data, params, sizeof (JoystickPos));
        QJsonObject paramsObj;
        paramsObj.insert("x", data.x);
        paramsObj.insert("y", data.y);
        paramsObj.insert("z", data.z);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-75]
QJsonObject DMagicianProtocol::decodeGetJoystickButton(quint8 rw, QByteArray params)
{
    Q_UNUSED(rw);
    QJsonObject resObj;

    resObj.insert("cmd", "GetJoystickButton");
    quint8 size = static_cast<quint8>(params.at(0));

    QJsonArray btns;
    for(int i(0); i < size; ++i) {
        QJsonObject paramsObj;
        btns.insert(i, static_cast<quint8>(params.at(i+1)));
    }

    QJsonObject paramsObj;
    paramsObj.insert("res", btns);
    resObj.insert("params", paramsObj);

    return resObj;
}

//![216-76]
QJsonObject DMagicianProtocol::decodeIsPirDetected(quint8 rw, QByteArray params)
{
    Q_UNUSED(rw);
    QJsonObject resObj;

    resObj.insert("cmd", "IsPirDetected");
    if (params.size() == sizeof (quint8)) {
        quint8 data;
        memcpy(&data, params, sizeof (quint8));
        QJsonObject paramsObj;
        paramsObj.insert("res", data);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-77]
QJsonObject DMagicianProtocol::decodeGetButtonStatus(quint8 rw, QByteArray params)
{
    Q_UNUSED(rw);
    QJsonObject resObj;

    resObj.insert("cmd", "GetButtonStatus");
    if (params.size() == sizeof (GetButtonStatusStruct)) {
        GetButtonStatusStruct status;
        memcpy(&status, params, sizeof (GetButtonStatusStruct));

        QJsonObject paramsObj;
        paramsObj.insert("redBtn", status.redBtn);
        paramsObj.insert("blueBtn", status.blueBtn);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-78]
QJsonObject DMagicianProtocol::decodeGetRedButtonStatus(quint8 rw, QByteArray params)
{
    Q_UNUSED(rw);
    QJsonObject resObj;

    resObj.insert("cmd", "GetRedButtonStatus");
    if (params.size() == sizeof (quint8)) {
        quint8 status = 0;
        memcpy(&status, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("status", status);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![216-79]
QJsonObject DMagicianProtocol::decodeGetBlueButtonStatus(quint8 rw, QByteArray params)
{
    Q_UNUSED(rw);
    QJsonObject resObj;

    resObj.insert("cmd", "GetBlueButtonStatus");
    if (params.size() == sizeof (quint8)) {
        quint8 status = 0;
        memcpy(&status, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("status", status);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

QJsonObject DMagicianProtocol::decodeBleSetMaster(quint8 rw, QByteArray params)
{
    QJsonObject resObj;
    if (rw == 1) {
        resObj.insert("cmd", "BluetoothSetMaster");
        if (params.size() >= 1) {
            int state = params[0];
            QJsonObject paramsObj;
            paramsObj.insert("state", state);
            resObj.insert("params", paramsObj);
        }
    }
    return resObj;
}

QJsonObject DMagicianProtocol::decodeBleSetSlave(quint8 rw, QByteArray params)
{
    QJsonObject resObj;
    if (rw == 1) {
        resObj.insert("cmd", "BluetoothSetSlave");
        if (params.size() >= 1) {
            int state = params[0];
            QJsonObject paramsObj;
            paramsObj.insert("state", state);
            resObj.insert("params", paramsObj);
        }
    }
    return resObj;
}

QJsonObject DMagicianProtocol::decodeBleRecvData(quint8 rw, QByteArray params)
{
    QJsonObject resObj;
    if (rw == 1) {
        resObj.insert("cmd", "BluetoothRecvData");
    }
    if (params.size() >= 2) {
        int state = params[0]; //无数据时0，有数据时1
        quint8 length = params[1]; //数据长度，最大21字节，不含终止符
        QByteArray dataArr = params.mid(2, length);
        QJsonObject paramsObj;
        paramsObj.insert("state", state);
        paramsObj.insert("data", QString(dataArr));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

QJsonObject DMagicianProtocol::decodeBleSendData(quint8 rw, QByteArray params)
{
    QJsonObject resObj;
    if (rw == 1) {
        resObj.insert("cmd", "BluetoothSendData");
        if (params.size() >= 1) {
            int state = params[0];
            QJsonObject paramsObj;
            paramsObj.insert("state", state);
            resObj.insert("params", paramsObj);
        }
    }
    return resObj;
}

QJsonObject DMagicianProtocol::decodeBleClearCacheData(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)
    QJsonObject resObj;
    if (rw == 1) {
        resObj.insert("cmd", "BluetoothClearCacheData");
    }
    return resObj;
}

QJsonObject DMagicianProtocol::decodeBleGetMacAddress(quint8 rw, QByteArray params)
{
    qDebug()<<"======>"<<params.toHex();
    QJsonObject resObj;
    if (rw == 1) {
        resObj.insert("cmd", "BluetoothGetMacAddress");
    }
    if (params.size() >= 2) {
        int state = params[0]; //获取到mac时为0，未获取到时为1
        quint8 length = params[1]; //数据长度，最大21字节，不含终止符
        QByteArray dataArr = params.mid(2, length);
        QJsonObject paramsObj;
        paramsObj.insert("state", state);
        paramsObj.insert("mac", QString(dataArr));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

/**********************************************************************************
 * *** ENCODE *** ENCODE *** ENCODE *** ENCODE *** ENCODE *** ENCODE *** ENCODE ***
 *********************************************************************************/

/* 设备信息 */
//! [0]
void DMagicianProtocol::encodeSetDeviceSN(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 0;
    payload.p_ctrl.c_rw = 1;

    checkStringValue(params, "deviceSN");

    QString deviceSN = params.value("deviceSN").toString();
    payload.p_params.append(deviceSN.toLatin1() + '\0');
}

void DMagicianProtocol::encodeGetDeviceSN(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 0;
    payload.p_ctrl.c_rw = 0;
}

//! [1]
void DMagicianProtocol::encodeSetDeviceName(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 1;
    payload.p_ctrl.c_rw = 1;

    checkStringValue(params, "deviceName");

    QString deviceName = params.value("deviceName").toString();
    payload.p_params.append(deviceName.toUtf8() + '\0');
}

void DMagicianProtocol::encodeGetDeviceName(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 1;
    payload.p_ctrl.c_rw = 0;
}

//! [2]
void DMagicianProtocol::encodeGetDeviceVersion(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)
    checkStringValue(params, "deviceName");
    m_Device = params.value("deviceName").toString();
    payload.p_cmdID = 2;
    payload.p_ctrl.c_rw = 0;
}

//! [3]
void DMagicianProtocol::encodeSetDeviceWithL(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 3;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "enable");
    checkIntValue(params, "version");

    quint8 enable = params.value("enable").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&enable), sizeof (quint8));

    quint8 version = static_cast<quint8>(params.value("version").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&version), sizeof (quint8));
}

void DMagicianProtocol::encodeGetDeviceWithL(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 3;
    payload.p_ctrl.c_rw = 0;
}

//! [4]
void DMagicianProtocol::encodeGetDeviceTime(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 4;
    payload.p_ctrl.c_rw = 0;
}

//! [5]
void DMagicianProtocol::encodeGetDeviceID(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 5;
    payload.p_ctrl.c_rw = 0;
}

//! [7]
void DMagicianProtocol::encodeGetProductName(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 7;
    payload.p_ctrl.c_rw = 0;
}

//! [8]
void DMagicianProtocol::encodeSetOLEDDisplay(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 8;
    payload.p_ctrl.c_rw = 1;

    checkStringValue(params, "text");

    QString text = params.value("text").toString();
    payload.p_params.append(text.toUtf8() + '\0');
}

void DMagicianProtocol::encodeSetProgressBar(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 9;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "value");

    quint8 value = static_cast<quint8>(params.value("value").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&value), sizeof (quint8));
}

/* 实时位姿 */
//! [10]
void DMagicianProtocol::encodeGetPose(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 10;
    payload.p_ctrl.c_rw = 0;
}

//! [11]
void DMagicianProtocol::encodeResetPose(PacketPayload &payload, QJsonObject params)
{
#pragma pack(push, 1)
    struct ResetPoseStruct {
        quint8 manual;
        float rearArmAngle;
        float frontArmAngle;
    };
#pragma pack(pop)

    payload.p_cmdID = 11;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "manual");
    checkFloatValue(params, "rearArmAngle");
    checkFloatValue(params, "frontArmAngle");

    ResetPoseStruct resetpose;
    resetpose.manual = static_cast<quint8>(params.value("manual").toInt());
    resetpose.rearArmAngle = static_cast<float>(params.value("rearArmAngle").toDouble());
    resetpose.frontArmAngle = static_cast<float>(params.value("frontArmAngle").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&resetpose), sizeof (ResetPoseStruct));
}

//! [13]
void DMagicianProtocol::encodeGetPoseL(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 13;
    payload.p_ctrl.c_rw = 0;
}

//! [15]
void DMagicianProtocol::encodeCheckPoseLimit(PacketPayload &payload, QJsonObject params)
{
#pragma pack(push, 1)
    struct CheckPoseLimitStruct {
        quint8 isJoint;
        float x;
        float y;
        float z;
        float r;
    };
#pragma pack(pop)

    payload.p_cmdID = 15;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "isJoint");
    checkFloatValue(params, "x");
    checkFloatValue(params, "y");
    checkFloatValue(params, "z");
    checkFloatValue(params, "r");

    CheckPoseLimitStruct checkPose;
    checkPose.isJoint = params.value("isJoint").toBool() ? 1 : 0;
    checkPose.x = static_cast<float>(params.value("x").toDouble());
    checkPose.y = static_cast<float>(params.value("y").toDouble());
    checkPose.z = static_cast<float>(params.value("z").toDouble());
    checkPose.r = static_cast<float>(params.value("r").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&checkPose), sizeof (CheckPoseLimitStruct));
}

/* 报警功能 */
//! [20]
void DMagicianProtocol::encodeGetAlarmsState(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 20;
    payload.p_ctrl.c_rw = 0;
}

void DMagicianProtocol::encodeClearAllAlarmsState(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 20;
    payload.p_ctrl.c_rw = 1;
}

/* 回零 */
//! [30]
void DMagicianProtocol::encodeSetHOMEParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 30;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "x");
    checkFloatValue(params, "y");
    checkFloatValue(params, "z");
    checkFloatValue(params, "r");

    PositionStruct homePose;
    homePose.x = static_cast<float>(params.value("x").toDouble());
    homePose.y = static_cast<float>(params.value("y").toDouble());
    homePose.z = static_cast<float>(params.value("z").toDouble());
    homePose.r = static_cast<float>(params.value("r").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&homePose), sizeof (PositionStruct));
}

void DMagicianProtocol::encodeGetHOMEParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 30;
    payload.p_ctrl.c_rw = 0;
}

//! [31]
void DMagicianProtocol::encodeSetHOMECmd(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 31;
    payload.p_ctrl.c_rw = 1;

#if 0
    quint32 temp = 0;
    payload.p_params.append(reinterpret_cast<char *>(&temp), sizeof (quint32));
#endif
}

//! [32]
void DMagicianProtocol::encodeSetAutoLeveling(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 32;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "enable");
    checkFloatValue(params, "precision");

    quint8 enable = params.value("enable").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&enable), sizeof (quint8));

    float precision = static_cast<float>(params.value("precision").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&precision), sizeof (float));
}

void DMagicianProtocol::encodeGetAutoLeveling(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 32;
    payload.p_ctrl.c_rw = 0;
}

//! [33]
void DMagicianProtocol::encodeGetGoHomeStatus(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 33;
    payload.p_ctrl.c_rw = 0;
}

//! [40]
void DMagicianProtocol::encodeSetHHTTrigMode(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 40;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "mode");

    quint8 mode = static_cast<quint8>(params.value("mode").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&mode), sizeof (quint8));
}

void DMagicianProtocol::encodeGetHHTTrigMode(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 40;
    payload.p_ctrl.c_rw = 0;
}

//! [41]
void DMagicianProtocol::encodeSetHHTTrigOutputEnabled(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 41;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "enable");

    quint8 enable = params.value("enable").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&enable), sizeof (quint8));
}

void DMagicianProtocol::encodeGetHHTTrigOutputEnabled(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 41;
    payload.p_ctrl.c_rw = 0;
}

//! [42]
void DMagicianProtocol::encodeGetHHTTrigOutput(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 42;
    payload.p_ctrl.c_rw = 0;
}

//! [60]
void DMagicianProtocol::encodeSetEndEffectorParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 60;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "xOffset");
    checkFloatValue(params, "yOffset");
    checkFloatValue(params, "zOffset");

    float xOffset = static_cast<float>(params.value("xOffset").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&xOffset), sizeof (float));

    float yOffset = static_cast<float>(params.value("yOffset").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&yOffset), sizeof (float));

    float zOffset = static_cast<float>(params.value("zOffset").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&zOffset), sizeof (float));
}

void DMagicianProtocol::encodeGetEndEffectorParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 60;
    payload.p_ctrl.c_rw = 0;
}

//! [61]
void DMagicianProtocol::encodeSetEndEffectorLaser(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 61;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "enable");
    checkBoolValue(params, "on");

    quint8 enable = params.value("enable").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&enable), sizeof (quint8));

    quint8 on = params.value("on").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&on), sizeof (quint8));
}

void DMagicianProtocol::encodeGetEndEffectorLaser(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 61;
    payload.p_ctrl.c_rw = 0;
}

//! [62]
void DMagicianProtocol::encodeSetEndEffectorSuctionCup(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 62;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "enable");
    checkBoolValue(params, "on");

    quint8 enable = params.value("enable").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&enable), sizeof (quint8));

    quint8 suck = params.value("on").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&suck), sizeof (quint8));
}

void DMagicianProtocol::encodeGetEndEffectorSuctionCup(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 62;
    payload.p_ctrl.c_rw = 0;
}

//! [63]
void DMagicianProtocol::encodeSetEndEffectorGripper(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 63;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "enable");
    checkBoolValue(params, "on");

    quint8 enable = params.value("enable").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&enable), sizeof (quint8));

    quint8 grip = params.value("on").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&grip), sizeof (quint8));
}

void DMagicianProtocol::encodeGetEndEffectorGripper(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 63;
    payload.p_ctrl.c_rw = 0;
}

//! [64]
void DMagicianProtocol::encodeSetEndEffectorType(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 64;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "type");

    quint8 type = static_cast<quint8>(params.value("type").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&type), sizeof (quint8));
}

void DMagicianProtocol::encodeGetEndEffectorType(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 64;
    payload.p_ctrl.c_rw = 0;
}

//! [65]
void DMagicianProtocol::encodeSetServoAngle(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 65;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "index");
    checkFloatValue(params, "value");

    quint8 index = static_cast<quint8>(params.value("index").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&index), sizeof (quint8));

    float angle = static_cast<float>(params.value("value").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&angle), sizeof (float));
}

void DMagicianProtocol::encodeGetServoAngle(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 65;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "index");

    quint8 index = static_cast<quint8>(params.value("index").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&index), sizeof (quint8));
}

//! [66]
void DMagicianProtocol::encodeSetArmSpeedRatio(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 66;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "value");
    checkIntValue(params, "type");

    quint8 type = static_cast<quint8>(params.value("type").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&type), sizeof (quint8));

    quint8 value = static_cast<quint8>(params.value("value").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&value), sizeof (quint8));
}

void DMagicianProtocol::encodeGetArmSpeedRatio(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 66;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "type");

    quint8 type = static_cast<quint8>(params.value("type").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&type), sizeof (quint8));
}

//! [67]
void DMagicianProtocol::encodeSetLSpeedRatio(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 67;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "value");
    checkIntValue(params, "type");

    quint8 type = static_cast<quint8>(params.value("type").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&type), sizeof (quint8));

    quint8 value = static_cast<quint8>(params.value("value").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&value), sizeof (quint8));
}

void DMagicianProtocol::encodeGetLSpeedRatio(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 67;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "type");

    quint8 type = static_cast<quint8>(params.value("type").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&type), sizeof (quint8));
}

//! [68]
void DMagicianProtocol::encodeGetConfigAndStatus(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 68;
    payload.p_ctrl.c_rw = 0;
}

//! [69]
void DMagicianProtocol::encodeGetRTStatus(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 69;
    payload.p_ctrl.c_rw = 0;
}

//! [70]
void DMagicianProtocol::encodeSetJOGJointParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 70;
    payload.p_ctrl.c_rw = 1;

    checkArrayValue(params,"velocity");
    checkArrayValue(params,"acceleration");

    QJsonArray velocityArr = params.value("velocity").toArray();
    QJsonArray accelerationArr = params.value("acceleration").toArray();

    if (velocityArr.count() >= 4 and accelerationArr.count() >= 4) {
        VelocityArrayStruct jogParams;
        for (int i = 0; i < 4; i++) {
            jogParams.velocity[i] = static_cast<float>(velocityArr.at(i).toDouble());
            jogParams.acceleration[i] = static_cast<float>(accelerationArr.at(i).toDouble());
        }
        payload.p_params.append(reinterpret_cast<char *>(&jogParams), sizeof (VelocityArrayStruct));
    } else {
        qDebug() << "invalid params.";
    }
}

void DMagicianProtocol::encodeGetJOGJointParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 70;
    payload.p_ctrl.c_rw = 0;
}

//! [71]
void DMagicianProtocol::encodeSetJOGCoordinateParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 71;
    payload.p_ctrl.c_rw = 1;

    checkArrayValue(params,"velocity");
    checkArrayValue(params,"acceleration");

    QJsonArray velocityArr = params.value("velocity").toArray();
    QJsonArray accelerationArr = params.value("acceleration").toArray();

    if (velocityArr.count() >= 4 and accelerationArr.count() >= 4) {
        VelocityArrayStruct jogParams;
        for (int i = 0; i < 4; i++) {
            jogParams.velocity[i] = static_cast<float>(velocityArr.at(i).toDouble());
            jogParams.acceleration[i] = static_cast<float>(accelerationArr.at(i).toDouble());
        }
        payload.p_params.append(reinterpret_cast<char *>(&jogParams), sizeof (VelocityArrayStruct));
    } else {
        qDebug() << "invalid params.";
    }
}

void DMagicianProtocol::encodeGetJOGCoordinateParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 71;
    payload.p_ctrl.c_rw = 0;
}

//! [72]
void DMagicianProtocol::encodeSetJOGCommonParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 72;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "velocityRatio");
    checkFloatValue(params, "accelerationRatio");

    VelocityRatioStruct jogParams;
    jogParams.velocityRatio = static_cast<float>(params.value("velocityRatio").toDouble());
    jogParams.accelerationRatio = static_cast<float>(params.value("accelerationRatio").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&jogParams), sizeof (VelocityRatioStruct));
}

void DMagicianProtocol::encodeGetJOGCommonParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 72;
    payload.p_ctrl.c_rw = 0;
}

//! [73]
void DMagicianProtocol::encodeSetJOGCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 73;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "isJoint");
    checkIntValue(params, "cmd");

    quint8 isJoint = params.value("isJoint").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&isJoint), sizeof (quint8));

    quint8 cmd = static_cast<quint8>(params.value("cmd").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&cmd), sizeof (quint8));
}

//! [74]
void DMagicianProtocol::encodeSetJOGLParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 74;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "velocity");
    checkFloatValue(params, "acceleration");

    VelocityStruct jogParams;
    jogParams.velocity = static_cast<float>(params.value("velocity").toDouble());
    jogParams.acceleration = static_cast<float>(params.value("acceleration").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&jogParams), sizeof (VelocityStruct));
}

//! [75]
void DMagicianProtocol::encodeSetJOGCmdTime(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 75;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "isJoint");
    checkIntValue(params, "cmd");
    checkFloatValue(params, "time");

    quint8 isJoint = params.value("isJoint").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&isJoint), sizeof (quint8));

    quint8 cmd = static_cast<quint8>(params.value("cmd").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&cmd), sizeof (quint8));

    float time = static_cast<float>(params.value("time").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&time), sizeof (float));
}

void DMagicianProtocol::encodeGetJOGLParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 74;
    payload.p_ctrl.c_rw = 0;
}

//! [80]
void DMagicianProtocol::encodeSetPTPJointParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 80;
    payload.p_ctrl.c_rw = 1;

    checkArrayValue(params,"velocity");
    checkArrayValue(params,"acceleration");

    QJsonArray velocityArr = params.value("velocity").toArray();
    QJsonArray accelerationArr = params.value("acceleration").toArray();

    if (velocityArr.count() >= 4 and accelerationArr.count() >= 4) {
        VelocityArrayStruct ptpJointParams;
        for (int i = 0; i < 4; i++) {
            ptpJointParams.velocity[i] = static_cast<float>(velocityArr.at(i).toDouble());
            ptpJointParams.acceleration[i] = static_cast<float>(accelerationArr.at(i).toDouble());
        }
        payload.p_params.append(reinterpret_cast<char *>(&ptpJointParams), sizeof (VelocityArrayStruct));
    } else {
        qDebug() << "invalid params.";
    }
}

void DMagicianProtocol::encodeGetPTPJointParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 80;
    payload.p_ctrl.c_rw = 0;
}

//! [81]
void DMagicianProtocol::encodeSetPTPCoordinateParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 81;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "rVelocity");
    checkFloatValue(params, "xyzVelocity");
    checkFloatValue(params, "rAcceleration");
    checkFloatValue(params, "xyzAcceleration");

    PTPCoordinateParamsStruct ptpCoordinateParams;
    ptpCoordinateParams.rVelocity = static_cast<float>(params.value("rVelocity").toDouble());
    ptpCoordinateParams.xyzVelocity = static_cast<float>(params.value("xyzVelocity").toDouble());
    ptpCoordinateParams.rAcceleration = static_cast<float>(params.value("rAcceleration").toDouble());
    ptpCoordinateParams.xyzAcceleration = static_cast<float>(params.value("xyzAcceleration").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&ptpCoordinateParams), sizeof (PTPCoordinateParamsStruct));
}

void DMagicianProtocol::encodeGetPTPCoordinateParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 81;
    payload.p_ctrl.c_rw = 0;
}

//! [82]
void DMagicianProtocol::encodeSetPTPJumpParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 82;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "jumpHeight");
    checkFloatValue(params, "zLimit");

    float jumpHeight = static_cast<float>(params.value("jumpHeight").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&jumpHeight), sizeof (float));

    float zLimit = static_cast<float>(params.value("zLimit").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&zLimit), sizeof (float));
}

void DMagicianProtocol::encodeGetPTPJumpParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 82;
    payload.p_ctrl.c_rw = 0;
}

//! [83]
void DMagicianProtocol::encodeSetPTPCommonParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 83;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "velocityRatio");
    checkFloatValue(params, "accelerationRatio");

    VelocityRatioStruct ptpParams;
    ptpParams.velocityRatio = static_cast<float>(params.value("velocityRatio").toDouble());
    ptpParams.accelerationRatio = static_cast<float>(params.value("accelerationRatio").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&ptpParams), sizeof (VelocityRatioStruct));
}

void DMagicianProtocol::encodeGetPTPCommonParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 83;
    payload.p_ctrl.c_rw = 0;
}

//! [84]
void DMagicianProtocol::encodeSetPTPCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 84;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "ptpMode");
    checkFloatValue(params, "x");
    checkFloatValue(params, "y");
    checkFloatValue(params, "z");
    checkFloatValue(params, "r");

    PTPCmdStruct ptpCmd;
    ptpCmd.ptpMode = static_cast<quint8>(params.value("ptpMode").toInt());
    ptpCmd.x = static_cast<float>(params.value("x").toDouble());
    ptpCmd.y = static_cast<float>(params.value("y").toDouble());
    ptpCmd.z = static_cast<float>(params.value("z").toDouble());
    ptpCmd.r = static_cast<float>(params.value("r").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&ptpCmd), sizeof (PTPCmdStruct));
}

//! [85]
void DMagicianProtocol::encodeSetPTPLParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 85;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "velocity");
    checkFloatValue(params, "acceleration");

    VelocityStruct ptpLParams;
    ptpLParams.velocity = static_cast<float>(params.value("velocity").toDouble());
    ptpLParams.acceleration = static_cast<float>(params.value("acceleration").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&ptpLParams), sizeof (VelocityStruct));
}

void DMagicianProtocol::encodeGetPTPLParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 85;
    payload.p_ctrl.c_rw = 0;
}

//! [86]
void DMagicianProtocol::encodeSetPTPWithLCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 86;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "ptpMode");
    checkFloatValue(params, "x");
    checkFloatValue(params, "y");
    checkFloatValue(params, "z");
    checkFloatValue(params, "r");
    checkFloatValue(params, "l");

    PTPWithLCmdStruct ptpWithLParams;
    ptpWithLParams.ptpMode = static_cast<quint8>(params.value("ptpMode").toInt());
    ptpWithLParams.x = static_cast<float>(params.value("x").toDouble());
    ptpWithLParams.y = static_cast<float>(params.value("y").toDouble());
    ptpWithLParams.z = static_cast<float>(params.value("z").toDouble());
    ptpWithLParams.r = static_cast<float>(params.value("r").toDouble());
    ptpWithLParams.l = static_cast<float>(params.value("l").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&ptpWithLParams), sizeof (PTPWithLCmdStruct));
}

//! [87]
void DMagicianProtocol::encodeSetPTPJump2Params(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 87;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "startJumpHeight");
    checkFloatValue(params, "endJumpHeight");
    checkFloatValue(params, "zLimit");

    PTPJump2ParamsStruct ptpParams;
    ptpParams.startJumpHeight = static_cast<float>(params.value("startJumpHeight").toDouble());
    ptpParams.endJumpHeight = static_cast<float>(params.value("endJumpHeight").toDouble());
    ptpParams.zLimit = static_cast<float>(params.value("zLimit").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&ptpParams), sizeof (PTPJump2ParamsStruct));
}

void DMagicianProtocol::encodeGetPTPJump2Params(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 87;
    payload.p_ctrl.c_rw = 0;
}

//! [88]
void DMagicianProtocol::encodeSetPTPPOCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 88;
    payload.p_ctrl.c_rw = 1;

    if (!params.contains("ptpCmd")) {
        qWarning() << "'ptpCmd':missing";
    }
    if (!params.contains("poCmd")) {
        qWarning() << "'poCmd':missing";
    }

    QJsonObject ptpCmdObj = params.value("ptpCmd").toObject();
    QJsonArray poCmdArray = params.value("poCmd").toArray();

    PTPCmdStruct ptpCmd;
    ptpCmd.ptpMode = static_cast<quint8>(ptpCmdObj.value("ptpMode").toInt());
    ptpCmd.x = static_cast<float>(ptpCmdObj.value("x").toDouble());
    ptpCmd.y = static_cast<float>(ptpCmdObj.value("y").toDouble());
    ptpCmd.z = static_cast<float>(ptpCmdObj.value("z").toDouble());
    ptpCmd.r = static_cast<float>(ptpCmdObj.value("r").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&ptpCmd), sizeof (PTPCmdStruct));

    quint8 count = static_cast<quint8>(poCmdArray.count());
    payload.p_params.append(reinterpret_cast<char *>(&count), sizeof (quint8));

    for (int i = 0; i < poCmdArray.count(); i++) {
        QJsonObject poCmdObj = poCmdArray.at(i).toObject();

        POCmdStruct poCmd;
        poCmd.ratio = static_cast<quint8>(poCmdObj.value("ratio").toInt());
        poCmd.port = static_cast<quint16>(poCmdObj.value("port").toInt());
        poCmd.level = static_cast<quint8>(poCmdObj.value("level").toInt());

        payload.p_params.append(reinterpret_cast<char *>(&poCmd), sizeof (POCmdStruct));
    }
}

//! [89]
void DMagicianProtocol::encodeSetPTPPOWithLCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 89;
    payload.p_ctrl.c_rw = 1;

    if (!params.contains("ptpCmd")) {
        qWarning() << "'ptpCmd':missing";
    }
    if (!params.contains("poCmd")) {
        qWarning() << "'poCmd':missing";
    }

    QJsonObject ptpCmdObj = params.value("ptpCmd").toObject();
    QJsonArray poCmdArray = params.value("poCmd").toArray();

    PTPWithLCmdStruct ptpCmd;
    ptpCmd.ptpMode = static_cast<quint8>(ptpCmdObj.value("ptpMode").toInt());
    ptpCmd.x = static_cast<float>(ptpCmdObj.value("x").toDouble());
    ptpCmd.y = static_cast<float>(ptpCmdObj.value("y").toDouble());
    ptpCmd.z = static_cast<float>(ptpCmdObj.value("z").toDouble());
    ptpCmd.l = static_cast<float>(ptpCmdObj.value("l").toDouble());
    ptpCmd.r = static_cast<float>(ptpCmdObj.value("r").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&ptpCmd), sizeof (PTPCmdStruct));

    quint8 count = static_cast<quint8>(poCmdArray.count());
    payload.p_params.append(reinterpret_cast<char *>(&count), sizeof (quint8));

    for (int i = 0; i < count; i++) {
        QJsonObject poCmdObj = poCmdArray.at(i).toObject();

        POCmdStruct poCmd;
        poCmd.ratio = static_cast<quint8>(poCmdObj.value("ratio").toInt());
        poCmd.port = static_cast<quint16>(poCmdObj.value("port").toInt());
        poCmd.level = static_cast<quint8>(poCmdObj.value("level").toInt());

        payload.p_params.append(reinterpret_cast<char *>(&poCmd), sizeof (POCmdStruct));
    }
}

//! [90]
void DMagicianProtocol::encodeSetCPParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 90;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "targetAcc");
    checkFloatValue(params, "junctionVel");
    checkBoolValue(params, "isRealTimeTrack");

    CPParamsStruct cpParams;
    cpParams.targetAcc = static_cast<float>(params.value("targetAcc").toDouble());
    cpParams.junctionVel = static_cast<float>(params.value("junctionVel").toDouble());

    bool isRealTimeTrack = params.value("isRealTimeTrack").toBool();
    if (isRealTimeTrack) {
        cpParams.isRealTimeTrack = 1;
        cpParams.period = static_cast<float>(params.value("period").toDouble());
    } else {
        cpParams.isRealTimeTrack = 0;
        cpParams.acc = static_cast<float>(params.value("acc").toDouble());
    }

    payload.p_params.append(reinterpret_cast<char *>(&cpParams), sizeof (CPParamsStruct));
}

void DMagicianProtocol::encodeGetCPParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 90;
    payload.p_ctrl.c_rw = 0;
}

//! [91]
void DMagicianProtocol::encodeSetCPCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 91;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "cpMode");
    checkFloatValue(params, "x");
    checkFloatValue(params, "y");
    checkFloatValue(params, "z");
    checkFloatValue(params, "power");

    CPCmdStruct cpCmd;
    cpCmd.cpMode = static_cast<quint8>(params.value("cpMode").toInt());
    cpCmd.x = static_cast<float>(params.value("x").toDouble());
    cpCmd.y = static_cast<float>(params.value("y").toDouble());
    cpCmd.z = static_cast<float>(params.value("z").toDouble());
    cpCmd.power = static_cast<float>(params.value("power").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&cpCmd), sizeof (CPCmdStruct));
}

//! [92]
void DMagicianProtocol::encodeSetCPLECmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 92;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "cpMode");
    checkFloatValue(params, "x");
    checkFloatValue(params, "y");
    checkFloatValue(params, "z");
    checkFloatValue(params, "power");

    CPCmdStruct cpCmd;
    cpCmd.cpMode = static_cast<quint8>(params.value("cpMode").toInt());
    cpCmd.x = static_cast<float>(params.value("x").toDouble());
    cpCmd.y = static_cast<float>(params.value("y").toDouble());
    cpCmd.z = static_cast<float>(params.value("z").toDouble());
    cpCmd.power = static_cast<float>(params.value("power").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&cpCmd), sizeof (CPCmdStruct));
}

//! [93]
void DMagicianProtocol::encodeSetCPRHeadHoldEnable(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 93;
    payload.p_ctrl.c_rw = 1;
}

void DMagicianProtocol::encodeGetCPRHeadHoldEnable(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 93;
    payload.p_ctrl.c_rw = 0;
}

//! [94]
void DMagicianProtocol::encodeSetCPCommonParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 94;
    payload.p_ctrl.c_rw = 1;
}

void DMagicianProtocol::encodeGetCPCommonParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 94;
    payload.p_ctrl.c_rw = 0;
}

//! [95]
void DMagicianProtocol::encodeSetCP2Cmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 95;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "cpMode");
    checkFloatValue(params, "x");
    checkFloatValue(params, "y");
    checkFloatValue(params, "z");

    CP2CmdStruct cp2cmd;
    cp2cmd.cpMode = static_cast<quint8>(params.value("cpMode").toInt());
    cp2cmd.x = static_cast<float>(params.value("x").toDouble());
    cp2cmd.y = static_cast<float>(params.value("y").toDouble());
    cp2cmd.z = static_cast<float>(params.value("z").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&cp2cmd), sizeof (CP2CmdStruct));
}

//! [100]
void DMagicianProtocol::encodeSetARCParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 100;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "xyzVelocity");
    checkFloatValue(params, "rVelocity");
    checkFloatValue(params, "xyzAcceleration");
    checkFloatValue(params, "rAcceleration");

    ARCParamsStruct arcParams;
    arcParams.xyzVelocity = static_cast<float>(params.value("xyzVelocity").toDouble());
    arcParams.rVelocity = static_cast<float>(params.value("rVelocity").toDouble());
    arcParams.xyzAcceleration = static_cast<float>(params.value("xyzAcceleration").toDouble());
    arcParams.rAcceleration = static_cast<float>(params.value("rAcceleration").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&arcParams), sizeof (ARCParamsStruct));
}

void DMagicianProtocol::encodeGetARCParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 100;
    payload.p_ctrl.c_rw = 0;
}

//! [101]
void DMagicianProtocol::encodeSetARCCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 101;
    payload.p_ctrl.c_rw = 1;

    if (!params.contains("cirPoint")) {
        qWarning() << "'cirPoint':missing";
    }
    if (!params.contains("toPoint")) {
        qWarning() << "'toPoint':missing";
    }

    ARCCmdStruct arcCmd;

    QJsonObject cirPointObj = params.value("cirPoint").toObject();
    arcCmd.cirPoint.x = static_cast<float>(cirPointObj.value("x").toDouble());
    arcCmd.cirPoint.y = static_cast<float>(cirPointObj.value("y").toDouble());
    arcCmd.cirPoint.z = static_cast<float>(cirPointObj.value("z").toDouble());
    arcCmd.cirPoint.r = static_cast<float>(cirPointObj.value("r").toDouble());

    QJsonObject toPointObj = params.value("toPoint").toObject();
    arcCmd.toPoint.x = static_cast<float>(toPointObj.value("x").toDouble());
    arcCmd.toPoint.y = static_cast<float>(toPointObj.value("y").toDouble());
    arcCmd.toPoint.z = static_cast<float>(toPointObj.value("z").toDouble());
    arcCmd.toPoint.r = static_cast<float>(toPointObj.value("r").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&arcCmd), sizeof (ARCCmdStruct));
}

//! [102]
void DMagicianProtocol::encodeSetCircleCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 102;
    payload.p_ctrl.c_rw = 1;

    if (!params.contains("cirPoint")) {
        qWarning() << "'cirPoint':missing, use ({x:0.0, y:0.0, z:0.0, r:0.0})";
    } else if (!params.value("cirPoint").isObject()) {
        qWarning() << "'cirPoint':type error";
    }
    if (!params.contains("toPoint")) {
        qWarning() << "'toPoint':missing, use ({x:0.0, y:0.0, z:0.0, r:0.0})";
    } else if (!params.value("toPoint").isObject()) {
        qWarning() << "'toPoint':type error";
    }

    checkIntValue(params, "count");

    CircleCmdStruct circleCmd;

    QJsonObject cirPointObj = params.value("cirPoint").toObject();
    circleCmd.cirPoint.x = static_cast<float>(cirPointObj.value("x").toDouble());
    circleCmd.cirPoint.y = static_cast<float>(cirPointObj.value("y").toDouble());
    circleCmd.cirPoint.z = static_cast<float>(cirPointObj.value("z").toDouble());
    circleCmd.cirPoint.r = static_cast<float>(cirPointObj.value("r").toDouble());

    QJsonObject toPointObj = params.value("toPoint").toObject();
    circleCmd.toPoint.x = static_cast<float>(toPointObj.value("x").toDouble());
    circleCmd.toPoint.y = static_cast<float>(toPointObj.value("y").toDouble());
    circleCmd.toPoint.z = static_cast<float>(toPointObj.value("z").toDouble());
    circleCmd.toPoint.r = static_cast<float>(toPointObj.value("r").toDouble());

    circleCmd.count = static_cast<quint32>(params.value("count").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&circleCmd), sizeof (CircleCmdStruct));
}

//! [103]
void DMagicianProtocol::encodeSetARCCommonParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 103;
    payload.p_ctrl.c_rw = 1;
}

void DMagicianProtocol::encodeGetARCCommonParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 103;
    payload.p_ctrl.c_rw = 0;
}

//! [110]
void DMagicianProtocol::encodeSetWAITCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 110;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "delay");

    quint32 delayMs = static_cast<quint32>(params.value("delay").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&delayMs), sizeof (quint32));
}

//! [120]
void DMagicianProtocol::encodeSetTRIGCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 120;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "port");
    checkIntValue(params, "mode");
    checkIntValue(params, "condition");
    checkIntValue(params, "threshold");

    TRIGCmdStruct triggerCmd;
    triggerCmd.port = static_cast<quint8>(params.value("port").toInt());
    triggerCmd.mode = static_cast<quint8>(params.value("mode").toInt());
    triggerCmd.condition = static_cast<quint8>(params.value("condition").toInt());
    triggerCmd.threshold = static_cast<quint16>(params.value("threshold").toInt());

    payload.p_params.append(reinterpret_cast<char *>(&triggerCmd), sizeof (TRIGCmdStruct));
}

//! [130]
void DMagicianProtocol::encodeSetIOMultiplexing(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 130;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "port");
    checkIntValue(params, "multiplex");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint8 level = static_cast<quint8>(params.value("multiplex").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&level), sizeof (quint8));
}

void DMagicianProtocol::encodeGetIOMultiplexing(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 130;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [113]
void DMagicianProtocol::encodeSetIODO(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 131;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "port");
    checkIntValue(params, "level");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint8 level = static_cast<quint8>(params.value("level").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&level), sizeof (quint8));
}

void DMagicianProtocol::encodeGetIODO(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 131;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [132]
void DMagicianProtocol::encodeSetIOPWM(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 132;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "port");
    checkFloatValue(params, "frequency");
    checkFloatValue(params, "dutyCycle");

    IOPWMStruct ioPWM;
    ioPWM.port = static_cast<quint8>(params.value("port").toInt());
    ioPWM.frequency = static_cast<float>(params.value("frequency").toDouble());
    ioPWM.dutyCycle = static_cast<float>(params.value("dutyCycle").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&ioPWM), sizeof (IOPWMStruct));
}

void DMagicianProtocol::encodeGetIOPWM(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 132;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [133]
void DMagicianProtocol::encodeGetIODI(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 133;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [134]
void DMagicianProtocol::encodeGetIOADC(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 134;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [135]
void DMagicianProtocol::encodeSetEMotor(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 135;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "index");
    checkBoolValue(params, "enable");
    checkIntValue(params, "speed");

    EMotorStruct eMotor;
    eMotor.index = static_cast<quint8>(params.value("index").toInt());
    eMotor.isEnabled = params.value("enable").toBool() ? 1 : 0;
    eMotor.speed = params.value("speed").toInt();

    payload.p_params.append(reinterpret_cast<char *>(&eMotor), sizeof (EMotorStruct));
}

//! [136]
void DMagicianProtocol::encodeSetEMotorS(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 136;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "index");
    checkBoolValue(params, "enable");
    checkIntValue(params, "speed");
    checkIntValue(params, "distance");

    EMotorSStruct eMotors;
    eMotors.index = static_cast<quint8>(params.value("index").toInt());
    eMotors.isEnabled = params.value("enable").toBool() ? 1 : 0;
    eMotors.speed = params.value("speed").toInt();
    eMotors.distance = static_cast<quint32>(params.value("distance").toInt());

    payload.p_params.append(reinterpret_cast<char *>(&eMotors), sizeof (EMotorSStruct));
}

//! [137]
void DMagicianProtocol::encodeSetColorSensor(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 137;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "enable");
    checkIntValue(params, "port");
    checkIntValue(params, "version");

    quint8 enable = params.value("enable").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&enable), sizeof (quint8));

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint8 version = static_cast<quint8>(params.value("version").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&version), sizeof (quint8));
}

void DMagicianProtocol::encodeGetColorSensor(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 137;
    payload.p_ctrl.c_rw = 0;
}

//! [138]
void DMagicianProtocol::encodeSetInfraredSensor(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 138;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "enable");
    checkIntValue(params, "port");
    checkIntValue(params, "version");

    quint8 enable = params.value("enable").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&enable), sizeof (quint8));

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint8 version = static_cast<quint8>(params.value("version").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&version), sizeof (quint8));
}

void DMagicianProtocol::encodeGetInfraredSensor(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 138;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [139]
void DMagicianProtocol::encodeSetIICInfo(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 139;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "mode");
    checkIntValue(params, "mainIndex");

    quint8 mode = static_cast<quint8>(params.value("mode").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&mode), sizeof (quint8));

    quint8 mainIndex = static_cast<quint8>(params.value("mainIndex").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&mainIndex), sizeof (quint8));
}

void DMagicianProtocol::encodeGetIICInfo(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 139;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "mode");
    checkIntValue(params, "mainIndex");
    checkIntValue(params, "subIndex");

    quint8 mode = static_cast<quint8>(params.value("mode").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&mode), sizeof (quint8));

    quint8 mainIndex = static_cast<quint8>(params.value("mainIndex").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&mainIndex), sizeof (quint8));

    quint8 subIndex = static_cast<quint8>(params.value("subIndex").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&subIndex), sizeof (quint8));
}

//! [140]
void DMagicianProtocol::encodeSetAngleSensorStaticError(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 140;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "rearArmAngleError");
    checkFloatValue(params, "frontArmAngleError");

    float rearArmAngleError = static_cast<float>(params.value("rearArmAngleError").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&rearArmAngleError), sizeof (float));

    float frontArmAngleError = static_cast<float>(params.value("frontArmAngleError").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&frontArmAngleError), sizeof (float));
}

void DMagicianProtocol::encodeGetAngleSensorStaticError(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 140;
    payload.p_ctrl.c_rw = 0;
}

//! [141]
void DMagicianProtocol::encodeSetAngleSensorCoef(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 141;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "rearArmAngle");
    checkFloatValue(params, "frontArmAngle");

    float rearArmAngle = static_cast<float>(params.value("rearArmAngle").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&rearArmAngle), sizeof (float));

    float frontArmAngle = static_cast<float>(params.value("frontArmAngle").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&frontArmAngle), sizeof (float));
}

void DMagicianProtocol::encodeGetAngleSensorCoef(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 141;
    payload.p_ctrl.c_rw = 0;
}

//! [142]
void DMagicianProtocol::encodeSetBaseDecoderStaticError(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 142;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "baseDecoderError");

    float baseDecoderError = static_cast<float>(params.value("baseDecoderError").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&baseDecoderError), sizeof (float));
}

void DMagicianProtocol::encodeGetBaseDecoderStaticError(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 142;
    payload.p_ctrl.c_rw = 0;
}

//! [143]
void DMagicianProtocol::encodeSetLRHandCalibrateValue(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 143;
    payload.p_ctrl.c_rw = 1;
}

void DMagicianProtocol::encodeGetLRHandCalibrateValue(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 143;
    payload.p_ctrl.c_rw = 0;
}

//! [150]
void DMagicianProtocol::encodeSetWIFIConfigMode(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 150;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "enable");

    quint8 enable = params.value("enable").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&enable), sizeof (quint8));
}

void DMagicianProtocol::encodeGetWIFIConfigMode(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 150;
    payload.p_ctrl.c_rw = 0;
}

//TODO
//! [151]
void DMagicianProtocol::encodeSetWIFISSID(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 151;
    payload.p_ctrl.c_rw = 1;

    checkStringValue(params, "ssid");

    QString str = params.value("ssid").toString();
    QByteArray arr = str.toUtf8();
    arr.append('\0');
    payload.p_params.append(arr);
}

void DMagicianProtocol::encodeGetWIFISSID(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 151;
    payload.p_ctrl.c_rw = 0;
}

//! [152]
void DMagicianProtocol::encodeSetWIFIPassword(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 152;
    payload.p_ctrl.c_rw = 1;

    checkStringValue(params, "password");

    QString str = params.value("password").toString();
    QByteArray arr = str.toUtf8();
    arr.append('\0');
    payload.p_params.append(arr);
}

void DMagicianProtocol::encodeGetWIFIPassword(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 152;
    payload.p_ctrl.c_rw = 0;
}

//! [153]
void DMagicianProtocol::encodeSetWIFIIPAddress(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 153;
    payload.p_ctrl.c_rw = 1;

    checkArrayValue(params, "address");

    WIFIIPAddressStruct wifiIpAddress;
    QJsonArray addressArray = params.value("address").toArray();
    for (int i = 0; i < addressArray.size(); ++i) {
        wifiIpAddress.address[i] = static_cast<quint8>(addressArray.at(i).toInt());
    }
    wifiIpAddress.dhcp = static_cast<quint8>(params.value("dhcp").toInt());

    payload.p_params.append(reinterpret_cast<char *>(&wifiIpAddress), sizeof (WIFIIPAddressStruct));
}

void DMagicianProtocol::encodeGetWIFIIPAddress(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 153;
    payload.p_ctrl.c_rw = 0;
}

//! [154]
void DMagicianProtocol::encodeSetWIFINetmask(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 154;
    payload.p_ctrl.c_rw = 1;

    checkArrayValue(params, "address");

    WifiAddressStruct wifiNetMask;
    QJsonArray addressArray = params.value("address").toArray();
    for (int i = 0; i < addressArray.size(); ++i) {
        wifiNetMask.address[i] = static_cast<quint8>(addressArray.at(i).toInt());
    }

    payload.p_params.append(reinterpret_cast<char *>(&wifiNetMask), sizeof (WifiAddressStruct));
}

void DMagicianProtocol::encodeGetWIFINetmask(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 154;
    payload.p_ctrl.c_rw = 0;
}

//! [155]
void DMagicianProtocol::encodeSetWIFIGateway(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 155;
    payload.p_ctrl.c_rw = 1;

    checkArrayValue(params, "address");

    WifiAddressStruct wifiGateWay;
    QJsonArray addressArray = params.value("address").toArray();
    for (int i = 0; i < addressArray.size(); ++i) {
        wifiGateWay.address[i] = static_cast<quint8>(addressArray.at(i).toInt());
    }

    payload.p_params.append(reinterpret_cast<char *>(&wifiGateWay), sizeof (WifiAddressStruct));
}

void DMagicianProtocol::encodeGetWIFIGateway(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 155;
    payload.p_ctrl.c_rw = 0;
}

//! [156]
void DMagicianProtocol::encodeSetWIFIDNS(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 156;
    payload.p_ctrl.c_rw = 1;

    checkArrayValue(params, "address");

    WifiAddressStruct wifiDns;
    QJsonArray addressArray = params.value("address").toArray();
    for (int i = 0; i < addressArray.size(); ++i) {
        wifiDns.address[i] = static_cast<quint8>(addressArray.at(i).toInt());
    }

    payload.p_params.append(reinterpret_cast<char *>(&wifiDns), sizeof (WifiAddressStruct));
}

void DMagicianProtocol::encodeGetWIFIDNS(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 156;
    payload.p_ctrl.c_rw = 0;
}

//! [157]
void DMagicianProtocol::encodeGetWIFIConnectStatus(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 157;
    payload.p_ctrl.c_rw = 0;
}

//! [170]
void DMagicianProtocol::encodeSetLostStepValue(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 170;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "value");

    float value = static_cast<float>(params.value("value").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&value), sizeof (float));
}

//! [171]
void DMagicianProtocol::encodeSetLostStepCmd(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 171;
    payload.p_ctrl.c_rw = 1;
}

//! [172]
void DMagicianProtocol::encodeSetCollisionCheck(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 172;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "enable");
    checkFloatValue(params, "threshold");

    quint8 on = params.value("enable").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&on), sizeof (quint8));

    float threshold = static_cast<float>(params.value("threshold").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&threshold), sizeof (float));
}

void DMagicianProtocol::encodeGetCollisionCheck(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 172;
    payload.p_ctrl.c_rw = 0;
}

//! [192]
void DMagicianProtocol::encodeGetDeviceStatus(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 192;
    payload.p_ctrl.c_rw = 0;
}

//! [193]
void DMagicianProtocol::encodeSetDeviceRunAPP(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 193;
    payload.p_ctrl.c_rw = 0;
}

//! [194]
void DMagicianProtocol::encodeSetUpgradeStatus(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 194;
    payload.p_ctrl.c_rw = 1;

    checkStringValue(params, "length");
    checkStringValue(params, "md5");

    /* 其中length占4字节，MD5校验码占16字节。 */
    QString length_str = params.value("length").toString();
    QByteArray length_ba;
    length_ba.append(length_str);
    QByteArray length_hex = QByteArray::fromHex(length_ba);
    if (length_hex.size() != 4) {
        return;
    }

    payload.p_params.append(reinterpret_cast<char *>(length_hex.data()), 4);

    QString md5_str = params.value("md5").toString();
    QByteArray md5_ba;
    md5_ba.append(md5_str);
    QByteArray md5_hex = QByteArray::fromHex(md5_ba);
    if (md5_hex.size() != 16) {
        return;
    }
    payload.p_params.append(reinterpret_cast<char *>(md5_hex.data()), 16);
}

void DMagicianProtocol::encodeGetUpgradeStatus(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 194;
    payload.p_ctrl.c_rw = 0;

    checkStringValue(params, "length");
    checkStringValue(params, "md5");

    /* 其中length占4字节，MD5校验码占16字节。 */
    QString length_str = params.value("length").toString();
    QByteArray length_ba;
    length_ba.append(length_str);
    QByteArray length_hex = QByteArray::fromHex(length_ba);
    if (length_hex.size() != 4) {
        return;
    }

    payload.p_params.append(reinterpret_cast<char *>(length_hex.data()), 4);

    QString md5_str = params.value("md5").toString();
    QByteArray md5_ba;
    md5_ba.append(md5_str);
    QByteArray md5_hex = QByteArray::fromHex(md5_ba);
    if (md5_hex.size() != 16) {
        return;
    }
    payload.p_params.append(reinterpret_cast<char *>(md5_hex.data()), 16);
}

//! [195]
void DMagicianProtocol::encodeSetRunUpgrade(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 195;
    payload.p_ctrl.c_rw = 0;
}

//! [196]
void DMagicianProtocol::encodeSetStartTransmit(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 196;
    payload.p_ctrl.c_rw = 0;
}

//! [197]
void DMagicianProtocol::encodeSetUpgradeVeify(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 197;
    payload.p_ctrl.c_rw = 0;
}

//! [198]
void DMagicianProtocol::encodeSetDeviceRestart(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 198;
    payload.p_ctrl.c_rw = 1;
}

//! [200]
void DMagicianProtocol::encodeGetColorObjExist(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 200;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    checkIntValue(params, "color");

    quint8 color = static_cast<quint8>(params.value("color").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&color), sizeof(quint8));
}

//! [200]
void DMagicianProtocol::encodeGetColorObjCoordinate(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 200;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    checkIntValue(params, "color");

    quint8 color = static_cast<quint8>(params.value("color").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&color), sizeof(quint8));

    checkIntValue(params, "coordinate");

    quint8 coordinate = static_cast<quint8>(params.value("coordinate").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&coordinate), sizeof (quint8));
}

//! [210]
void DMagicianProtocol::encodeGetSeeedDistanceSensor(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 210;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [211]
void DMagicianProtocol::encodeSetSeeedTempSensor(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 211;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

void DMagicianProtocol::encodeGetSeeedTempSensor(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 211;
    payload.p_ctrl.c_rw = 0;
}

//! [212]
void DMagicianProtocol::encodeSetSeeedLightSensor(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 212;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

void DMagicianProtocol::encodeGetSeeedLightSensor(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 212;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "mode");

    quint8 mode = static_cast<quint8>(params.value("mode").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&mode), sizeof (quint8));
}

//! [213]
void DMagicianProtocol::encodeSetSeeedColorSensor(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 213;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

void DMagicianProtocol::encodeGetSeeedColorSensor(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 213;
    payload.p_ctrl.c_rw = 0;
}

//! [214]
void DMagicianProtocol::encodeSetSeeedRGBLed(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 214;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "port");
    checkFloatValue(params, "rgbValue");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    float rgb = static_cast<float>(params.value("rgbValue").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&rgb), sizeof (float));
}

//! [215]
void DMagicianProtocol::encodeGetMCUTemperature(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 215;
    payload.p_ctrl.c_rw = 0;
}

//! [240]
void DMagicianProtocol::encodeSetQueuedCmdStart(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 240;
    payload.p_ctrl.c_rw = 1;
}

//! [241]
void DMagicianProtocol::encodeSetQueuedCmdStop(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 241;
    payload.p_ctrl.c_rw = 1;
}

//! [242]
void DMagicianProtocol::encodeSetQueuedCmdForceStop(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 242;
    payload.p_ctrl.c_rw = 1;
}

//! [243]
void DMagicianProtocol::encodeSetQueuedCmdStartDownload(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 243;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "totalLoop");
    checkIntValue(params, "linePerLoop");

    quint8 totalLoop = static_cast<quint8>(params.value("totalLoop").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&totalLoop), sizeof (quint8));

    quint8 linePerLoop = static_cast<quint8>(params.value("linePerLoop").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&linePerLoop), sizeof (quint8));
}

//! [244]
void DMagicianProtocol::encodeSetQueuedCmdStopDownload(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 244;
    payload.p_ctrl.c_rw = 1;
}

//! [245]
void DMagicianProtocol::encodeSetQueuedCmdClear(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 245;
    payload.p_ctrl.c_rw = 1;
}

//! [246]
void DMagicianProtocol::encodeGetQueuedCmdCurrentIndex(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 246;
    payload.p_ctrl.c_rw = 0;
}

//! [247]
void DMagicianProtocol::encodeGetQueuedCmdLeftSpace(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 247;
    payload.p_ctrl.c_rw = 0;
}

//! [216-1]
void DMagicianProtocol::encodeSensorRGBLEDInit(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 1;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-2]
void DMagicianProtocol::encodeSensorSetRGBLEDVlaue(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 2;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");
    checkIntValue(params, "index");
    checkIntValue(params, "red");
    checkIntValue(params, "green");
    checkIntValue(params, "blue");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint8 index = static_cast<quint8>(params.value("index").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&index), sizeof (quint8));

    quint8 red = static_cast<quint8>(params.value("red").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&red), sizeof (quint8));

    quint8 green = static_cast<quint8>(params.value("green").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&green), sizeof (quint8));

    quint8 blue = static_cast<quint8>(params.value("blue").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&blue), sizeof (quint8));
}

//! [216-3]
void DMagicianProtocol::encodeSensorRGBLEDTurnOFFAll(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 3;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-4]
void DMagicianProtocol::encodeSensorSetRGBLEDState(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 4;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");
    checkIntValue(params, "index");
    checkIntValue(params, "on");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint8 index = static_cast<quint8>(params.value("index").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&index), sizeof (quint8));

    quint8 on = static_cast<quint8>(params.value("on").toBool() ? 1 : 0);
    payload.p_params.append(reinterpret_cast<char *>(&on), sizeof (quint8));
}

//! [216-10]
void DMagicianProtocol::encodeSensorOledInit(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 10;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-11]
void DMagicianProtocol::encodeSensorOledClear(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 11;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-12]
void DMagicianProtocol::encodeSensorOledDisplay(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 12;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");
    checkIntValue(params, "x");
    checkIntValue(params, "y");
    checkStringValue(params, "text");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint8 x = static_cast<quint8>(params.value("x").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&x), sizeof (quint8));

    quint8 y = static_cast<quint8>(params.value("y").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&y), sizeof (quint8));

    QString text = params.value("text").toString();
    payload.p_params.append(text.toUtf8() + '\0');
}

//! [216-20]
void DMagicianProtocol::encodeSensorKnobInit(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 20;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-21]
void DMagicianProtocol::encodeSensorGetKnob(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 21;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-22]
void DMagicianProtocol::encodeSensorVoiceInit(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 22;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-23]
void DMagicianProtocol::encodeSensorGetVoice(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 23;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-24]
void DMagicianProtocol::encodeSensorLightInit(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 24;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-25]
void DMagicianProtocol::encodeSensorGetLight(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 25;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-26]
void DMagicianProtocol::encodeSensorPEInit(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 26;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");
    checkIntValue(params, "version");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint8 version = static_cast<quint8>(params.value("version").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&version), sizeof (quint8));
}

//! [216-27]
void DMagicianProtocol::encodeSensorGetPEState(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 27;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint8 version = static_cast<quint8>(params.value("version").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&version), sizeof (quint8));
}

//! [216-28]
void DMagicianProtocol::encodeSensorUltrasonicInit(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 28;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-29]
void DMagicianProtocol::encodeSensorUltrasonicGet(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 29;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-30]
void DMagicianProtocol::encodeSensorSHT31Init(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 30;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-31]
void DMagicianProtocol::encodeSensorGetSHT31(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 31;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-32]
void DMagicianProtocol::encodeSensorColorInit(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 32;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-33]
void DMagicianProtocol::encodeSensorGetColor(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 33;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");
    checkIntValue(params, "index");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint8 index = static_cast<quint8>(params.value("index").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&index), sizeof (quint8));
}

//! [216-34]
void DMagicianProtocol::encodeSensorGetColorRes(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 34;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-40]
void DMagicianProtocol::encodeSensorXBeeInit(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 40;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");
    checkIntValue(params, "baud");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint32 baud = static_cast<quint32>(params.value("baud").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&baud), sizeof (quint32));
}

//! [216-41]
void DMagicianProtocol::encodeSensorXBeeSend(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 41;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");
    checkStringValue(params, "text");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    QString text = params.value("text").toString();
    payload.p_params.append(text.toUtf8() + '\0');
}

//! [216-42]
void DMagicianProtocol::encodeSensorXBeeReceive(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 42;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-43]
void DMagicianProtocol::encodeSensorXBeeClear(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 43;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-50]
void DMagicianProtocol::encodeSensorSYNInit(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 50;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");
    checkIntValue(params, "baud");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint32 baud = static_cast<quint32>(params.value("baud").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&baud), sizeof (quint32));
}

//! [216-51]
void DMagicianProtocol::encodeSensorSetSYN(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 51;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");
    checkIntValue(params, "sound");
    checkIntValue(params, "speed");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint8 sound = static_cast<quint8>(params.value("sound").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&sound), sizeof (quint8));

    quint8 speed = static_cast<quint8>(params.value("speed").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&speed), sizeof (quint8));
}

//! [216-52]
void DMagicianProtocol::encodeSensorSetSYNMusic(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 52;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");
    checkIntValue(params, "type");
    checkIntValue(params, "index");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint8 type = static_cast<quint8>(params.value("type").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&type), sizeof (quint8));

    quint8 index = static_cast<quint8>(params.value("index").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&index), sizeof (quint8));
}

//! [216-53]
void DMagicianProtocol::encodeSensorSendSYN(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 53;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");
    checkStringValue(params, "text");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    /* string -> unicode */
    QString text = params.value("text").toString();
    for (int i = 0; i < text.size(); ++i) {
        quint16 c = text.at(i).unicode();
        QByteArray num;
        num[0] = static_cast<char>(c >> 8);
        num[1] = static_cast<char>(c);
        payload.p_params.append(num);
    }
}

//! [216-54]
void DMagicianProtocol::encodeSensorSetSYNCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 54;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "port");
    checkIntValue(params, "cmd");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    quint8 cmd = static_cast<quint8>(params.value("cmd").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&cmd), sizeof (quint8));
}

//! [216-60]
void DMagicianProtocol::encodeSetBleInf(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 60;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "mode");
    checkStringValue(params, "name");

    quint8 mode = static_cast<quint8>(params.value("mode").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&mode), sizeof (quint8));

    QString name = params.value("name").toString();
    payload.p_params.append(name.toUtf8() + '\0');
}

//! [216-60]
void DMagicianProtocol::encodeGetBleInf(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 60;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
}

//! [216-61]
void DMagicianProtocol::encodeBleConnect(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 61;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "time");
    checkStringValue(params, "name");

    quint8 time = static_cast<quint8>(params.value("time").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&time), sizeof (quint8));

    QString name = params.value("name").toString();
    payload.p_params.append(name.toUtf8() + '\0');
}

//! [216-63]
void DMagicianProtocol::encodeBleReadOneData(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 63;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
}

//! [216-64]
void DMagicianProtocol::encodeBleWriteOneData(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 64;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkStringValue(params, "data");

    QString data = params.value("data").toString();
    payload.p_params.append(data.toUtf8() + '\0');
}

//! [216-65]
void DMagicianProtocol::encodeSetBleMesh(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 65;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkStringValue(params, "groupID");
    checkIntValue(params, "devID");

    char *c = params.value("groupID").toString().toLatin1().data();
    for (quint64 i = 0; i < strlen(c); i++) {
        char id = c[i];
        quint8 t_groupId = static_cast<quint8>(id-'0');
        payload.p_params.append(reinterpret_cast<char *>(&t_groupId), sizeof (quint8));
    }

    quint8 devID = static_cast<quint8>(params.value("devID").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&devID), sizeof (quint8));
}

//! [216-66]
void DMagicianProtocol::encodeBleReadMeshData(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 66;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
}

//! [216-67]
void DMagicianProtocol::encodeBleWriteMeshData(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 67;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "devID");
    checkStringValue(params, "data");

    quint8 devID = static_cast<quint8>(params.value("devID").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&devID), sizeof (quint8));

    QString data = params.value("data").toString();

    payload.p_params.append(static_cast<char>(strlen(data.toUtf8() + '\0')));
    payload.p_params.append(data.toUtf8() + '\0');

}

//! [216-68]
void DMagicianProtocol::encodeBleClearMeshData(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 68;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

}

//! [216-70]
void DMagicianProtocol::encodeSetSensorHandModel(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 70;
    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-71]
void DMagicianProtocol::encodeGetSensorHandModel(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 71;
    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-72]
void DMagicianProtocol::encodeIsSensorHandModel(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 72;
    quint8 port = static_cast<quint8>(params.value("port").toInt());
    quint8 data = static_cast<quint8>(params.value("data").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&data), sizeof (quint8));
}

//! [216-73]
void DMagicianProtocol::encodeIsJoystickButton(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 73;
    quint8 port = static_cast<quint8>(params.value("port").toInt());
    quint8 index = static_cast<quint8>(params.value("index").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&index), sizeof (quint8));
}

//! [216-74]
void DMagicianProtocol::encodeGetJoystickPos(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 74;
    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-75]
void DMagicianProtocol::encodeGetJoystickButton(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 75;
    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-76]
void DMagicianProtocol::encodeIsPirDetected(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 76;
    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [216-77]
void DMagicianProtocol::encodeGetButtonStatus(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 77;
    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    return ;
}

//! [216-78]
void DMagicianProtocol::encodeGetRedButtonStatus(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 78;
    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    return ;
}

//! [216-79]
void DMagicianProtocol::encodeGetBlueButtonStatus(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 79;
    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));

    return ;
}

void DMagicianProtocol::encodeBleSetMaster(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 90;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkStringValue(params, "mac"); //12位的mac地址
    QByteArray data = params.value("mac").toString().toUtf8();
    if (data.length()>12)
    {
        data = data.mid(0,12);
    }
    else if (data.length()<12)
    {
        data.append(12-data.length(), '\0');
    }
    else
    {
        qDebug()<<"***********encodeBleSetMaster:the mac data length is not 12*******************";
    }
    payload.p_params.append(data);
}

void DMagicianProtocol::encodeBleSetSlave(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 91;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
}

void DMagicianProtocol::encodeBleRecvData(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 92;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
}

void DMagicianProtocol::encodeBleSendData(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 93;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkStringValue(params, "data");
    QByteArray data = params.value("data").toString().toUtf8();
    data.append('\0');
    int dataLen = data.size();
    if (dataLen>255)
    {
        qDebug()<<"********encodeBleWriteData:the data length is rather then 255*******************";
    }
    else
    {
        data = data.mid(0,254);
        data.append('\0');
    }
    payload.p_params.append(static_cast<char>(dataLen));
    payload.p_params.append(data);
}

void DMagicianProtocol::encodeBleClearCacheData(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 94;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
}

void DMagicianProtocol::encodeBleGetMacAddress(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)
    payload.p_cmdID = 216;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 95;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
}

//! [217-10]
void DMagicianProtocol::encodeSetStopPointServer(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 217;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 10;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "PointX");
    checkIntValue(params, "PointY");

    quint16 PointX = static_cast<quint16>(params.value("PointX").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&PointX), sizeof (quint16));

    quint16 PointY = static_cast<quint16>(params.value("PointY").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&PointY), sizeof (quint16));
}

QJsonObject DMagicianProtocol::decodeSetStopPointServer(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetStopPointServer");
    }
    return resObj;
}

//! [217-11]
void DMagicianProtocol::encodeSetStopPointParam(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 217;
    payload.p_ctrl.c_rw = 1;

    quint8 subid = 11;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkIntValue(params, "scopeErr");
    checkIntValue(params, "stopErr");

    quint8 scopeErr = static_cast<quint8>(params.value("scopeErr").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&scopeErr), sizeof (quint8));

    quint8 stopErr = static_cast<quint8>(params.value("stopErr").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&stopErr), sizeof (quint8));
}

QJsonObject DMagicianProtocol::decodeSetStopPointParam(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetStopPointParam");
    }
    return resObj;
}

//! [217-12]
void DMagicianProtocol::encodeGetStopPointState(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)
    payload.p_cmdID = 217;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 12;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));
}

QJsonObject DMagicianProtocol::decodeGetStopPointState(quint8 rw, QByteArray params)
{

    QJsonObject resObj;

    if (rw == 0) {
        StopPointState s;
        memcpy(&s, params, sizeof (StopPointState));
        QJsonObject paramsObj;
        resObj.insert("cmd", "GetStopPointState");
        paramsObj.insert("result", s.isstop);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//! [217-20]
void DMagicianProtocol::encodeGetImgToArmXY(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 217;
    payload.p_ctrl.c_rw = 0;

    quint8 subid = 20;
    payload.p_params.append(reinterpret_cast<char *>(&subid), sizeof (quint8));

    checkFloatValue(params, "imgX");
    checkFloatValue(params, "imgY");

    float imgX = static_cast<float>(params.value("imgX").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&imgX), sizeof (float));

    float imgY = static_cast<float>(params.value("imgY").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&imgY), sizeof (float));

    uint8_t needTranxy = static_cast<uint8_t>(params.value("needTranxy").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&needTranxy), sizeof (uint8_t));

    uint8_t suckApriltag = static_cast<uint8_t>(params.value("suckApriltag").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&suckApriltag), sizeof (uint8_t));

    uint8_t apriltagHeight  = static_cast<uint8_t>(params.value("apriltagHeight").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&apriltagHeight), sizeof (uint8_t));
}

QJsonObject DMagicianProtocol::decodeGetImgToArmXY(quint8 rw, QByteArray params)
{
    QJsonObject resObj;
    if (rw == 0) {
        GetImgToArmXY s;
        memcpy(&s, params, sizeof (GetImgToArmXY));
        QJsonObject paramsObj;
        resObj.insert("cmd", "GetImgToArmXY");
        paramsObj.insert("armX", static_cast<double>(s.armX));
        paramsObj.insert("armY", static_cast<double>(s.armY));
        paramsObj.insert("okflag", s.okflag);

        resObj.insert("params", paramsObj);
    }
    return resObj;
}






