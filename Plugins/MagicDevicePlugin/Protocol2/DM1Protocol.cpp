#include "DM1Protocol.h"

#include <QDataStream>
#include <QJsonArray>
#include <QDateTime>
#include <QTime>
#include <QDebug>

DM1Protocol *DM1Protocol::getInstance()
{
    static DM1Protocol *instance = nullptr;
    if (instance == nullptr) {
        instance = new DM1Protocol();
    }
    return instance;
}

DM1Protocol::DM1Protocol(QObject *parent) : QObject(parent)
{
    _encodeFunctionInit();
    _decodeFunctionInit();
}

void DM1Protocol::_encodeFunctionInit()
{
    /* id:0~5 */
    m_encodeFuncMap.insert("SetDeviceSN", &DM1Protocol::encodeSetDeviceSN);
    m_encodeFuncMap.insert("GetDeviceSN", &DM1Protocol::encodeGetDeviceSN);
    m_encodeFuncMap.insert("SetDeviceName", &DM1Protocol::encodeSetDeviceName);
    m_encodeFuncMap.insert("GetDeviceName", &DM1Protocol::encodeGetDeviceName);
    m_encodeFuncMap.insert("GetDeviceVersion", &DM1Protocol::encodeGetDeviceVersion);
    m_encodeFuncMap.insert("GetHardwareVersion", &DM1Protocol::encodeGetHardwareVersion);

    /* id:10~13 */
    m_encodeFuncMap.insert("GetPose", &DM1Protocol::encodeGetPose);
    m_encodeFuncMap.insert("ResetPose", &DM1Protocol::encodeResetPose);

    /* id:20 */
    m_encodeFuncMap.insert("GetAlarmsState", &DM1Protocol::encodeGetAlarmsState);
    m_encodeFuncMap.insert("ClearAllAlarmsState", &DM1Protocol::encodeClearAllAlarmsState);

    /* id:28~29 */
    m_encodeFuncMap.insert("SetServoPower", &DM1Protocol::encodeSetServoPower);
    m_encodeFuncMap.insert("GetServoPower", &DM1Protocol::encodeGetServoPower);
    m_encodeFuncMap.insert("GetRunState", &DM1Protocol::encodeGetRunState);

    /* id:31~34 */
    m_encodeFuncMap.insert("SetHOMECmd", &DM1Protocol::encodeSetHOMECmd);
    m_encodeFuncMap.insert("SetHOMEInitialPos", &DM1Protocol::encodeSetHOMEInitialPos);

    /* id:40~42 */
    m_encodeFuncMap.insert("SetHHTTrigMode", &DM1Protocol::encodeSetHHTTrigMode);
    m_encodeFuncMap.insert("GetHHTTrigMode", &DM1Protocol::encodeGetHHTTrigMode);
    m_encodeFuncMap.insert("SetHHTTrigOutputEnabled", &DM1Protocol::encodeSetHHTTrigOutputEnabled);
    m_encodeFuncMap.insert("GetHHTTrigOutputEnabled", &DM1Protocol::encodeGetHHTTrigOutputEnabled);
    m_encodeFuncMap.insert("GetHHTTrigOutput", &DM1Protocol::encodeGetHHTTrigOutput);

    m_encodeFuncMap.insert("SetArmOrientation", &DM1Protocol::encodeSetArmOrientation);
    m_encodeFuncMap.insert("GetArmOrientation", &DM1Protocol::encodeGetArmOrientation);

    /* id:60~63 */
    m_encodeFuncMap.insert("SetEndEffectorParams", &DM1Protocol::encodeSetEndEffectorParams);
    m_encodeFuncMap.insert("GetEndEffectorParams", &DM1Protocol::encodeGetEndEffectorParams);
    m_encodeFuncMap.insert("SetEndEffectorLaser", &DM1Protocol::encodeSetEndEffectorLaser);
    m_encodeFuncMap.insert("GetEndEffectorLaser", &DM1Protocol::encodeGetEndEffectorLaser);
    m_encodeFuncMap.insert("SetEndEffectorSuctionCup", &DM1Protocol::encodeSetEndEffectorSuctionCup);
    m_encodeFuncMap.insert("GetEndEffectorSuctionCup", &DM1Protocol::encodeGetEndEffectorSuctionCup);
    m_encodeFuncMap.insert("SetEndEffectorGripper", &DM1Protocol::encodeSetEndEffectorGripper);
    m_encodeFuncMap.insert("GetEndEffectorGripper", &DM1Protocol::encodeGetEndEffectorGripper);

    /* id:70~76 */
    m_encodeFuncMap.insert("SetJOGJointParams", &DM1Protocol::encodeSetJOGJointParams);
    m_encodeFuncMap.insert("GetJOGJointParams", &DM1Protocol::encodeGetJOGJointParams);
    m_encodeFuncMap.insert("SetJOGCoordinateParams", &DM1Protocol::encodeSetJOGCoordinateParams);
    m_encodeFuncMap.insert("GetJOGCoordinateParams", &DM1Protocol::encodeGetJOGCoordinateParams);
    m_encodeFuncMap.insert("SetJOGCommonParams", &DM1Protocol::encodeSetJOGCommonParams);
    m_encodeFuncMap.insert("GetJOGCommonParams", &DM1Protocol::encodeGetJOGCommonParams);
    m_encodeFuncMap.insert("SetJOGCmd", &DM1Protocol::encodeSetJOGCmd);
    m_encodeFuncMap.insert("SetInchMode", &DM1Protocol::encodeSetInchMode);
    m_encodeFuncMap.insert("GetInchMode", &DM1Protocol::encodeGetInchMode);
    m_encodeFuncMap.insert("SetInchParam", &DM1Protocol::encodeSetInchParam);
    m_encodeFuncMap.insert("GetInchParam", &DM1Protocol::encodeGetInchParam);

    /* id:80~88 */
    m_encodeFuncMap.insert("SetPTPJointParams", &DM1Protocol::encodeSetPTPJointParams);
    m_encodeFuncMap.insert("GetPTPJointParams", &DM1Protocol::encodeGetPTPJointParams);
    m_encodeFuncMap.insert("SetPTPCoordinateParams", &DM1Protocol::encodeSetPTPCoordinateParams);
    m_encodeFuncMap.insert("GetPTPCoordinateParams", &DM1Protocol::encodeGetPTPCoordinateParams);
    m_encodeFuncMap.insert("SetPTPJumpParams", &DM1Protocol::encodeSetPTPJumpParams);
    m_encodeFuncMap.insert("GetPTPJumpParams", &DM1Protocol::encodeGetPTPJumpParams);
    m_encodeFuncMap.insert("SetPTPCommonParams", &DM1Protocol::encodeSetPTPCommonParams);
    m_encodeFuncMap.insert("GetPTPCommonParams", &DM1Protocol::encodeGetPTPCommonParams);
    m_encodeFuncMap.insert("SetPTPCmd", &DM1Protocol::encodeSetPTPCmd);
    m_encodeFuncMap.insert("SetPTPPOCmd", &DM1Protocol::encodeSetPTPPOCmd);

    /* id:90~95 */
    m_encodeFuncMap.insert("SetCPParams", &DM1Protocol::encodeSetCPParams);
    m_encodeFuncMap.insert("GetCPParams", &DM1Protocol::encodeGetCPParams);
    m_encodeFuncMap.insert("SetCPCmd", &DM1Protocol::encodeSetCPCmd);
    m_encodeFuncMap.insert("SetCPLECmd", &DM1Protocol::encodeSetCPLECmd);

    /* id:100~104、108~109 */
    m_encodeFuncMap.insert("SetARCParams", &DM1Protocol::encodeSetARCParams);
    m_encodeFuncMap.insert("GetARCParams", &DM1Protocol::encodeGetARCParams);
    m_encodeFuncMap.insert("SetARCCmd", &DM1Protocol::encodeSetARCCmd);
    m_encodeFuncMap.insert("SetCircleCmd", &DM1Protocol::encodeSetCircleCmd);
    m_encodeFuncMap.insert("SetMotivateCmd", &DM1Protocol::encodeSetMotivateCmd);
    m_encodeFuncMap.insert("SetMotivateZCmd", &DM1Protocol::encodeSetMotivateZCmd);
    m_encodeFuncMap.insert("SetARCPOCmd", &DM1Protocol::encodeSetARCPOCmd);
    m_encodeFuncMap.insert("SetCirclePOCmd", &DM1Protocol::encodeSetCirclePOCmd);

    /* id:110 */
    m_encodeFuncMap.insert("SetWAITCmd", &DM1Protocol::encodeSetWAITCmd);

    /* id:120 */
    m_encodeFuncMap.insert("SetTRIGCmd", &DM1Protocol::encodeSetTRIGCmd);

    /* id:130~139 */
    m_encodeFuncMap.insert("SetIODO", &DM1Protocol::encodeSetIODO);
    m_encodeFuncMap.insert("GetIODO", &DM1Protocol::encodeGetIODO);
    m_encodeFuncMap.insert("GetIODI", &DM1Protocol::encodeGetIODI);
    m_encodeFuncMap.insert("GetIOADC", &DM1Protocol::encodeGetIOADC);
    m_encodeFuncMap.insert("SetIODOs", &DM1Protocol::encodeSetIODOs);
    m_encodeFuncMap.insert("GetIODOs", &DM1Protocol::encodeGetIODOs);
    m_encodeFuncMap.insert("GetIODIs", &DM1Protocol::encodeGetIODIs);
    m_encodeFuncMap.insert("GetIOADCs", &DM1Protocol::encodeGetIOADCs);

    /* id:141 */
    m_encodeFuncMap.insert("SetRebootEncoderICPVL", &DM1Protocol::encodeSetRebootEncoderICPVL);

    /* id:161 */
    m_encodeFuncMap.insert("SetFirmwareMode", &DM1Protocol::encodeSetFirmwareMode);
    m_encodeFuncMap.insert("GetFirmwareMode", &DM1Protocol::encodeGetFirmwareMode);

    /* id:166~168 */
    m_encodeFuncMap.insert("SetFirmwareReboot", &DM1Protocol::encodeSetFirmwareReboot);
    m_encodeFuncMap.insert("SetLanPortConfig", &DM1Protocol::encodeSetLanPortConfig);
    m_encodeFuncMap.insert("GetLanPortConfig", &DM1Protocol::encodeGetLanPortConfig);
    m_encodeFuncMap.insert("SetFirmwareNotifyM4Mode", &DM1Protocol::encodeSetFirmwareNotifyM4Mode);
    m_encodeFuncMap.insert("GetFirmwareNotifyM4Mode", &DM1Protocol::encodeGetFirmwareNotifyM4Mode);

    /* id:200~211 */
    m_encodeFuncMap.insert("SetSafeModeEnabled", &DM1Protocol::encodeSetSafeModeEnabled);
    m_encodeFuncMap.insert("GetSafeModeEnabled", &DM1Protocol::encodeGetSafeModeEnabled);
    m_encodeFuncMap.insert("SetCollisionThreshold", &DM1Protocol::encodeSetCollisionThreshold);
    m_encodeFuncMap.insert("GetCollisionThreshold", &DM1Protocol::encodeGetCollisionThreshold);
    m_encodeFuncMap.insert("SetBasicDynamicParams", &DM1Protocol::encodeSetBasicDynamicParams);
    m_encodeFuncMap.insert("GetBasicDynamicParams", &DM1Protocol::encodeGetBasicDynamicParams);
    m_encodeFuncMap.insert("SetLoadParams", &DM1Protocol::encodeSetLoadParams);
    m_encodeFuncMap.insert("GetLoadParams", &DM1Protocol::encodeGetLoadParams);
    m_encodeFuncMap.insert("SetFeedforward", &DM1Protocol::encodeSetFeedforward);
    m_encodeFuncMap.insert("GetFeedforward", &DM1Protocol::encodeGetFeedforward);
    m_encodeFuncMap.insert("SetMotivationMode", &DM1Protocol::encodeSetMotivationMode);
    m_encodeFuncMap.insert("GetMotivationMode", &DM1Protocol::encodeGetMotivationMode);
    m_encodeFuncMap.insert("GetTrajectory", &DM1Protocol::encodeGetTrajectory);
    m_encodeFuncMap.insert("SetSafeStrategy", &DM1Protocol::encodeSetSafeStrategy);
    m_encodeFuncMap.insert("GetSafeStrategy", &DM1Protocol::encodeGetSafeStrategy);
    m_encodeFuncMap.insert("SetSafeGuardMode", &DM1Protocol::encodeSetSafeGuardMode);
    m_encodeFuncMap.insert("GetSafeGuardMode", &DM1Protocol::encodeGetSafeGuardMode);
    m_encodeFuncMap.insert("GetSafeGuardStatus", &DM1Protocol::encodeGetSafeGuardStatus);
    m_encodeFuncMap.insert("GetUserParams", &DM1Protocol::encodeGetUserParams);

    /* id:240~247 */
    m_encodeFuncMap.insert("QueuedCmdStart", &DM1Protocol::encodeSetQueuedCmdStart);
    m_encodeFuncMap.insert("QueuedCmdStop", &DM1Protocol::encodeSetQueuedCmdStop);
    m_encodeFuncMap.insert("QueuedCmdForceStop", &DM1Protocol::encodeSetQueuedCmdForceStop);
    m_encodeFuncMap.insert("QueuedCmdStartDownload", &DM1Protocol::encodeSetQueuedCmdStartDownload);
    m_encodeFuncMap.insert("QueuedCmdStopDownload", &DM1Protocol::encodeSetQueuedCmdStopDownload);
    m_encodeFuncMap.insert("QueuedCmdClear", &DM1Protocol::encodeSetQueuedCmdClear);
    m_encodeFuncMap.insert("GetQueuedCmdCurrentIndex", &DM1Protocol::encodeGetQueuedCmdCurrentIndex);
    m_encodeFuncMap.insert("GetQueuedCmdLeftSpace", &DM1Protocol::encodeGetQueuedCmdLeftSpace);

    /* id:250~251 */
    m_encodeFuncMap.insert("SetUserCoordinate", &DM1Protocol::encodeSetUserCoordinate);
    m_encodeFuncMap.insert("GetUserCoordinate", &DM1Protocol::encodeGetUserCoordinate);
    m_encodeFuncMap.insert("SetToolCoordinate", &DM1Protocol::encodeSetToolCoordinate);
    m_encodeFuncMap.insert("GetToolCoordinate", &DM1Protocol::encodeGetToolCoordinate);
}

