#include "M1Plugin.h"

#include <QDebug>

const QString M1Plugin::PluginName = "M1";
const QString M1Plugin::Version = "1.0.1";

M1Plugin::M1Plugin(QObject *parent) : DPluginInterface(parent)
{
    m_controller = new M1Controller(this);
    connect(m_controller, &M1Controller::deviceDisconnected_signal,
            this, &M1Plugin::handleDeviceDisconnected_slot);
    connect(this, &M1Plugin::closeAllDevice_signal,
            m_controller, &M1Controller::closeAllDevice_slot);
    connect(m_controller, &M1Controller::sendFinishPacket_signal,
            this, &M1Plugin::sendResPacket_slot);
}

QString M1Plugin::getVersion()
{
    return Version;
}

/* 收到消息 */
void M1Plugin::pReceiveMassage_slot(QString id, QJsonObject obj)
{
    if (id == "ALL") {
        handleDobotLinkCommand(obj);
    } else if (id == PluginName) {
        handleM1Command(obj);
    }
}

/* [!!!消息分发!!!] */
void M1Plugin::handleDobotLinkCommand(const QJsonObject &obj)
{
    qDebug() << "[ALL] {M1} get obj" << obj;
    if (obj.contains("METHOD")) {
        QString method = obj.value("METHOD").toString();
        if (method == "EXIT") {
            emit closeAllDevice_signal();
        } else if (method == "CloseWebSocket") {

            QJsonObject params = obj.value("params").toObject();
            quint16 port = static_cast<quint16>(params.value("WSport").toInt(0));
            m_controller->disConnectDevices(port);
        }
    }
}

void M1Plugin::handleM1Command(const QJsonObject &obj)
{
//    qDebug() << "M1Plugin" << obj;
    M1Packet packet;
    packet.setPacket(obj);

    if (checkPacket(packet) == false) {
        return;
    }

    if (packet.api.contains("GetPose")) {
        handleGetPoseCmd(packet);
    } else if (packet.api.endsWith("Cmd")) {         //execute command
        handleActionCmd(packet);
    } else if (packet.api.endsWith("State")) {       //get/clear state
        handleStateCmd(packet);
    } else if (packet.api.endsWith("Params")) {      //set/get params
        handleParamsCmd(packet);
    } else if (packet.api.endsWith("Dobot")) {       //search & dis/connect
        handleDobotCmd(packet);
    } else if (packet.api.contains("IO")) {          //IO control
        handleIOCmd(packet);
    } else if (packet.api.contains("Device")) {      //Device
        handleDeviceCmd(packet);
    } else if (packet.api.endsWith("Sensor")) {      //Sensor
        handleSensorCmd(packet);
    } else if (packet.api.contains("HHTTrig")) {     //HHTTrig
        handleHHTTrigCmd(packet);
    } else if (packet.api.contains("Firmware")) {     //HHTTrig
        handleFirmwareCmd(packet);
    }  else {
        handleOtherCmd(packet);
    }
}

bool M1Plugin::checkPacket(const M1Packet &packet)
{
    if (packet.api == "SearchDobot") {
        return true;
    }

    if (packet.portName.isEmpty()) {

        M1ResPacket resPacket(packet.id, packet.port);
        resPacket.setErrorObj(10, "PortName was not specified.");
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
        return false;
    }
    return true;
}

//![1] GetPose
void M1Plugin::handleGetPoseCmd(const M1Packet &packet)
{  
    QJsonObject resObj;

    if (packet.api == "GetPose") {
        resObj = m_controller->pGetPose(packet);
    }
    else {
        M1ResPacket resPacket(packet.id, packet.port);
        resPacket.setErrorObj(82, "can not handle api with : GetPose");
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    }

    if (!resObj.isEmpty()) {
        emit pSendMessage_signal(PluginName, resObj);
    }
}