void DM1Protocol::_decodeFunctionInit()
{
    m_decodeFuncMap.insert(0, &DM1Protocol::decodeDeviceSN);
    m_decodeFuncMap.insert(1, &DM1Protocol::decodeDeviceName);
    m_decodeFuncMap.insert(2, &DM1Protocol::decodeDeviceVersion);
    m_decodeFuncMap.insert(5, &DM1Protocol::decodeHardwareVersion);

    m_decodeFuncMap.insert(10, &DM1Protocol::decodePose);
    m_decodeFuncMap.insert(11, &DM1Protocol::decodeResetPose);

    m_decodeFuncMap.insert(20, &DM1Protocol::decodeAlarmState);

    m_decodeFuncMap.insert(28, &DM1Protocol::decodeServoPower);
    m_decodeFuncMap.insert(29, &DM1Protocol::decodeRunState);

    m_decodeFuncMap.insert(31, &DM1Protocol::decodeHOMECmd);
    m_decodeFuncMap.insert(34, &DM1Protocol::decodeHOMEInitialPos);

    m_decodeFuncMap.insert(40, &DM1Protocol::decodeHHTTrigMode);
    m_decodeFuncMap.insert(41, &DM1Protocol::decodeHHTTrigOutputEnabled);
    m_decodeFuncMap.insert(42, &DM1Protocol::decodeHHTTrigOutput);

    m_decodeFuncMap.insert(50, &DM1Protocol::decodeArmOrientation);

    m_decodeFuncMap.insert(60, &DM1Protocol::decodeEndEffectorParams);
    m_decodeFuncMap.insert(61, &DM1Protocol::decodeEndEffectorLaser);
    m_decodeFuncMap.insert(62, &DM1Protocol::decodeEndEffectorSuctionCup);
    m_decodeFuncMap.insert(63, &DM1Protocol::decodeEndEffectorGripper);

    m_decodeFuncMap.insert(70, &DM1Protocol::decodeJOGJointParams);
    m_decodeFuncMap.insert(71, &DM1Protocol::decodeJOGCoordinateParams);
    m_decodeFuncMap.insert(72, &DM1Protocol::decodeJOGCommonParams);
    m_decodeFuncMap.insert(73, &DM1Protocol::decodeJOGCmd);

    m_decodeFuncMap.insert(75, &DM1Protocol::decodeInchMode);
    m_decodeFuncMap.insert(76, &DM1Protocol::decodeInchParam);

    m_decodeFuncMap.insert(80, &DM1Protocol::decodePTPJointParams);
    m_decodeFuncMap.insert(81, &DM1Protocol::decodePTPCoordinateParams);
    m_decodeFuncMap.insert(82, &DM1Protocol::decodePTPJumpParams);
    m_decodeFuncMap.insert(83, &DM1Protocol::decodePTPCommonParams);
    m_decodeFuncMap.insert(84, &DM1Protocol::decodePTPCmd);
    m_decodeFuncMap.insert(88, &DM1Protocol::decodePTPPOCmd);

    m_decodeFuncMap.insert(90, &DM1Protocol::decodeCPParams);
    m_decodeFuncMap.insert(91, &DM1Protocol::decodeCPCmd);
    m_decodeFuncMap.insert(92, &DM1Protocol::decodeCPLECmd);

    m_decodeFuncMap.insert(100, &DM1Protocol::decodeARCParams);
    m_decodeFuncMap.insert(101, &DM1Protocol::decodeARCCmd);
    m_decodeFuncMap.insert(102, &DM1Protocol::decodeCircleCmd);
    m_decodeFuncMap.insert(103, &DM1Protocol::decodeMotivateCmd);
    m_decodeFuncMap.insert(104, &DM1Protocol::decodeMotivateZCmd);

    m_decodeFuncMap.insert(108, &DM1Protocol::decodeARCPOCmd);
    m_decodeFuncMap.insert(109, &DM1Protocol::decodeCirclePOCmd);

    m_decodeFuncMap.insert(110, &DM1Protocol::decodeWAITCmd);

    m_decodeFuncMap.insert(120, &DM1Protocol::decodeTRIGCmd);

    m_decodeFuncMap.insert(131, &DM1Protocol::decodeIODO);
    m_decodeFuncMap.insert(133, &DM1Protocol::decodeIODI);
    m_decodeFuncMap.insert(134, &DM1Protocol::decodeIOADC);
    m_decodeFuncMap.insert(136, &DM1Protocol::decodeIODOs);
    m_decodeFuncMap.insert(137, &DM1Protocol::decodeGetIODIs);
    m_decodeFuncMap.insert(138, &DM1Protocol::decodeGetIOADCs);

    m_decodeFuncMap.insert(141, &DM1Protocol::decodeRebootEncoderICPVL);

    m_decodeFuncMap.insert(161, &DM1Protocol::decodeFirmwareMode);

    m_decodeFuncMap.insert(166, &DM1Protocol::decodeFirmwareReboot);
    m_decodeFuncMap.insert(167, &DM1Protocol::decodeLanPortConfig);
    m_decodeFuncMap.insert(168, &DM1Protocol::decodeFirmwareNotifyM4Mode);

    m_decodeFuncMap.insert(200, &DM1Protocol::decodeSafeModeEnabled);
    m_decodeFuncMap.insert(201, &DM1Protocol::decodeCollisionThreshold);
    m_decodeFuncMap.insert(202, &DM1Protocol::decodeBasicDynamicParams);
    m_decodeFuncMap.insert(203, &DM1Protocol::decodeLoadParams);
    m_decodeFuncMap.insert(204, &DM1Protocol::decodeFeedforward);
    m_decodeFuncMap.insert(205, &DM1Protocol::decodeMotivationMode);
    m_decodeFuncMap.insert(206, &DM1Protocol::decodeTrajectory);
    m_decodeFuncMap.insert(208, &DM1Protocol::decodeSafeStrategy);
    m_decodeFuncMap.insert(210, &DM1Protocol::decodeSafeGuardMode);
    m_decodeFuncMap.insert(211, &DM1Protocol::decodeSafeGuardStatus);
    m_decodeFuncMap.insert(220, &DM1Protocol::decodeGetUserParams);

    m_decodeFuncMap.insert(240, &DM1Protocol::decodeQueuedCmdStart);
    m_decodeFuncMap.insert(241, &DM1Protocol::decodeQueuedCmdStop);
    m_decodeFuncMap.insert(242, &DM1Protocol::decodeQueuedCmdForceStop);
    m_decodeFuncMap.insert(243, &DM1Protocol::decodeQueuedCmdStartDownload);
    m_decodeFuncMap.insert(244, &DM1Protocol::decodeQueuedCmdStopDownload);
    m_decodeFuncMap.insert(245, &DM1Protocol::decodeQueuedCmdClear);
    m_decodeFuncMap.insert(246, &DM1Protocol::decodeQueuedCmdCurrentIndex);
    m_decodeFuncMap.insert(247, &DM1Protocol::decodeQueuedCmdLeftSpace);

    m_decodeFuncMap.insert(250, &DM1Protocol::decodeUserCoordinate);
    m_decodeFuncMap.insert(251, &DM1Protocol::decodeToolCoordinate);
}