//![2] execute Cmd
void M1Plugin::handleActionCmd(const M1Packet &packet)
{
    QJsonObject resObj;

    if (packet.api == "SetHOMECmd") {
        resObj = m_controller->pSetHOMECmd(packet);
    } else if (packet.api == "SetPTPCmd") {
        resObj = m_controller->pSetPTPCmd(packet);
    } else if (packet.api == "SetPTPPOCmd") {
        resObj = m_controller->pSetPTPPOCmd(packet);
//    } else if (packet.api == "SetPTPWithLCmd") {
//        resObj = m_controller->pSetPTPWithLCmd(packet);
//    } else if (packet.api == "SetPTPPOWithLCmd") {
//        resObj = m_controller->pSetPTPPOWithLCmd(packet);
    } else if (packet.api == "SetJOGCmd") {
        resObj = m_controller->pSetJOGCmd(packet);
    } else if (packet.api == "SetCPCmd") {
        resObj = m_controller->pSetCPCmd(packet);
    } else if (packet.api == "SetCPLECmd") {
        resObj = m_controller->pSetCPLECmd(packet);
    } else if (packet.api == "SetQueuedCmd") {
        resObj = m_controller->pSetQueuedCmd(packet);
    } else if (packet.api == "SetARCCmd") {
        resObj = m_controller->pSetARCCmd(packet);
    } else if (packet.api == "SetCircleCmd") {
        resObj = m_controller->pSetCircleCmd(packet);
    } else if (packet.api == "SetExcitCmd") {
        resObj = m_controller->pSetExcitCmd(packet);
    } else if(packet.api == "SetCmdTimeout") {
        resObj = m_controller->pSetCmdTimeout(packet);
    } else if (packet.api == "SetTRIGCmd") {
        resObj = m_controller->pSetTRIGCmd(packet);
    } else if (packet.api == "SetWAITCmd") {
        resObj = m_controller->pSetWAITCmd(packet);
    } else {
        qDebug() << "can not handle api with : Cmd";
        M1ResPacket resPacket(packet.id, packet.port);
        resPacket.setErrorObj(82, QString("can not handle api with : %1").arg(packet.api));
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    }

    if (!resObj.isEmpty()) {
        resObj.insert("id", static_cast<double>(packet.id));
        emit pSendMessage_signal(PluginName, resObj);
    }
}

//![3] get/clear State
void M1Plugin::handleStateCmd(const M1Packet &packet)
{
    QJsonObject resObj;

    if (packet.api == "GetAlarmsState") {
        resObj = m_controller->pGetAlarmsState(packet);
    } else if (packet.api == "ClearAllAlarmsState") {
        resObj = m_controller->pClearAllAlarmsState(packet);
    } else {
        qDebug() << "can not handle api with : State";
        M1ResPacket resPacket(packet.id, packet.port);
        resPacket.setErrorObj(82, QString("can not handle api with : %1").arg(packet.api));
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    }

    if (!resObj.isEmpty()) {
        resObj.insert("id", static_cast<double>(packet.id));
        emit pSendMessage_signal(PluginName, resObj);
    }
}