bool DM1Protocol::checkIntValue(const QJsonObject &obj, const QString &value)
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

bool DM1Protocol::checkFloatValue(const QJsonObject &obj, const QString &value)
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

bool DM1Protocol::checkBoolValue(const QJsonObject &obj, const QString &value)
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

bool DM1Protocol::checkStringValue(const QJsonObject &obj, const QString &value, QString defaultValue)
{
    if (!obj.contains(value)) {
        qDebug().noquote() << QString("'%1':missing, use ('%2')").arg(value).arg(defaultValue);
        return false;
    } else if (!obj.value(value).isString()) {
        qWarning().noquote() << QString("'%1':type error").arg(value);
        return false;
    }
    return true;
}

bool DM1Protocol::checkArrayValue(const QJsonObject &obj, const QString &value)
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

QJsonObject DM1Protocol::addQueueIndexInfo(QJsonObject &resObj, QByteArray &params)
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

const QStringList DM1Protocol::getCommandList()
{
    return m_encodeFuncMap.keys();
}

QJsonObject DM1Protocol::parseParams(PacketPayload &payload)
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

bool DM1Protocol::setPayload(QString cmd, PacketPayload &payload, QJsonObject params)
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
QJsonObject DM1Protocol::decodeDeviceSN(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeDeviceName(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeDeviceVersion(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetDeviceVersion");
    }

    if (params.size() == 4) {
        quint8 type = 0, majorVersion = 0, minorVersion = 0, revision = 0;
        QDataStream out(&params, QIODevice::ReadOnly);
        out >> type >> majorVersion >> minorVersion >> revision;

        QJsonObject paramsObj;
        paramsObj.insert("type", type);
        paramsObj.insert("majorVersion", majorVersion);
        paramsObj.insert("minorVersion", minorVersion);
        paramsObj.insert("revision", revision);
        resObj.insert("params", paramsObj);
    } else if (params.size() >= 9) {
        quint8 type = 0;
        memcpy(&type, params, sizeof (quint8));

        QByteArray majorVersion = params.mid(1, 2).data();
        QByteArray minorVersion = params.mid(3, 2).data();
        QByteArray revision = params.mid(5, 2).data();
        QByteArray debug = params.mid(7, 2).data();

        QJsonObject paramsObj;
        paramsObj.insert("type", type);
        paramsObj.insert("majorVersion", majorVersion.toInt());
        paramsObj.insert("minorVersion", minorVersion.toInt());
        paramsObj.insert("revision", revision.toInt());
        paramsObj.insert("debug", debug.toInt());
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![5]
QJsonObject DM1Protocol::decodeHardwareVersion(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetHardwareVersion");
    }

    if (params.size() == 12 * 11 * sizeof (char)) {
        QJsonObject paramsObj;
        paramsObj.insert("machineNum", params.mid(0, 11).data());
        paramsObj.insert("mainBoard", params.mid(11, 11).data());
        paramsObj.insert("driverRearArm", params.mid(22, 11).data());
        paramsObj.insert("driverFrontArm", params.mid(33, 11).data());
        paramsObj.insert("driverZArm", params.mid(44, 11).data());
        paramsObj.insert("driverRArm", params.mid(55, 11).data());
        paramsObj.insert("encoderRearArm", params.mid(66, 11).data());
        paramsObj.insert("encoderFrontArm", params.mid(77, 11).data());
        paramsObj.insert("encoderZArm", params.mid(88, 11).data());
        paramsObj.insert("encoderRArm", params.mid(99, 11).data());
        paramsObj.insert("brakeBoard", params.mid(120, 11).data());
        paramsObj.insert("endIOBoard", params.mid(131, 11).data());
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![10]
QJsonObject DM1Protocol::decodePose(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetPose");
    }

    if (params.size() == sizeof (PoseStruct)) {
        PoseStruct pose;
        memcpy(&pose, params, sizeof (PoseStruct));

        QJsonObject paramsObj;
        paramsObj.insert("x", static_cast<double>(pose.coord.x));
        paramsObj.insert("y", static_cast<double>(pose.coord.y));
        paramsObj.insert("z", static_cast<double>(pose.coord.z));
        paramsObj.insert("r", static_cast<double>(pose.coord.r));

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
QJsonObject DM1Protocol::decodeResetPose(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "ResetPose");
    }
    return resObj;
}

//![20]
QJsonObject DM1Protocol::decodeAlarmState(quint8 rw, QByteArray params)
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

//![28]
QJsonObject DM1Protocol::decodeServoPower(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetServoPower");
    } else {
        resObj.insert("cmd", "SetServoPower");
    }

    if (params.size() == sizeof (quint8)){
        quint8 on = 0;
        memcpy(&on, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("on", on > 0 ? true : false);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![29]
QJsonObject DM1Protocol::decodeRunState(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetRunState");
    }

    if (params.size() == sizeof (quint32)){
        quint32 state = 0;
        memcpy(&state, params, sizeof (quint32));

        QJsonObject paramsObj;
        paramsObj.insert("state", static_cast<double>(state));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![31]
QJsonObject DM1Protocol::decodeHOMECmd(quint8 rw, QByteArray params)
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

//![34]
QJsonObject DM1Protocol::decodeHOMEInitialPos(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetHOMEInitialPos");
    }
    return resObj;
}

//![40]
QJsonObject DM1Protocol::decodeHHTTrigMode(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetHHTTrigMode");
    } else {
        resObj.insert("cmd", "SetHHTTrigMode");
    }

    if (params.size() == sizeof (quint8)){
        quint8 mode = 0;
        memcpy(&mode, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("mode", mode);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![41]
QJsonObject DM1Protocol::decodeHHTTrigOutputEnabled(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeHHTTrigOutput(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeArmOrientation(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetArmOrientation");
    } else {
        resObj.insert("cmd", "SetArmOrientation");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 orientation = 0;
        memcpy(&orientation, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("orientation", orientation);
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![60]
QJsonObject DM1Protocol::decodeEndEffectorParams(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeEndEffectorLaser(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeEndEffectorSuctionCup(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeEndEffectorGripper(quint8 rw, QByteArray params)
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

//![70]
QJsonObject DM1Protocol::decodeJOGJointParams(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeJOGCoordinateParams(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeJOGCommonParams(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeJOGCmd(quint8 rw, QByteArray params)
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

//![75]
QJsonObject DM1Protocol::decodeInchMode(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetInchMode");
    } else {
        resObj.insert("cmd", "SetInchMode");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 mode;
        memcpy(&mode, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("mode", mode);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![76]
QJsonObject DM1Protocol::decodeInchParam(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetInchParam");
    } else {
        resObj.insert("cmd", "SetInchParam");
    }

    if (params.size() == sizeof (InchParamsStruct)) {
        InchParamsStruct inchParams;
        memcpy(&inchParams, params, sizeof (InchParamsStruct));

        QJsonObject paramsObj;
        paramsObj.insert("distanceMM", static_cast<double>(inchParams.distanceMM));
        paramsObj.insert("distanceANG", static_cast<double>(inchParams.distanceANG));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![80]
QJsonObject DM1Protocol::decodePTPJointParams(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodePTPCoordinateParams(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodePTPJumpParams(quint8 rw, QByteArray params)
{
    struct PTPJumpParamsStruct {
        float jumpHeight;
        float zLimit;
        quint32 isUsingZLimit;
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
        paramsObj.insert("isUsingZLimit", ptpParams.zLimit > 0 ? true : false);
        resObj.insert("params", paramsObj);
    } else if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![83]
QJsonObject DM1Protocol::decodePTPCommonParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetPTPCommonParams");
        if (params.size() == sizeof (VelocityRatioStruct)) {
            VelocityRatioStruct ptpParams;
            memcpy(&ptpParams, params, sizeof (VelocityRatioStruct));

            QJsonObject paramsObj;
            paramsObj.insert("velocityRatio", static_cast<double>(ptpParams.velocityRatio));
            paramsObj.insert("accelerationRatio", static_cast<double>(ptpParams.accelerationRatio));
            resObj.insert("params", paramsObj);
        }
    } else {
        resObj.insert("cmd", "SetPTPCommonParams");
        if (params.size() == sizeof (quint64)) {
           addQueueIndexInfo(resObj, params);
       }
    }


    return resObj;
}

//![84]
QJsonObject DM1Protocol::decodePTPCmd(quint8 rw, QByteArray params)
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

//![88]
QJsonObject DM1Protocol::decodePTPPOCmd(quint8 rw, QByteArray params)
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

//![90]
QJsonObject DM1Protocol::decodeCPParams(quint8 rw, QByteArray params)
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

        if (cpParams.realTimeTrack > 0) {
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
QJsonObject DM1Protocol::decodeCPCmd(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeCPLECmd(quint8 rw, QByteArray params)
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

//![100]
QJsonObject DM1Protocol::decodeARCParams(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeARCCmd(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeCircleCmd(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetCircleCmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![103]
QJsonObject DM1Protocol::decodeMotivateCmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetMotivateCmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![104]
QJsonObject DM1Protocol::decodeMotivateZCmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetMotivateZCmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![108]
QJsonObject DM1Protocol::decodeARCPOCmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetARCPOCmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![109]
QJsonObject DM1Protocol::decodeCirclePOCmd(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetCirclePOCmd");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![110]
QJsonObject DM1Protocol::decodeWAITCmd(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeTRIGCmd(quint8 rw, QByteArray params)
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

//![131]
QJsonObject DM1Protocol::decodeIODO(quint8 rw, QByteArray params)
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

//![133]
QJsonObject DM1Protocol::decodeIODI(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeIOADC(quint8 rw, QByteArray params)
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

QJsonObject DM1Protocol::decodeIODOs(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetIODOs");
        if (params.size() >= static_cast<int>(2 * sizeof (quint8))) {
            qDebug()<<"params:"<<params;
            quint8 address = 0, number = 0;
            QDataStream out(&params, QIODevice::ReadOnly);
            out >> address >> number;
            QJsonObject paramsObj;
            paramsObj.insert("address", address);
            paramsObj.insert("number", number);
            QJsonArray valueArray;
            for (int i = 0; i<number; i++) {
                quint8 value;
                out >> value;
                valueArray.append(value);
            }
            paramsObj.insert("level", valueArray);
            resObj.insert("params", paramsObj);
        }
    } else {
        resObj.insert("cmd", "SetIODOs");
    }

    return resObj;
}

QJsonObject DM1Protocol::decodeGetIODIs(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetIODIs");
        if (params.size() >= static_cast<int>(2 * sizeof (quint8))) {
            qDebug()<<"params:"<<params;
            quint8 address = 0, number = 0;
            QDataStream out(&params, QIODevice::ReadOnly);
            out >> address >> number;
            QJsonObject paramsObj;
            paramsObj.insert("address", address);
            paramsObj.insert("number", number);
            QJsonArray valueArray;
            for (int i = 0; i<number; i++) {
                quint8 value;
                out >> value;
                valueArray.append(value);
            }
            paramsObj.insert("value", valueArray);
            resObj.insert("params", paramsObj);
        }
    }
    return resObj;
}

QJsonObject DM1Protocol::decodeGetIOADCs(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetIOADCs");
        if (params.size() >= static_cast<int>(2 * sizeof (quint8))) {
            qDebug()<<"params:"<<params;
            quint8 address = 0, number = 0;
            QDataStream out(&params, QIODevice::ReadOnly);
            out >> address >> number;
            QJsonObject paramsObj;
            paramsObj.insert("address", address);
            paramsObj.insert("number", number);
            QJsonArray valueArray;
            for (int i = 0; i<number; i++) {
                quint16 value;
                out >> value;
                valueArray.append(value);
            }
            paramsObj.insert("level", valueArray);
            resObj.insert("params", paramsObj);
        }
    }
    return resObj;
}

QJsonObject DM1Protocol::decodeGetIODACs(quint8 rw, QByteArray params)
{
    Q_UNUSED(rw)
    Q_UNUSED(params)
    return QJsonObject();
}

QJsonObject DM1Protocol::decodeRebootEncoderICPVL(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetRebootEncoderICPVL");
    }
    return resObj;
}

//![161]
QJsonObject DM1Protocol::decodeFirmwareMode(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetFirmwareMode");
        if (params.size() == sizeof(FirmwareModeStruct)) {
            FirmwareModeStruct mode;
            memcpy(&mode, params, sizeof(FirmwareModeStruct));

            QJsonObject paramsObj;
            paramsObj.insert("firmwareMode", mode.mode);
            paramsObj.insert("ctrl", mode.ctrl);
            resObj.insert("params", paramsObj);
        }
    } else {
        resObj.insert("cmd", "SetFirmwareMode");
    }

    if (params.size() == sizeof (quint64)) {
        addQueueIndexInfo(resObj, params);
    }
    return resObj;
}

//![166]
QJsonObject DM1Protocol::decodeFirmwareReboot(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "SetFirmwareReboot");
    }
    return resObj;
}

//![167]
QJsonObject DM1Protocol::decodeLanPortConfig(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetLanPortConfig");
    } else {
        resObj.insert("cmd", "SetLanPortConfig");
    }

    if (params.size() == sizeof (LanPortInfoStruct)) {
        LanPortInfoStruct info;
        memcpy(&info, params, sizeof (LanPortInfoStruct));

        QJsonObject paramsObj;
        paramsObj.insert("status", info.status > 0 ? true : false);
        paramsObj.insert("isdhcp", info.isdhcp > 0 ? true : false);
        paramsObj.insert("addr", reinterpret_cast<char*>(&info.addr));
        paramsObj.insert("mask", reinterpret_cast<char*>(&info.mask));
        paramsObj.insert("gateway", reinterpret_cast<char*>(&info.gateway));
        paramsObj.insert("dns", reinterpret_cast<char*>(&info.dns));

        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![168]
QJsonObject DM1Protocol::decodeFirmwareNotifyM4Mode(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetFirmwareNotifyM4Mode");
        if (params.size() == sizeof (quint8)) {
            quint8 mode;
            memcpy(&mode, params, sizeof (quint8));

            QJsonObject paramsObj;
            paramsObj.insert("mode", mode);
            resObj.insert("params", paramsObj);
        }
    } else {
        resObj.insert("cmd", "SetFirmwareNotifyM4Mode");
    }


    return resObj;
}

//![200]
QJsonObject DM1Protocol::decodeSafeModeEnabled(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetSafeModeEnabled");
    } else {
        resObj.insert("cmd", "SetSafeModeEnabled");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 enabled = 0;
        memcpy(&enabled, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("isEnabled", enabled > 0 ? true : false);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![201]
QJsonObject DM1Protocol::decodeCollisionThreshold(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetCollisionThreshold");
    } else {
        resObj.insert("cmd", "SetCollisionThreshold");
    }

    if (params.size() == sizeof (TorDiffParamsStruct)) {
        TorDiffParamsStruct torDiffParams;
        memcpy(&torDiffParams, params, sizeof (TorDiffParamsStruct));

        QJsonObject paramsObj;
        paramsObj.insert("torDiffJ1", static_cast<double>(torDiffParams.torDiffJ1));
        paramsObj.insert("torDiffJ2", static_cast<double>(torDiffParams.torDiffJ2));
        paramsObj.insert("torDiffJ3", static_cast<double>(torDiffParams.torDiffJ3));
        paramsObj.insert("torDiffJ4", static_cast<double>(torDiffParams.torDiffJ4));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![202]
QJsonObject DM1Protocol::decodeBasicDynamicParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetBasicDynamicParams");
    } else {
        resObj.insert("cmd", "SetBasicDynamicParams");
    }

    if (params.size() == sizeof (BasicDynamicParamsStruct)) {
        BasicDynamicParamsStruct dynamicParams;
        memcpy(&dynamicParams, params, sizeof (BasicDynamicParamsStruct));

        QJsonObject paramsObj;
        paramsObj.insert("ZZ1", static_cast<double>(dynamicParams.ZZ1));
        paramsObj.insert("FS1", static_cast<double>(dynamicParams.FS1));
        paramsObj.insert("FV1", static_cast<double>(dynamicParams.FV1));
        paramsObj.insert("ZZ2", static_cast<double>(dynamicParams.ZZ2));
        paramsObj.insert("MX2", static_cast<double>(dynamicParams.MX2));
        paramsObj.insert("MY2", static_cast<double>(dynamicParams.MY2));
        paramsObj.insert("IA2", static_cast<double>(dynamicParams.IA2));
        paramsObj.insert("FS2", static_cast<double>(dynamicParams.FS2));
        paramsObj.insert("FV2", static_cast<double>(dynamicParams.FV2));

        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![203]
QJsonObject DM1Protocol::decodeLoadParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetLoadParams");
    } else {
        resObj.insert("cmd", "SetLoadParams");
    }

    if (params.size() == sizeof (float)) {
        float loadParams;
        memcpy(&loadParams, params, sizeof (float));

        QJsonObject paramsObj;
        paramsObj.insert("loadParams", static_cast<double>(loadParams));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![204]
QJsonObject DM1Protocol::decodeFeedforward(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetFeedforward");
    } else {
        resObj.insert("cmd", "SetFeedforward");
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

//![205]
QJsonObject DM1Protocol::decodeMotivationMode(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetMotivationMode");
    } else {
        resObj.insert("cmd", "SetMotivationMode");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 mode;
        memcpy(&mode, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("mode", static_cast<double>(mode));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![206]
QJsonObject DM1Protocol::decodeTrajectory(quint8 rw, QByteArray params)
{
    struct DataStruct {
        float data1;
        float data2;
        float data3;
        float data4;
    };

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetTrajectory");
    }

    if (params.size() > 0) {
        quint8 count;
        memcpy(&count, params, sizeof (quint8));

        QJsonArray dataArray;
        int dataSize = sizeof (DataStruct);
        if (params.size() == dataSize * count + 1) {
            for (int i = 0; i < count; ++i) {
                QByteArray ba = params.mid(dataSize * i + 1, sizeof (DataStruct));
                DataStruct data;
                memcpy(&data, ba, sizeof (DataStruct));

                QJsonObject dataObj;
                dataObj.insert("data1", static_cast<double>(data.data1));
                dataObj.insert("data2", static_cast<double>(data.data2));
                dataObj.insert("data3", static_cast<double>(data.data3));
                dataObj.insert("data4", static_cast<double>(data.data4));
                dataArray.append(dataObj);
            }
        } else {
            qDebug() << "GetTrajectory: count error, count:" << count
                     << "target size:" << dataSize * count
                     << "get size" << params.size();
        }

        QJsonObject paramsObj;
        paramsObj.insert("data", dataArray);
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![208]
QJsonObject DM1Protocol::decodeSafeStrategy(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetSafeStrategy");
    } else {
        resObj.insert("cmd", "SetSafeStrategy");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 strategy;
        memcpy(&strategy, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("strategy", static_cast<double>(strategy));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![210]
QJsonObject DM1Protocol::decodeSafeGuardMode(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetSafeGuardMode");
    } else {
        resObj.insert("cmd", "SetSafeGuardMode");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 mode;
        memcpy(&mode, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("mode", static_cast<double>(mode));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![211]
QJsonObject DM1Protocol::decodeSafeGuardStatus(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetSafeGuardStatus");
    }

    if (params.size() == sizeof (quint8)) {
        quint8 status;
        memcpy(&status, params, sizeof (quint8));

        QJsonObject paramsObj;
        paramsObj.insert("status", static_cast<double>(status));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![220]
QJsonObject DM1Protocol::decodeGetUserParams(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetUserParams");
    }
    qDebug() << params.size() << sizeof (tagUserParams);
    if (params.size() == sizeof (tagUserParams)) {
        tagUserParams userParams;
        memcpy(&userParams, params, sizeof (tagUserParams));

        QJsonArray userParamsArray;
        for (int i = 0; i < 8; ++i) {
            userParamsArray.append(static_cast<double>(userParams.params[i]));
        }
        QJsonObject paramsObj;
        paramsObj.insert("userParams", userParamsArray);

        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![240]
QJsonObject DM1Protocol::decodeQueuedCmdStart(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "QueuedCmdStart");
    }
    return resObj;
}

//![241]
QJsonObject DM1Protocol::decodeQueuedCmdStop(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "QueuedCmdStop");
    }
    return resObj;
}

//![242]
QJsonObject DM1Protocol::decodeQueuedCmdForceStop(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "QueuedCmdForceStop");
    }
    return resObj;
}

//![243]
QJsonObject DM1Protocol::decodeQueuedCmdStartDownload(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "QueuedCmdStartDownload");
    }
    return resObj;
}

//![244]
QJsonObject DM1Protocol::decodeQueuedCmdStopDownload(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "QueuedCmdStopDownload");
    }
    return resObj;
}

//![245]
QJsonObject DM1Protocol::decodeQueuedCmdClear(quint8 rw, QByteArray params)
{
    Q_UNUSED(params)

    QJsonObject resObj;

    if (rw == 1) {
        resObj.insert("cmd", "QueuedCmdClear");
    }
    return resObj;
}

//![246]
QJsonObject DM1Protocol::decodeQueuedCmdCurrentIndex(quint8 rw, QByteArray params)
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
QJsonObject DM1Protocol::decodeQueuedCmdLeftSpace(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetQueuedCmdLeftSpace");
    }

    if (params.size() == sizeof (quint32)) {
        quint32 leftSpace = 0;
        memcpy(&leftSpace, params, sizeof (quint32));

        QJsonObject paramsObj;
        paramsObj.insert("leftSpace", static_cast<double>(leftSpace));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![250]
QJsonObject DM1Protocol::decodeUserCoordinate(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetUserCoordinate");
    } else {
        resObj.insert("cmd", "SetUserCoordinate");
    }

    if (params.size() == sizeof (PositionStruct)) {
        PositionStruct coord;
        memcpy(&coord, params, sizeof (PositionStruct));

        QJsonObject paramsObj;
        paramsObj.insert("x", static_cast<double>(coord.x));
        paramsObj.insert("y", static_cast<double>(coord.y));
        paramsObj.insert("z", static_cast<double>(coord.z));
        paramsObj.insert("r", static_cast<double>(coord.r));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}

//![251]
QJsonObject DM1Protocol::decodeToolCoordinate(quint8 rw, QByteArray params)
{
    QJsonObject resObj;

    if (rw == 0) {
        resObj.insert("cmd", "GetToolCoordinate");
    } else {
        resObj.insert("cmd", "SetToolCoordinate");
    }

    if (params.size() == sizeof (PositionStruct)) {
        PositionStruct coord;
        memcpy(&coord, params, sizeof (PositionStruct));

        QJsonObject paramsObj;
        paramsObj.insert("x", static_cast<double>(coord.x));
        paramsObj.insert("y", static_cast<double>(coord.y));
        paramsObj.insert("z", static_cast<double>(coord.z));
        paramsObj.insert("r", static_cast<double>(coord.r));
        resObj.insert("params", paramsObj);
    }
    return resObj;
}


/**********************************************************************************
 * *** ENCODE *** ENCODE *** ENCODE *** ENCODE *** ENCODE *** ENCODE *** ENCODE ***
 *********************************************************************************/

/* 设备信息 */
//! [0]
void DM1Protocol::encodeSetDeviceSN(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 0;
    payload.p_ctrl.c_rw = 1;

    checkStringValue(params, "deviceSN");

    QString deviceSN = params.value("deviceSN").toString();
    payload.p_params = deviceSN.toLatin1() + '\0';
}

void DM1Protocol::encodeGetDeviceSN(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 0;
    payload.p_ctrl.c_rw = 0;
}

//! [1]
void DM1Protocol::encodeSetDeviceName(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 1;
    payload.p_ctrl.c_rw = 1;

    checkStringValue(params, "deviceName");

    QString deviceName = params.value("deviceName").toString();
    payload.p_params = deviceName.toUtf8() + '\0';
}

void DM1Protocol::encodeGetDeviceName(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 1;
    payload.p_ctrl.c_rw = 0;
}

//! [2]
void DM1Protocol::encodeGetDeviceVersion(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 2;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "type");

    quint8 typeIndex = static_cast<quint8>(params.value("type").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&typeIndex), sizeof (quint8));
}

//! [5]
void DM1Protocol::encodeGetHardwareVersion(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 5;
    payload.p_ctrl.c_rw = 0;
}

/* 实时位姿 */
//! [10]
void DM1Protocol::encodeGetPose(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 10;
    payload.p_ctrl.c_rw = 0;
}

//! [11]
void DM1Protocol::encodeResetPose(PacketPayload &payload, QJsonObject params)
{
#pragma pack(push, 1)
    struct ResetPoseStruct {
        quint8 manual;
        float frontAngle1;
        float frontAngle2;
    };
#pragma pack(pop)

    payload.p_cmdID = 11;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "manual");
    checkFloatValue(params, "rearArmAngle");
    checkFloatValue(params, "frontArmAngle");

    ResetPoseStruct resetpose;
    resetpose.manual = static_cast<quint8>(params.value("manual").toInt());
    resetpose.frontAngle1 = static_cast<float>(params.value("frontAngle1").toDouble());
    resetpose.frontAngle2 = static_cast<float>(params.value("frontAngle2").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&resetpose), sizeof (ResetPoseStruct));
}

/* 报警功能 */
//! [20]
void DM1Protocol::encodeGetAlarmsState(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 20;
    payload.p_ctrl.c_rw = 0;
}

void DM1Protocol::encodeClearAllAlarmsState(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 20;
    payload.p_ctrl.c_rw = 1;
}

//! [28]
void DM1Protocol::encodeSetServoPower(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 28;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "on");

    quint8 on = params.value("on").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&on), sizeof (quint8));
}

void DM1Protocol::encodeGetServoPower(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 20;
    payload.p_ctrl.c_rw = 0;
}

//! [29]
void DM1Protocol::encodeGetRunState(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 29;
    payload.p_ctrl.c_rw = 0;
}

/* 回零 */
//! [31]
void DM1Protocol::encodeSetHOMECmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 31;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "isResetPars");

    quint8 isResetPars = params.value("isResetPars").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&isResetPars), sizeof (quint8));
}

//! [34]
void DM1Protocol::encodeSetHOMEInitialPos(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 34;
    payload.p_ctrl.c_rw = 1;
}

//! [40]
void DM1Protocol::encodeSetHHTTrigMode(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 40;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "mode");

    quint8 mode = static_cast<quint8>(params.value("mode").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&mode), sizeof (quint8));
}

void DM1Protocol::encodeGetHHTTrigMode(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 40;
    payload.p_ctrl.c_rw = 0;
}

//! [41]
void DM1Protocol::encodeSetHHTTrigOutputEnabled(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 41;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "enable");

    quint8 enable = params.value("enable").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&enable), sizeof (quint8));
}

void DM1Protocol::encodeGetHHTTrigOutputEnabled(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 41;
    payload.p_ctrl.c_rw = 0;
}

//! [42]
void DM1Protocol::encodeGetHHTTrigOutput(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 42;
    payload.p_ctrl.c_rw = 0;
}

//! [50]
void DM1Protocol::encodeSetArmOrientation(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 50;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "orientation");

    quint8 orientation = static_cast<quint8>(params.value("orientation").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&orientation), sizeof (quint8));
}

void DM1Protocol::encodeGetArmOrientation(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 50;
    payload.p_ctrl.c_rw = 0;
}

//! [60]
void DM1Protocol::encodeSetEndEffectorParams(PacketPayload &payload, QJsonObject params)
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

void DM1Protocol::encodeGetEndEffectorParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 60;
    payload.p_ctrl.c_rw = 0;
}

//! [61]
void DM1Protocol::encodeSetEndEffectorLaser(PacketPayload &payload, QJsonObject params)
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

void DM1Protocol::encodeGetEndEffectorLaser(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 61;
    payload.p_ctrl.c_rw = 0;
}

//! [62]
void DM1Protocol::encodeSetEndEffectorSuctionCup(PacketPayload &payload, QJsonObject params)
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

void DM1Protocol::encodeGetEndEffectorSuctionCup(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 62;
    payload.p_ctrl.c_rw = 0;
}

//! [63]
void DM1Protocol::encodeSetEndEffectorGripper(PacketPayload &payload, QJsonObject params)
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

void DM1Protocol::encodeGetEndEffectorGripper(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 63;
    payload.p_ctrl.c_rw = 0;
}

//! [70]
void DM1Protocol::encodeSetJOGJointParams(PacketPayload &payload, QJsonObject params)
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

void DM1Protocol::encodeGetJOGJointParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 70;
    payload.p_ctrl.c_rw = 0;
}

//! [71]
void DM1Protocol::encodeSetJOGCoordinateParams(PacketPayload &payload, QJsonObject params)
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

void DM1Protocol::encodeGetJOGCoordinateParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 71;
    payload.p_ctrl.c_rw = 0;
}

//! [72]
void DM1Protocol::encodeSetJOGCommonParams(PacketPayload &payload, QJsonObject params)
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

void DM1Protocol::encodeGetJOGCommonParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 72;
    payload.p_ctrl.c_rw = 0;
}

//! [73]
void DM1Protocol::encodeSetJOGCmd(PacketPayload &payload, QJsonObject params)
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

//! [75]
void DM1Protocol::encodeSetInchMode(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 75;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "mode");

    quint8 inchMode = static_cast<quint8>(params.value("mode").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&inchMode), sizeof (quint8));
}

void DM1Protocol::encodeGetInchMode(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 75;
    payload.p_ctrl.c_rw = 0;
}

//! [76]
void DM1Protocol::encodeSetInchParam(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 76;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "distanceMM");
    checkFloatValue(params, "distanceANG");

    InchParamsStruct inchParams;
    inchParams.distanceMM = static_cast<float>(params.value("distanceMM").toDouble());
    inchParams.distanceANG = static_cast<float>(params.value("distanceANG").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&inchParams), sizeof (InchParamsStruct));
}