//![4] set/get Params
void M1Plugin::handleParamsCmd(const M1Packet &packet)
{
    QJsonObject resObj;

    if (packet.api == "SetHOMEParams") {
        resObj = m_controller->pSetHOMEParams(packet);
    } else if (packet.api == "GetHomeParams") {
        resObj = m_controller->pGetHOMEParams(packet);
    } else if (packet.api == "SetEndEffectorParams") {
        resObj = m_controller->pSetEndEffectorParams(packet);
    } else if (packet.api == "GetEndEffectorParams") {
        resObj = m_controller->pGetEndEffectorParams(packet);
    } else if (packet.api == "SetPTPJointParams") {
        resObj = m_controller->pSetPTPJointParams(packet);
    } else if (packet.api == "GetPTPJointParams") {
        resObj = m_controller->pGetPTPJointParams(packet);
    } else if (packet.api == "SetJOGJointParams") {
        resObj = m_controller->pSetJOGJointParams(packet);
    } else if (packet.api == "GetJOGJointParams") {
        resObj = m_controller->pGetJOGJointParams(packet);
    } else if (packet.api == "SetJOGCoordinateParams") {
        resObj = m_controller->pSetJOGCoordinateParams(packet);
    } else if (packet.api == "GetJOGCoordinateParams") {
        resObj = m_controller->pGetJOGCoordinateParams(packet);
    } else if (packet.api == "SetJOGLParams") {
        resObj = m_controller->pSetJOGLParams(packet);
    } else if (packet.api == "GetJOGLParams") {
        resObj = m_controller->pGetJOGLParams(packet);
    } else if (packet.api == "SetJOGCommonParams") {
        resObj = m_controller->pSetJOGCommonParams(packet);
    } else if (packet.api == "GetJOGCommonParams") {
        resObj = m_controller->pGetJOGCommonParams(packet);
//    } else if (packet.api == "SetPTPLParams") {
//        resObj = m_controller->pSetPTPLParams(packet);
//    } else if (packet.api == "GetPTPLParams") {
//        resObj = m_controller->pGetPTPLParams(packet);
    } else if (packet.api == "SetCPParams") {
        resObj = m_controller->pSetCPParams(packet);
    } else if (packet.api == "GetCPParams") {
        resObj = m_controller->pGetCPParams(packet);
    } else if (packet.api == "SetCPParams") {
        resObj = m_controller->pSetCPParams(packet);
    } else if (packet.api == "GetCPParams") {
        resObj = m_controller->pGetCPParams(packet);
    } else if (packet.api == "SetPTPCommonParams") {
        resObj = m_controller->pSetPTPCommonParams(packet);
    } else if (packet.api == "GetPTPCommonParams") {
        resObj = m_controller->pGetPTPCommonParams(packet);
    } else if (packet.api == "SetPTPJumpParams") {
        resObj = m_controller->pSetPTPJumpParams(packet);
    } else if (packet.api == "GetPTPJumpParams") {
        resObj = m_controller->pGetPTPJumpParams(packet);
    } else if (packet.api == "SetPTPCoordinateParams") {
        resObj = m_controller->pSetPTPCoordinateParams(packet);
    } else if (packet.api == "SetARCParams") {
        resObj = m_controller->pSetARCParams(packet);
    } else if (packet.api == "GetARCParams") {
        resObj = m_controller->pGetARCParams(packet);
    } else if (packet.api == "SetBasicDynamicParams") {
        resObj = m_controller->pSetBasicDynamicParams(packet);
    } else if (packet.api == "GetBasicDynamicParams") {
        resObj = m_controller->pGetBasicDynamicParams(packet);
    } else if (packet.api == "GetPTPCoordinateParams") {
        resObj = m_controller->pGetPTPCoordinateParams(packet);
    } else {
        qDebug() << "can not handle api with : Params";
        M1ResPacket resPacket(packet.id, packet.port);
        resPacket.setErrorObj(82, QString("can not handle api with : %1").arg(packet.api));
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    }

    if (!resObj.isEmpty()) {
        resObj.insert("id", static_cast<double>(packet.id));
        emit pSendMessage_signal(PluginName, resObj);
    }
}

//![5] search & dis/connect
void M1Plugin::handleDobotCmd(const M1Packet &packet)
{
    QJsonObject resObj;

    if (packet.api == "SearchDobot") {
        resObj = m_controller->pSearchDobot(packet);
    } else if (packet.api == "ConnectDobot") {
        resObj = m_controller->pConnectDobot(packet);
    } else if (packet.api == "DisconnectDobot") {
        resObj = m_controller->pDisConnectDobot(packet);
    } else {
        qDebug() << "can not handle api with : Dobot";
        M1ResPacket resPacket(packet.id, packet.port);
        resPacket.setErrorObj(82, QString("can not handle api with : %1").arg(packet.api));
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    }

    if (!resObj.isEmpty()) {
        resObj.insert("id", static_cast<double>(packet.id));
        emit pSendMessage_signal(PluginName, resObj);
    }
}

//![6] IO Cmd
void M1Plugin::handleIOCmd(const M1Packet &packet)
{
    QJsonObject resObj;

//    if (packet.api == "SetIOMultiplexing") {
//        resObj = m_controller->pSetIOMultiplexing(packet);
//    } else if (packet.api == "GetIOMultiplexing") {
//        resObj = m_controller->pGetIOMultiplexing(packet);
//    } else
    if (packet.api == "SetIODO") {
        resObj = m_controller->pSetIODO(packet);
    } else if (packet.api == "GetIODO") {
        resObj = m_controller->pGetIODO(packet);
    } else if (packet.api == "GetIODI") {
        resObj = m_controller->pGetIODI(packet);
    } else if (packet.api == "GetIOADC") {
        resObj = m_controller->pGetIOADC(packet);
    } else {
        qDebug() << "can not handle api with : IO";
        M1ResPacket resPacket(packet.id, packet.port);
        resPacket.setErrorObj(82, QString("can not handle api with : %1").arg(packet.api));
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    }

    if (!resObj.isEmpty()) {
        resObj.insert("id", static_cast<double>(packet.id));
        emit pSendMessage_signal(PluginName, resObj);
    }
}

//![7] Device Cmd
void M1Plugin::handleDeviceCmd(const M1Packet &packet)
{
    QJsonObject resObj;

    if (packet.api == "SetDeviceName") {
        resObj = m_controller->pSetDeviceName(packet);
    } else if (packet.api == "GetDeviceSN") {
        resObj = m_controller->pGetDeviceSN(packet);
    } else if (packet.api == "SetDeviceSN") {
        resObj = m_controller->pSetDeviceSN(packet);
    } else if (packet.api == "GetDeviceName") {
        resObj = m_controller->pGetDeviceName(packet);
    } else if (packet.api == "SetDeviceWithL") {
        resObj = m_controller->pSetDeviceWithL(packet);
    } else if (packet.api == "GetDeviceWithL") {
        resObj = m_controller->pGetDeviceWithL(packet);
    } else if (packet.api == "GetDeviceVersion") {
        resObj = m_controller->pGetDeviceVersion(packet);
    } else {
        qDebug() << "can not handle api with : Device";
        M1ResPacket resPacket(packet.id, packet.port);
        resPacket.setErrorObj(82, QString("can not handle api with : %1").arg(packet.api));
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    }

    if (!resObj.isEmpty()) {
        resObj.insert("id", static_cast<double>(packet.id));
        emit pSendMessage_signal(PluginName, resObj);
    }
}

//![8] Sensor Cmd
void M1Plugin::handleSensorCmd(const M1Packet &packet)
{
    QJsonObject resObj;

    if (!resObj.isEmpty()) {
        resObj.insert("id", static_cast<double>(packet.id));
        emit pSendMessage_signal(PluginName, resObj);
    }
}

//![9] HHTTrig Cmd
void M1Plugin::handleHHTTrigCmd(const M1Packet &packet)
{
    QJsonObject resObj;

    if (packet.api == "SetHHTTrigMode") {
        resObj = m_controller->pSetHHTTrigMode(packet);
    } else if (packet.api == "GetHHTTrigMode") {
        resObj = m_controller->pGetHHTTrigMode(packet);
    } else if (packet.api == "GetHHTTrigOutput") {
        resObj = m_controller->pGetHHTTrigOutput(packet);
    } else if (packet.api == "SetHHTTrigOutputEnabled") {
        resObj = m_controller->pSetHHTTrigOutputEnabled(packet);
    } else if (packet.api == "GetHHTTrigOutputEnabled") {
        resObj = m_controller->pGetHHTTrigOutputEnabled(packet);
    } else {
        qDebug() << "can not handle api with : HHTTrig";
        M1ResPacket resPacket(packet.id, packet.port);
        resPacket.setErrorObj(82, QString("can not handle api with : %1").arg(packet.api));
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    }

    if (!resObj.isEmpty()) {
        resObj.insert("id", static_cast<double>(packet.id));
        emit pSendMessage_signal(PluginName, resObj);
    }
}

void M1Plugin::handleFirmwareCmd(const M1Packet &packet)
{
    QJsonObject resObj;

    if (packet.api == "SetFirmwareLanConfig") {
        resObj = m_controller->pSetFirmwareLanConfig(packet);
    } else if (packet.api == "GetFirmwareLanConfig") {
        resObj = m_controller->pGetFirmwareLanConfig(packet);
    } else if (packet.api == "FirmwareReboot") {
        resObj = m_controller->pFirmwareReboot(packet);
    }else {
        qDebug() << "can not handle api with : Firmware";
        M1ResPacket resPacket(packet.id, packet.port);
        resPacket.setErrorObj(82, QString("can not handle api with : %1").arg(packet.api));
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    }

    if (!resObj.isEmpty()) {
        resObj.insert("id", static_cast<double>(packet.id));
        emit pSendMessage_signal(PluginName, resObj);
    }
}