void DM1Protocol::encodeGetInchParam(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 76;
    payload.p_ctrl.c_rw = 0;
}

//! [80]
void DM1Protocol::encodeSetPTPJointParams(PacketPayload &payload, QJsonObject params)
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

void DM1Protocol::encodeGetPTPJointParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 80;
    payload.p_ctrl.c_rw = 0;
}

//! [81]
void DM1Protocol::encodeSetPTPCoordinateParams(PacketPayload &payload, QJsonObject params)
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

void DM1Protocol::encodeGetPTPCoordinateParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 81;
    payload.p_ctrl.c_rw = 0;
}

//! [82]
void DM1Protocol::encodeSetPTPJumpParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 82;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "jumpHeight");
    checkFloatValue(params, "zLimit");

    if (!params.contains("isUsingZLimit")) {
        qDebug().noquote() << "'isUsingZLimit':missing, use (true)";
    } else if (!params.value("isUsingZLimit").isBool()) {
        qWarning().noquote() << "'isUsingZLimit':type error";
    }

    float jumpHeight = static_cast<float>(params.value("jumpHeight").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&jumpHeight), sizeof (float));

    float zLimit = static_cast<float>(params.value("zLimit").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&zLimit), sizeof (float));

    quint32 isUsingZLimit = params.value("isUsingZLimit").toBool(true) ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&isUsingZLimit), sizeof (quint32));
}

void DM1Protocol::encodeGetPTPJumpParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 82;
    payload.p_ctrl.c_rw = 0;
}

//! [83]
void DM1Protocol::encodeSetPTPCommonParams(PacketPayload &payload, QJsonObject params)
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

void DM1Protocol::encodeGetPTPCommonParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 83;
    payload.p_ctrl.c_rw = 0;
}

//! [84]
void DM1Protocol::encodeSetPTPCmd(PacketPayload &payload, QJsonObject params)
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
    ptpCmd.coord.x = static_cast<float>(params.value("x").toDouble());
    ptpCmd.coord.y = static_cast<float>(params.value("y").toDouble());
    ptpCmd.coord.z = static_cast<float>(params.value("z").toDouble());
    ptpCmd.coord.r = static_cast<float>(params.value("r").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&ptpCmd), sizeof (PTPCmdStruct));
}

//! [88]
void DM1Protocol::encodeSetPTPPOCmd(PacketPayload &payload, QJsonObject params)
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
    ptpCmd.coord.x = static_cast<float>(ptpCmdObj.value("x").toDouble());
    ptpCmd.coord.y = static_cast<float>(ptpCmdObj.value("y").toDouble());
    ptpCmd.coord.z = static_cast<float>(ptpCmdObj.value("z").toDouble());
    ptpCmd.coord.r = static_cast<float>(ptpCmdObj.value("r").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&ptpCmd), sizeof (PTPCmdStruct));

    quint8 count = static_cast<quint8>(poCmdArray.count());
    payload.p_params.append(reinterpret_cast<char *>(&count), sizeof (quint8));

    for (int i = 0; i < poCmdArray.count(); i++) {
        QJsonObject poCmdObj = poCmdArray.at(i).toObject();

        ParallelOutputStruct pOut;
        pOut.ratio = static_cast<quint8>(poCmdObj.value("ratio").toInt());
        pOut.port = static_cast<quint16>(poCmdObj.value("port").toInt());
        pOut.level = static_cast<quint8>(poCmdObj.value("level").toInt());

        payload.p_params.append(reinterpret_cast<char *>(&pOut), sizeof (ParallelOutputStruct));
    }
}