//![10] Other Cmd
void M1Plugin::handleOtherCmd(const M1Packet &packet)
{
    QJsonObject resObj;

    if (packet.api == "SetEndEffectorSuctionCup") {
        resObj = m_controller->pSetEndEffectorSuctionCup(packet);
    } else if (packet.api == "GetEndEffectorSuctionCup") {
        resObj = m_controller->pGetEndEffectorSuctionCup(packet);
    } else if (packet.api == "SetEndEffectorGripper") {
        resObj = m_controller->pSetEndEffectorGripper(packet);
    } else if (packet.api == "GetEndEffectorGripper") {
        resObj = m_controller->pGetEndEffectorGripper(packet);
    } else if (packet.api == "SetEndEffectorLaser") {
        resObj = m_controller->pSetEndEffectorLaser(packet);
    } else if (packet.api == "GetEndEffectorLaser") {
        resObj = m_controller->pGetEndEffectorLaser(packet);
    } else if (packet.api == "SetArmOrientation") {
        resObj = m_controller->pSetArmOrientation(packet);
    } else if (packet.api == "GetArmOrientation") {
        resObj = m_controller->pGetArmOrientation(packet);
    } else if (packet.api == "GetHardwareVersion") {
        resObj = m_controller->pGetHardwareVersion(packet);
    } else if (packet.api == "GetTrajectory") {
        resObj = m_controller->pGetTrajectory(packet);
    } else if (packet.api == "SetExcitMode") {
        resObj = m_controller->pSetExcitMode(packet);
    } else if (packet.api == "GetExcitMode") {
        resObj = m_controller->pGetExcitMode(packet);
    } else if(packet.api == "ResetPose") {
         resObj = m_controller->pResetPose(packet);
    } else if(packet.api == "SetSafeMode") {
         resObj = m_controller->pSetSafeMode(packet);
    } else if(packet.api == "GetSafeMode") {
         resObj = m_controller->pGetSafeMode(packet);
    } else if (packet.api == "GetQueuedCmdCurrentIndex") {
        resObj = m_controller->pGetQueuedCmdCurrentIndex(packet);
    } else if(packet.api == "QueuedCmdStart") {
         resObj = m_controller->pQueuedCmdStart(packet);
    } else if(packet.api == "QueuedCmdStop") {
         resObj = m_controller->pQueuedCmdStop(packet);
    } else if(packet.api == "QueuedCmdClear") {
         resObj = m_controller->pQueuedCmdClear(packet);
    } else if(packet.api == "SetCollisionThreshold") {
        resObj = m_controller->pSetCollisionThreshold(packet);
    } else if(packet.api == "GetCollisionThreshold") {
        resObj = m_controller->pGetCollisionThreshold(packet);
    } else if(packet.api == "SetPlayLoad") {
        resObj = m_controller->pSetPlayLoad(packet);
    } else if(packet.api == "GetPlayLoad") {
        resObj = m_controller->pGetPlayLoad(packet);
    } else if(packet.api == "GetCollisionCount") {
        resObj = m_controller->pGetCollisionCount(packet);
    } else if(packet.api == "SetSafeStrategy") {
        resObj = m_controller->pSetSafeStrategy(packet);
    } else if(packet.api == "GetSafeStrategy") {
        resObj = m_controller->pGetSafeStrategy(packet);
    } else if(packet.api == "SetSafeGuardMode") {
        resObj = m_controller->pSetSafeGuardMode(packet);
    } else if(packet.api == "GetSafeGuardMode") {
        resObj = m_controller->pGetSafeGuardMode(packet);
    } else if(packet.api == "SetSafeGuardStatus") {
        resObj = m_controller->pSetSafeGuardStatus(packet);
    } else if(packet.api == "GetSafeGuardStatus") {
        resObj = m_controller->pGetSafeGuardStatus(packet);
    } else if(packet.api == "SetQueuedCmdForceStopExec") {
        resObj = m_controller->pSetQueuedCmdForceStopExec(packet);
    } else if(packet.api == "SetQueuedCmdStartDownload") {
        resObj = m_controller->pSetQueuedCmdStartDownload(packet);
    } else if(packet.api == "SetQueuedCmdStopDownload") {
        resObj = m_controller->pSetQueuedCmdStopDownload(packet);
    } else if(packet.api == "GetQueuedCmdLeftSpace") {
        resObj = m_controller->pGetQueuedCmdLeftSpace(packet);
    } else {
        qDebug() << "can not handle this api :( , please contact the author." << packet.api;
        M1ResPacket resPacket(packet.id, packet.port);
        resPacket.setErrorObj(82, QString("can not handle api with : %1").arg(packet.api));
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    }

    if (!resObj.isEmpty()) {
        resObj.insert("id", static_cast<double>(packet.id));
        emit pSendMessage_signal(PluginName, resObj);
    }
}

/* SLOT */
void M1Plugin::handleDeviceDisconnected_slot(QString portName)
{
    M1ResPacket resPacket(0);
    resPacket.setErrorObj(99, "caution: Magician device disconnected.");
    resPacket.errorObj.insert("portName", portName);

    emit pSendMessage_signal(PluginName, resPacket.getResultObj());
}

void M1Plugin::sendResPacket_slot(QJsonObject resObj)
{
    emit pSendMessage_signal(PluginName, resObj);
}