//! [90]
void DM1Protocol::encodeSetCPParams(PacketPayload &payload, QJsonObject params)
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
        cpParams.realTimeTrack = 1;
        cpParams.period = static_cast<float>(params.value("period").toDouble());
    } else {
        cpParams.realTimeTrack = 0;
        cpParams.acc = static_cast<float>(params.value("acc").toDouble());
    }

    payload.p_params.append(reinterpret_cast<char *>(&cpParams), sizeof (CPParamsStruct));
}

void DM1Protocol::encodeGetCPParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 90;
    payload.p_ctrl.c_rw = 0;
}

//! [91]
void DM1Protocol::encodeSetCPCmd(PacketPayload &payload, QJsonObject params)
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
void DM1Protocol::encodeSetCPLECmd(PacketPayload &payload, QJsonObject params)
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

//! [100]
void DM1Protocol::encodeSetARCParams(PacketPayload &payload, QJsonObject params)
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

void DM1Protocol::encodeGetARCParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 100;
    payload.p_ctrl.c_rw = 0;
}

//! [101]
void DM1Protocol::encodeSetARCCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 101;
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
void DM1Protocol::encodeSetCircleCmd(PacketPayload &payload, QJsonObject params)
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
void DM1Protocol::encodeSetMotivateCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 103;
    payload.p_ctrl.c_rw = 1;

    foreach (const QString &v, QStringList() << "q1" << "q2" << "dq1" << "dq2"<< "ddq1"<< "ddq2") {
        if (!params.contains(v)) {
            qDebug() << QString("'%1':missing, use (0.0)").arg(v);
        } else if (!params.value(v).isDouble()) {
            qWarning() << QString("'%1':type error").arg(v);
        }
    }

    float q1 = static_cast<float>(params.value("q1").toDouble());
    float q2 = static_cast<float>(params.value("q2").toDouble());
    float dq1 = static_cast<float>(params.value("dq1").toDouble());
    float dq2 = static_cast<float>(params.value("dq2").toDouble());
    float ddq1 = static_cast<float>(params.value("ddq1").toDouble());
    float ddq2 = static_cast<float>(params.value("ddq2").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&q1), sizeof (float));
    payload.p_params.append(reinterpret_cast<char *>(&q2), sizeof (float));
    payload.p_params.append(reinterpret_cast<char *>(&dq1), sizeof (float));
    payload.p_params.append(reinterpret_cast<char *>(&dq2), sizeof (float));
    payload.p_params.append(reinterpret_cast<char *>(&ddq1), sizeof (float));
    payload.p_params.append(reinterpret_cast<char *>(&ddq2), sizeof (float));
}

//! [104]
void DM1Protocol::encodeSetMotivateZCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 104;
    payload.p_ctrl.c_rw = 1;

    foreach (const QString &v, QStringList() << "qz" << "dqz" << "ddqz") {
        if (!params.contains(v)) {
            qDebug() << QString("'%1':missing, use (0.0)").arg(v);
        } else if (!params.value(v).isDouble()) {
            qWarning() << QString("'%1':type error").arg(v);
        }
    }

    float qz = static_cast<float>(params.value("qz").toDouble());
    float dqz = static_cast<float>(params.value("dqz").toDouble());
    float ddqz = static_cast<float>(params.value("ddqz").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&qz), sizeof (float));
    payload.p_params.append(reinterpret_cast<char *>(&dqz), sizeof (float));
    payload.p_params.append(reinterpret_cast<char *>(&ddqz), sizeof (float));
}

//! [108]
void DM1Protocol::encodeSetARCPOCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 108;
    payload.p_ctrl.c_rw = 1;

    if (!params.contains("cirPoint")) {
        qWarning() << "'cirPoint':missing";
    }
    if (!params.contains("toPoint")) {
        qWarning() << "'toPoint':missing";
    }
    if (!params.contains("arcPO")) {
        qWarning() << "'arcpo':missing";
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

    QJsonArray arcPOArray = params.value("arcPO").toArray();
    quint8 count = static_cast<quint8>(arcPOArray.count());
    payload.p_params.append(reinterpret_cast<char *>(&count), sizeof (quint8));

    for (int i = 0; i < arcPOArray.count(); i++) {
        QJsonObject poObj = arcPOArray.at(i).toObject();

        ParallelOutputStruct pOut;
        pOut.ratio = static_cast<quint8>(poObj.value("ratio").toInt());
        pOut.port = static_cast<quint16>(poObj.value("port").toInt());
        pOut.level = static_cast<quint8>(poObj.value("level").toInt());

        payload.p_params.append(reinterpret_cast<char *>(&pOut), sizeof (ParallelOutputStruct));
    }
}

//! [109]
void DM1Protocol::encodeSetCirclePOCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 109;
    payload.p_ctrl.c_rw = 1;

    if (!params.contains("cirPoint")) {
        qWarning() << "'cirPoint':missing";
    }
    if (!params.contains("toPoint")) {
        qWarning() << "'toPoint':missing";
    }
    if (!params.contains("circlePO")) {
        qWarning() << "'arcpo':missing";
    }

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

    QJsonArray circlePO = params.value("circlePO").toArray();
    quint8 count = static_cast<quint8>(circlePO.count());
    payload.p_params.append(reinterpret_cast<char *>(&count), sizeof (quint8));

    for (int i = 0; i < circlePO.count(); i++) {
        QJsonObject poObj = circlePO.at(i).toObject();

        ParallelOutputStruct pOut;
        pOut.ratio = static_cast<quint8>(poObj.value("ratio").toInt());
        pOut.port = static_cast<quint16>(poObj.value("port").toInt());
        pOut.level = static_cast<quint8>(poObj.value("level").toInt());

        payload.p_params.append(reinterpret_cast<char *>(&pOut), sizeof (ParallelOutputStruct));
    }
}

//! [110]
void DM1Protocol::encodeSetWAITCmd(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 110;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "delay");

    quint32 delayMs = static_cast<quint32>(params.value("delay").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&delayMs), sizeof (quint32));
}

//! [120]
void DM1Protocol::encodeSetTRIGCmd(PacketPayload &payload, QJsonObject params)
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

//! [131]
void DM1Protocol::encodeSetIODO(PacketPayload &payload, QJsonObject params)
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

void DM1Protocol::encodeGetIODO(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 131;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [133]
void DM1Protocol::encodeGetIODI(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 133;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

//! [134]
void DM1Protocol::encodeGetIOADC(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 134;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "port");

    quint8 port = static_cast<quint8>(params.value("port").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&port), sizeof (quint8));
}

void DM1Protocol::encodeSetIODOs(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 136;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "address");
    checkIntValue(params, "number");

    checkStringValue(params, "value");

    quint8 number = static_cast<quint8>(params.value("number").toInt());
    quint8 address = static_cast<quint8>(params.value("address").toInt());

    payload.p_params.append(reinterpret_cast<char *>(&address), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&number), sizeof (quint8));

    char *c = params.value("value").toString().toLatin1().data();
    for (quint64 i = 0; i < strlen(c); i++) {
        char t = c[i];
        quint8 value = static_cast<quint8>(t-'0');
        payload.p_params.append(reinterpret_cast<char *>(&value), sizeof (quint8));
    }
}

void DM1Protocol::encodeGetIODOs(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 136;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "address");
    checkIntValue(params, "number");

    quint8 number = static_cast<quint8>(params.value("number").toInt());
    quint8 address = static_cast<quint8>(params.value("address").toInt());

    payload.p_params.append(reinterpret_cast<char *>(&address), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&number), sizeof (quint8));
}

void DM1Protocol::encodeGetIODIs(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 137;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "address");
    checkIntValue(params, "number");

    quint8 number = static_cast<quint8>(params.value("number").toInt());
    quint8 address = static_cast<quint8>(params.value("address").toInt());

    payload.p_params.append(reinterpret_cast<char *>(&address), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&number), sizeof (quint8));
}

void DM1Protocol::encodeGetIOADCs(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 138;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "address");
    checkIntValue(params, "number");

    quint8 number = static_cast<quint8>(params.value("number").toInt());
    quint8 address = static_cast<quint8>(params.value("address").toInt());

    payload.p_params.append(reinterpret_cast<char *>(&address), sizeof (quint8));
    payload.p_params.append(reinterpret_cast<char *>(&number), sizeof (quint8));
}

//! [141]
void DM1Protocol::encodeSetRebootEncoderICPVL(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 141;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "jointNumber");

    float jointNumber = static_cast<float>(params.value("jointNumber").toDouble());
    float encode = 324.0;

    payload.p_params.append(reinterpret_cast<char *>(&encode), sizeof (float));
    payload.p_params.append(reinterpret_cast<char *>(&jointNumber), sizeof (float));
}

//! [161]
void DM1Protocol::encodeSetFirmwareMode(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 161;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "firmwareMode");

    quint8 firmwareMode = static_cast<quint8>(params.value("firmwareMode").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&firmwareMode), sizeof (quint8));

    quint8 ctrl = 1;
    payload.p_params.append(reinterpret_cast<char *>(&ctrl), sizeof (quint8));
}

void DM1Protocol::encodeGetFirmwareMode(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 161;
    payload.p_ctrl.c_rw = 0;

    quint8 firmwareMode = 0;
    payload.p_params.append(reinterpret_cast<char *>(&firmwareMode), sizeof (quint8));

    quint8 ctrl = 0;
    payload.p_params.append(reinterpret_cast<char *>(&ctrl), sizeof (quint8));

}

//! [166]
void DM1Protocol::encodeSetFirmwareReboot(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 166;
    payload.p_ctrl.c_rw = 1;

    quint8 firm = 1;
    payload.p_params.append(reinterpret_cast<char *>(&firm), sizeof (quint8));
}

//! [167]
void DM1Protocol::encodeSetLanPortConfig(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 167;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "isdhcp");
    checkStringValue(params, "addr", "0.0.0.0");
    checkStringValue(params, "mask", "255.255.255.0");
    checkStringValue(params, "gateway");
    checkStringValue(params, "dns");

    quint8 status = static_cast<quint8>(params.value("status").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&status), sizeof (quint8));

    quint8 isdhcp = static_cast<quint8>(params.value("isdhcp").toBool() ? 1 : 0);
    payload.p_params.append(reinterpret_cast<char *>(&isdhcp), sizeof (quint8));

    QByteArray addr = params.value("addr").toString("0.0.0.0").toLatin1() + '\0';
    addr.resize(16);
    payload.p_params.append(addr);

    QByteArray mask = params.value("mask").toString("255.255.255.0").toLatin1() + '\0';
    mask.resize(16);
    payload.p_params.append(mask);

    QByteArray gateway = params.value("gateway").toString().toLatin1() + '\0';
    gateway.resize(16);
    payload.p_params.append(gateway);

    QByteArray dns = params.value("dns").toString().toLatin1() + '\0';
    dns.resize(16);
    payload.p_params.append(dns);
}

void DM1Protocol::encodeGetLanPortConfig(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 167;
    payload.p_ctrl.c_rw = 0;
}

//! [168]
void DM1Protocol::encodeSetFirmwareNotifyM4Mode(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 168;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "mode");

    quint8 mode = static_cast<quint8>(params.value("mode").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&mode), sizeof (quint8));
}

void DM1Protocol::encodeGetFirmwareNotifyM4Mode(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 168;
    payload.p_ctrl.c_rw = 0;
}

//! [200]
void DM1Protocol::encodeSetSafeModeEnabled(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 200;
    payload.p_ctrl.c_rw = 1;

    checkBoolValue(params, "enable");

    quint8 enable = params.value("enable").toBool() ? 1 : 0;
    payload.p_params.append(reinterpret_cast<char *>(&enable), sizeof (quint8));
}

void DM1Protocol::encodeGetSafeModeEnabled(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 200;
    payload.p_ctrl.c_rw = 0;
}

//! [201]
void DM1Protocol::encodeSetCollisionThreshold(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 201;
    payload.p_ctrl.c_rw = 1;

    QStringList list;
    list << "torDiffJ1" << "torDiffJ2" << "torDiffJ3" << "torDiffJ4";

    foreach (const QString &v, list) {
        if (!params.contains(v)) {
            qDebug() << QString("'%1':missing, use (0.0)").arg(v);
        } else if (!params.value(v).isDouble()) {
            qWarning() << QString("'%1':type error").arg(v);
        }
    }

    TorDiffParamsStruct torDiffParams;
    torDiffParams.torDiffJ1 = static_cast<float>(params.value("torDiffJ1").toDouble());
    torDiffParams.torDiffJ2 = static_cast<float>(params.value("torDiffJ2").toDouble());
    torDiffParams.torDiffJ3 = static_cast<float>(params.value("torDiffJ3").toDouble());
    torDiffParams.torDiffJ4 = static_cast<float>(params.value("torDiffJ4").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&torDiffParams), sizeof (TorDiffParamsStruct));
}

void DM1Protocol::encodeGetCollisionThreshold(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 201;
    payload.p_ctrl.c_rw = 0;
}

//! [202]
void DM1Protocol::encodeSetBasicDynamicParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 202;
    payload.p_ctrl.c_rw = 1;

    QStringList list;
    list << "ZZ1" << "FS1" << "FV1" << "ZZ2" << "MX2"<< "MY2" << "IA2" << "FS2" << "FV2";

    foreach (const QString &v, list) {
        if (!params.contains(v)) {
            qDebug() << QString("'%1':missing, use (0.0)").arg(v);
        } else if (!params.value(v).isDouble()) {
            qWarning() << QString("'%1':type error").arg(v);
        }
    }

    BasicDynamicParamsStruct data;
    data.ZZ1 = static_cast<float>(params.value("ZZ1").toDouble());
    data.FS1 = static_cast<float>(params.value("FS1").toDouble());
    data.FV1 = static_cast<float>(params.value("FV1").toDouble());
    data.ZZ2 = static_cast<float>(params.value("ZZ2").toDouble());
    data.MX2 = static_cast<float>(params.value("MX2").toDouble());
    data.MY2 = static_cast<float>(params.value("MY2").toDouble());
    data.IA2 = static_cast<float>(params.value("IA2").toDouble());
    data.FS2 = static_cast<float>(params.value("FS2").toDouble());
    data.FV2 = static_cast<float>(params.value("FV2").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&data), sizeof (BasicDynamicParamsStruct));
}

void DM1Protocol::encodeGetBasicDynamicParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 202;
    payload.p_ctrl.c_rw = 0;
}

//! [203]
void DM1Protocol::encodeSetLoadParams(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 203;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "loadParams");

    float loadParams = static_cast<float>(params.value("loadParams").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&loadParams), sizeof (float));
}

void DM1Protocol::encodeGetLoadParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 203;
    payload.p_ctrl.c_rw = 0;
}

//! [204]
void DM1Protocol::encodeSetFeedforward(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 204;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "value");

    quint8 value = static_cast<quint8>(params.value("value").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&value), sizeof (quint8));
}

void DM1Protocol::encodeGetFeedforward(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 204;
    payload.p_ctrl.c_rw = 0;
}

//! [205]
void DM1Protocol::encodeSetMotivationMode(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 205;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "mode");

    quint8 mode = static_cast<quint8>(params.value("mode").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&mode), sizeof (quint8));
}

void DM1Protocol::encodeGetMotivationMode(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 205;
    payload.p_ctrl.c_rw = 0;
}

//! [206]
void DM1Protocol::encodeGetTrajectory(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 206;
    payload.p_ctrl.c_rw = 0;

    checkIntValue(params, "countMax");
    checkIntValue(params, "index");

    quint8 countMax = static_cast<quint8>(params.value("countMax").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&countMax), sizeof (quint8));

    quint64 index = static_cast<quint64>(params.value("index").toDouble());
    payload.p_params.append(reinterpret_cast<char *>(&index), sizeof (quint64));
}

//! [208]
void DM1Protocol::encodeSetSafeStrategy(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 208;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "strategy");

    quint8 strategy = static_cast<quint8>(params.value("strategy").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&strategy), sizeof (quint8));
}

void DM1Protocol::encodeGetSafeStrategy(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 208;
    payload.p_ctrl.c_rw = 0;
}

//! [210]
void DM1Protocol::encodeSetSafeGuardMode(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 210;
    payload.p_ctrl.c_rw = 1;

    checkIntValue(params, "mode");

    quint8 safeGuardMode = static_cast<quint8>(params.value("mode").toInt());
    payload.p_params.append(reinterpret_cast<char *>(&safeGuardMode), sizeof (quint8));
}

void DM1Protocol::encodeGetSafeGuardMode(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 210;
    payload.p_ctrl.c_rw = 0;
}

//! [211]
void DM1Protocol::encodeGetSafeGuardStatus(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 211;
    payload.p_ctrl.c_rw = 0;
}

void DM1Protocol::encodeGetUserParams(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 220;
    payload.p_ctrl.c_rw = 0;
}

//! [240]
void DM1Protocol::encodeSetQueuedCmdStart(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 240;
    payload.p_ctrl.c_rw = 1;
}

//! [241]
void DM1Protocol::encodeSetQueuedCmdStop(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 241;
    payload.p_ctrl.c_rw = 1;
}

//! [242]
void DM1Protocol::encodeSetQueuedCmdForceStop(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 242;
    payload.p_ctrl.c_rw = 1;
}

//! [243]
void DM1Protocol::encodeSetQueuedCmdStartDownload(PacketPayload &payload, QJsonObject params)
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
void DM1Protocol::encodeSetQueuedCmdStopDownload(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 244;
    payload.p_ctrl.c_rw = 1;
}

//! [245]
void DM1Protocol::encodeSetQueuedCmdClear(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 245;
    payload.p_ctrl.c_rw = 1;
}

//! [246]
void DM1Protocol::encodeGetQueuedCmdCurrentIndex(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 246;
    payload.p_ctrl.c_rw = 0;
}

//! [247]
void DM1Protocol::encodeGetQueuedCmdLeftSpace(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 247;
    payload.p_ctrl.c_rw = 0;
}

void DM1Protocol::encodeSetUserCoordinate(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 250;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "x");
    checkFloatValue(params, "y");
    checkFloatValue(params, "z");
    checkFloatValue(params, "r");

    PositionStruct coord;
    coord.x = static_cast<float>(params.value("x").toDouble());
    coord.y = static_cast<float>(params.value("y").toDouble());
    coord.z = static_cast<float>(params.value("z").toDouble());
    coord.r = static_cast<float>(params.value("r").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&coord), sizeof (PositionStruct));
}

void DM1Protocol::encodeGetUserCoordinate(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 250;
    payload.p_ctrl.c_rw = 0;
}

void DM1Protocol::encodeSetToolCoordinate(PacketPayload &payload, QJsonObject params)
{
    payload.p_cmdID = 251;
    payload.p_ctrl.c_rw = 1;

    checkFloatValue(params, "x");
    checkFloatValue(params, "y");
    checkFloatValue(params, "z");
    checkFloatValue(params, "r");

    PositionStruct coord;
    coord.x = static_cast<float>(params.value("x").toDouble());
    coord.y = static_cast<float>(params.value("y").toDouble());
    coord.z = static_cast<float>(params.value("z").toDouble());
    coord.r = static_cast<float>(params.value("r").toDouble());

    payload.p_params.append(reinterpret_cast<char *>(&coord), sizeof (PositionStruct));
}

void DM1Protocol::encodeGetToolCoordinate(PacketPayload &payload, QJsonObject params)
{
    Q_UNUSED(params)

    payload.p_cmdID = 251;
    payload.p_ctrl.c_rw = 0;
}


