#include "DProtocol3.h"
#include <QDataStream>
#include <QJsonDocument>
#include "P3FrameAdapter.h"
#include "MagicianGoApi.h"
#include "GeneralApi.h"
#include "CameraApi.h"
#include "MagicBoxApi.h"
#include "CameraCalibration.h"
#include "DError/DError.h"
#include <QDebug>

bool DProtocol3::sendData(quint64 id, const QJsonObject &obj, bool needReply)
{
    Q_UNUSED(id);
    Q_UNUSED(needReply) //todo： 开放通讯异步功能

    P3Packet packet;
    packet.setPacketFromObj(obj);

    //search & dis/connect
    if (packet.api.endsWith("Port")) {
//        handlePortCommand(packet);
        // todo: 这是什么情况？没有信息返回？
        return false;
    } else {
        if(packet.api.contains("Device") || packet.api == "HeartBeat" ||  packet.api == "GetProductName"){
            handleGeneralCommand(packet);
        } else if(packet.api.contains("Camera")){
            handleCameraCommand(packet);
        } else if (packet.api.contains("MagicBox")) {
            handleMagicBoxCommand(packet);
        } else{
            handleMagicianGoCommand(packet);
        }
    }

    return true;
}

void  DProtocol3::stop()
{
    emit frameStop_signal();
}

void  DProtocol3::start()
{
    emit frameStart_signal();
}

void DProtocol3::setCommPars(quint8 resendTimesMax, quint32 commTimeout)
{
    emit frameSetCommPars_signal(resendTimesMax, commTimeout);
}

uint32_t DProtocol3::sm_seqNum = 0; // 给P3协议栈用，多设备共用，必须是静态

DProtocol3:: DProtocol3(QObject *parent) :
    DProtocolInterface(parent)
{
    QTimer::singleShot(1, MagicianGoApi::instance()->thread(), [&]() {
        // Plugin已经被PluginManager移动到其他线程
        // 不能切换主线程的单例的所属线程
        QThread *th = this->thread();
        MagicianGoApi::instance()->moveToThread(th);
        GeneralApi::instance()->moveToThread(th);
        CameraApi::instance()->moveToThread(th);
        CameraCalibration::instance()->moveToThread(th);
        MagicBoxApi::instance()->moveToThread(th);
    });

    connect(this, &DProtocol3::frameAddRawData_signal, P3FrameAdapter::instance(), &P3FrameAdapter::addRawData_slot);
    connect(this, &DProtocol3::frameStart_signal, [](){
        // 没有东西要启动的
    });
    connect(this, &DProtocol3::frameStop_signal, []{
        // 没有东西要停止的
    });
    connect(this, &DProtocol3::frameSetCommPars_signal, P3FrameAdapter::instance(), &P3FrameAdapter::setComPars_slot);
    connect(P3FrameAdapter::instance(), &P3FrameAdapter::sendRawData_signal, this, [&](quint32 seqNum, QByteArray data) {
        // 恶心的做法，小心处理，P3协议栈接口问题
        if (m_idPortSeqMap.contains(seqNum)) {
            emit sendRawData_signal(data);
            if(!m_idPortSeqMap.value(seqNum).isAck) {
                // 不需要应答的指令在调用io接口，发送数据后，清除缓存数据
                QJsonObject resObj;
                resObj.insert("result", "true");
                receiveData_slot(false, resObj, seqNum);
            }
        } else {
            // 多机之间的数据会串
            qDebug() << "invail seq num" << seqNum;
            qDebug() << data;
        }
    });
    connect(MagicianGoApi::instance(), &MagicianGoApi::receiveData_signal, this, &DProtocol3::receiveData_slot);
    connect(GeneralApi::instance(), &GeneralApi::receiveData_signal, this, &DProtocol3::receiveData_slot);
    connect(CameraApi::instance(), &CameraApi::receiveData_signal, this, &DProtocol3::receiveData_slot);
    connect(MagicBoxApi::instance(), &MagicBoxApi::receiveData_signal, this, &DProtocol3::receiveData_slot);
    connect(CameraCalibration::instance(), &CameraCalibration::receiveData_signal, this, &DProtocol3::receiveData_slot);
}

DProtocol3::~ DProtocol3()
{
    P3FrameAdapter::instance()->disconnect();
    MagicianGoApi::instance()->disconnect();
    CameraApi::instance()->disconnect();
    MagicBoxApi::instance()->disconnect();
    CameraCalibration::instance()->disconnect();
}

void  DProtocol3::handleGeneralCommand(const P3Packet &packet){
    QJsonObject resObj;
    P3ResPacket resPacket(packet.id, packet.wsPort);

    if (packet.api == "HeartBeat") {
        handleHeartBeat(packet);
    } else if (packet.api == "SetDeviceSN") {
        handleSetDeviceSN(packet);
    } else if (packet.api == "GetDeviceSN") {
        handleGetDeviceSN(packet);
    } else if (packet.api == "SetDeviceName") {
        handleSetDeviceName(packet);
    } else if (packet.api == "GetDeviceName") {
        handleGetDeviceName(packet);
    } else if (packet.api == "GetDeviceFwSoftwareVersion") {
        handleGetDeviceFwSoftwareVersion(packet);
    } else if (packet.api == "GetDeviceFwHardwareVersion") {
        handleGetDeviceFwHardwareVersion(packet);
    } else if (packet.api == "GetDeviceTime") {
        handleGetDeviceTime(packet);
    } else if (packet.api == "GetDeviceID") {
        handleGetDeviceID(packet);
    } else if (packet.api == "GetProductName") {
        handleGetProductName(packet);
    } else if (packet.api == "DeviceReboot") {
        handleDeviceReboot(packet);
    } else {
        resPacket.setErrorObj(ERROR_PROTOCOLFRAME_NOGENERAL);
        resObj = resPacket.getResultObj();
        emit recviceData_signal(resObj);
    }
}

void  DProtocol3::handleMagicianGoCommand(const P3Packet &packet)
{
    QJsonObject resObj;
    P3ResPacket resPacket(packet.id, packet.wsPort);

    if (packet.api == "SetRunningState") {
        handleSetRunningState(packet);
    } else if (packet.api == "GetRunningState") {
        handleGetRunningState(packet);
    } else if (packet.api == "SetRunningMode") {
        handleSetRunningMode(packet);
    } else if (packet.api == "GetRunningMode") {
        handleGetRunningMode(packet);
    } else if (packet.api == "SetBaseMode") {
        handleSetBaseMode(packet);
    } else if (packet.api == "SetBaseSpeedRatio") {
        handleSetBaseSpeedRatio(packet);
    }else if (packet.api == "GetBaseSpeedRatio") {
        handleGetBaseSpeedRatio(packet);
    }else if (packet.api == "SetMoveSpeed") {
        handleSetMoveSpeed(packet);
    } else if (packet.api == "GetMoveSpeed") {
        handleGetMoveSpeed(packet);
    } else if (packet.api == "SetRotate") {
        handleSetRotate(packet);
    } else if (packet.api == "SetMoveDist") {
        handleSetMoveDist(packet);
    } else if (packet.api == "SetMovePos") {
        handleSetMovePos(packet);
    } else if (packet.api == "SetArcRad") {
        handleSetArcRad(packet);
    } else if (packet.api == "SetArcCent") {
        handleSetArcCent(packet);
    } else if (packet.api == "SetLightRGB") {
        handleSetLightRGB(packet);
    } else if (packet.api == "SetLightPrompt") {
        handleSetLightPrompt(packet);
    } else if (packet.api == "SetBuzzerSound") {
        handleSetBuzzerSound(packet);
    } else if (packet.api == "SetOriginPoint") {
        handleSetOriginPoint(packet);
    } else if (packet.api == "GetUltrasoundData") {
        handleGetUltrasoundData(packet);
    } else if (packet.api == "GetSwitchState") {
        handleGetSwitchState(packet);
    } else if (packet.api == "SetSpeedometer") {
        handleSetSpeedometer(packet);
    } else if (packet.api == "GetSpeedometer") {
        handleGetSpeedometer(packet);
    } else if (packet.api == "GetStallProtection") {
        handleGetStallProtection(packet);
    } else if (packet.api == "GetOffGround") {
        handleGetOffGround(packet);
    } else if (packet.api == "GetBatteryVoltage") {
        handleGetBatteryVoltage(packet);
    } else if (packet.api == "GetImuAngle") {
        handleGetImuAngle(packet);
    } else if (packet.api == "GetAlarmInfo") {
        handleGetAlarmInfo(packet);
    } else if (packet.api == "CleanAlarmInfo") {
        handleCleanAlarmInfo(packet);
    } else if (packet.api == "SetTraceAuto") {
        handleSetTraceAuto(packet);
    } else if (packet.api == "SetTraceLineInfo") {
        handleSetTraceLineInfo(packet);
    } else if (packet.api == "SetTraceSpeed") {
        handleSetTraceSpeed(packet);
    } else if (packet.api == "SetLineSensorState") {
        handleSetLineSensorState(packet);
    } else if (packet.api == "GetLineSensorState") {
        handleGetLineSensorState(packet);
    } else if (packet.api == "SetLineSensorAdjust") {
        handleSetLineSensorAdjust(packet);
    } else if (packet.api == "SetLineSensorDefault") {
        handleSetLineSensorDefault(packet);
    } else if (packet.api == "GetLineSensorData") {
        handleGetLineSensorData(packet);
    } else if (packet.api == "SetCmdQueueStart") {
        handleSetCmdQueueStart(packet);
    } else if (packet.api == "SetCmdQueueStop") {
        handleSetCmdQueueStop(packet);
    } else if (packet.api == "SetCmdQueueForcelyStop") {
        handleSetCmdQueueForcelyStop(packet);
    } else if (packet.api == "CleanCmdQueue") {
        handleCleanCmdQueue(packet);
    } else if (packet.api == "GetQueuedCmdCurrentIndex") {
        handleGetCmdQueueCurrentIndex(packet);
    } else if (packet.api == "GetCmdQueueAvailableSpace") {
        handleGetCmdQueueAvailableSpace(packet);
    } else if(packet.api == "SetMoveSpeedTime"){
        handleSetMoveSpeedTime(packet);
    } else if(packet.api == "SetMoveSpeedDirect"){
        handleSetMoveSpeedDirect(packet);
    } else if(packet.api == "SetCoordClosedLoop"){
        handleSetCoordClosedLoop(packet);
    } else if(packet.api == "SetIncrementClosedLoop"){
        handleSetIncrementClosedLoop(packet);
    } else if(packet.api == "GetImuSpeed"){
        handleGetImuSpeed(packet);
    } else if(packet.api == "SetTracePid"){
        handleSetTracePid(packet);
    } else if(packet.api == "SetLightPrompt"){
        handleSetLightPrompt(packet);
    } else if (packet.api == "SetTraceLoop") {
        handleSetTraceLoop(packet);
    } else if (packet.api == "SetStopPointServer") {
        handleSetStopPointServer(packet);
    } else if (packet.api == "SetStopPointParam") {
        handleSetStopPointParam(packet);
    } else if (packet.api == "GetStopPointState") {
        handleGetStopPointState(packet);
    } else if (packet.api == "GetImgToArmXY") {
        handleGetImgToArmXY(packet);
    } else {
        resPacket.setErrorObj(ERROR_PROTOCOLFRAME_NOGO);
        resObj = resPacket.getResultObj();
        emit recviceData_signal(resObj);
    }

}

void  DProtocol3::handleCameraCommand(const P3Packet &packet)
{
    QJsonObject resObj;
    P3ResPacket resPacket(packet.id, packet.wsPort);

    if (packet.api == "GetArmCameraAngle") {
        handleGetArmCameraAngle(packet);
    } else if (packet.api == "GetArmCameraColor") {
        handleGetArmCameraColor(packet);
    } else if (packet.api == "GetArmCameraObj") {
        handleGetArmCameraObj(packet);
    } else if (packet.api == "GetArmCameraTag") {
        handleGetArmCameraTag(packet);
    } else if (packet.api == "SetArmCameraCalibrationMode") {
        handleSetArmCameraCalibrationMode(packet);
    } else if (packet.api == "GetArmCameraCalibrationMode") {
        handleGetArmCameraCalibrationMode(packet);
    } else if (packet.api == "GetCameraCalibrationData") {
        handleGetCameraCalibrationData(packet);
    }else if (packet.api == "GetCarCameraAngle") {
        handleGetCarCameraAngle(packet);
    } else if (packet.api == "GetCarCameraColor") {
        handleGetCarCameraColor(packet);
    } else if (packet.api == "GetCarCameraObj") {
        handleGetCarCameraObj(packet);
    } else if (packet.api == "GetCarCameraTag") {
        handleGetCarCameraTag(packet);
    } else if (packet.api == "SetCarCameraCalibrationMode") {
        handleSetCarCameraCalibrationMode(packet);
    } else if (packet.api == "GetCarCameraCalibrationMode") {
        handleGetCarCameraCalibrationMode(packet);
    } else if (packet.api == "GetCameraCalibrationData") {
        handleGetCameraCalibrationData(packet);
    } else if (packet.api == "SetCarCameraRunModel") {
        handleSetCarCameraRunModel(packet);
    } else if (packet.api == "GetCarCameraRunModel") {
        handleGetCarCameraRunModel(packet);
    } else if (packet.api == "SetArmCameraRunModel") {
        handleSetArmCameraRunModel(packet);
    } else if (packet.api == "GetArmCameraRunModel") {
        handleGetArmCameraRunModel(packet);
    } else {
        resPacket.setErrorObj(ERROR_PROTOCOLFRAME_NOCAMERA);
        resObj = resPacket.getResultObj();
        emit recviceData_signal(resObj);
    }
}

void  DProtocol3::handleMagicBoxCommand(const P3Packet &packet)
{
    QJsonObject resObj;
    P3ResPacket resPacket(packet.id, packet.wsPort);

    if (packet.api == "GetMagicBoxMode") {
        handleGetMagicBoxMode(packet);
    } else if (packet.api == "GetMagicBoxNum") {
        handleGetMagicBoxNum(packet);
    } else if (packet.api == "SetMagicBoxScriptMode") {
        handleSetMagicBoxScriptMode(packet);
    } else {
        resPacket.setErrorObj(ERROR_PROTOCOLFRAME_NOBOX);
        resObj = resPacket.getResultObj();
        emit recviceData_signal(resObj);
    }
}

void  DProtocol3::handleHeartBeat(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    GeneralApi::instance()->HeartBeat(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetDeviceSN(const P3Packet &packet)
{
    QByteArray ba = packet.paramsObj.value("deviceSN").toString().toLatin1();
     char * deviceSN = ba.data();

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    GeneralApi::instance()->SetDeviceSN(sm_seqNum, deviceSN);
    sm_seqNum++;
}

void  DProtocol3::handleGetDeviceSN(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    GeneralApi::instance()->GetDeviceSN(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetDeviceName(const P3Packet &packet)
{
    QByteArray ba= packet.paramsObj.value("deviceName").toString().toLatin1();
    char *deviceName = ba.data();

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    GeneralApi::instance()->SetDeviceName(sm_seqNum, deviceName);
    sm_seqNum++;
}

void  DProtocol3::handleGetDeviceName(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    GeneralApi::instance()->GetDeviceName(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetDeviceFwSoftwareVersion(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    GeneralApi::instance()->GetDeviceFwSoftwareVersion(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetDeviceFwHardwareVersion(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    GeneralApi::instance()->GetDeviceFwHardwareVersion(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetDeviceTime(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    GeneralApi::instance()->GetDeviceTime(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetDeviceID(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    GeneralApi::instance()->GetDeviceID(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetProductName(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    GeneralApi::instance()->GetProductName(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleDeviceReboot(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    GeneralApi::instance()->DeviceReboot(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetRunningState(const P3Packet &packet)
{
    uint8_t runningState = static_cast<uint8_t>(packet.paramsObj.value("runningState").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetRunningState(sm_seqNum, runningState);
    sm_seqNum++;
}

void  DProtocol3::handleGetRunningState(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetRunningState(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetRunningMode(const P3Packet &packet)
{
    uint8_t runningMode =static_cast<uint8_t>(packet.paramsObj.value("runningMode").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetRunningMode(sm_seqNum, runningMode);
    sm_seqNum++;
}

void  DProtocol3::handleGetRunningMode(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetRunningMode(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetBaseMode(const P3Packet &packet)
{
    uint8_t baseMode = static_cast<uint8_t>(packet.paramsObj.value("baseMode").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetBaseMode(sm_seqNum, baseMode);
    sm_seqNum++;
}

void  DProtocol3::handleSetBaseSpeedRatio(const P3Packet &packet)
{
    uint8_t baseMode = static_cast<uint8_t>(packet.paramsObj.value("speedRatio").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetBaseSpeedRatio(sm_seqNum, baseMode);
    sm_seqNum++;
}

void  DProtocol3::handleGetBaseSpeedRatio(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetBaseSpeedRatio(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetMoveSpeed(const P3Packet &packet)
{
    float x = static_cast<float>(packet.paramsObj.value("x").toDouble());
    float y = static_cast<float>(packet.paramsObj.value("y").toDouble());
    float r = static_cast<float>(packet.paramsObj.value("r").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetMoveSpeed(sm_seqNum, x, y, r);
    sm_seqNum++;
}

void  DProtocol3::handleGetMoveSpeed(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetMoveSpeed(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetRotate(const P3Packet &packet)
{
    float r = static_cast<float>(packet.paramsObj.value("r").toDouble());
    float Vr = static_cast<float>(packet.paramsObj.value("Vr").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetRotate(sm_seqNum, r, Vr);
    sm_seqNum++;
}

void  DProtocol3::handleSetMoveDist(const P3Packet &packet)
{
    float x = static_cast<float>(packet.paramsObj.value("x").toDouble());
    float y = static_cast<float>(packet.paramsObj.value("y").toDouble());
    float Vx = static_cast<float>(packet.paramsObj.value("Vx").toDouble());
    float Vy = static_cast<float>(packet.paramsObj.value("Vy").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetMoveDist(sm_seqNum, x, y, Vx, Vy);
    sm_seqNum++;
}

void  DProtocol3::handleSetMovePos(const P3Packet &packet)
{
    float x = static_cast<float>(packet.paramsObj.value("x").toDouble());
    float y = static_cast<float>(packet.paramsObj.value("y").toDouble());
    float s = static_cast<float>(packet.paramsObj.value("s").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetMovePos(sm_seqNum, x, y, s);
    sm_seqNum++;
}

void  DProtocol3::handleSetArcRad(const P3Packet &packet)
{
    float velocity = static_cast<float>(packet.paramsObj.value("velocity").toDouble());
    float radius = static_cast<float>(packet.paramsObj.value("radius").toDouble());
    float angle = static_cast<float>(packet.paramsObj.value("angle").toDouble());
    int mode= static_cast<int>(packet.paramsObj.value("mode").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetArcRad(sm_seqNum, velocity, radius, angle, mode);
    sm_seqNum++;
}

void  DProtocol3::handleSetArcCent(const P3Packet &packet)
{
    float velocity = static_cast<float>(packet.paramsObj.value("velocity").toDouble());
    float x = static_cast<float>(packet.paramsObj.value("x").toDouble());
    float y = static_cast<float>(packet.paramsObj.value("y").toDouble());
    float angle = static_cast<float>(packet.paramsObj.value("angle").toDouble());
    int mode= static_cast<int>(packet.paramsObj.value("mode").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetArcCent(sm_seqNum, velocity, x, y, angle, mode);
    sm_seqNum++;
}

void  DProtocol3::handleSetLightRGB(const P3Packet &packet)
{
    uint16_t number = static_cast<uint16_t>(packet.paramsObj.value("number").toDouble());
    uint8_t effect = static_cast<uint8_t>(packet.paramsObj.value("effect").toDouble());
    uint8_t r = static_cast<uint8_t>(packet.paramsObj.value("r").toDouble());
    uint8_t g = static_cast<uint8_t>(packet.paramsObj.value("g").toDouble());
    uint8_t b = static_cast<uint8_t>(packet.paramsObj.value("b").toDouble());
    float cycle = static_cast<float>(packet.paramsObj.value("cycle").toDouble());
    int counts = static_cast<int>(packet.paramsObj.value("counts").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetLightRGB(sm_seqNum, number, effect, r, g, b, cycle, counts);
    sm_seqNum++;
}

void  DProtocol3::handleSetLightPrompt(const P3Packet &packet)
{
    uint8_t index = static_cast<uint8_t>(packet.paramsObj.value("index").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetLightPrompt(sm_seqNum, index);
    sm_seqNum++;
}

void DProtocol3::handleSetTraceLoop(const P3Packet &packet)
{
    bool enable = static_cast<bool>(packet.paramsObj.value("enable").toBool());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetTraceLoop(sm_seqNum, enable);
    sm_seqNum++;
}

void  DProtocol3::handleSetBuzzerSound(const P3Packet &packet)
{
    uint8_t index = static_cast<uint8_t>(packet.paramsObj.value("index").toDouble());
    uint8_t tone = static_cast<uint8_t>(packet.paramsObj.value("tone").toDouble());
    float beat = static_cast<float>(packet.paramsObj.value("beat").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetBuzzerSound(sm_seqNum, index, tone, beat);
    sm_seqNum++;
}

void  DProtocol3::handleSetOriginPoint(const P3Packet &packet)
{
    uint8_t enable = static_cast<uint8_t>(packet.paramsObj.value("enable").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetOriginPoint(sm_seqNum, enable);
    sm_seqNum++;
}

void  DProtocol3::handleGetUltrasoundData(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetUltrasoundData(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetSwitchState(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetSwitchState(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetSpeedometer(const P3Packet &packet)
{
    float x = static_cast<float>(packet.paramsObj.value("x").toDouble());
    float y = static_cast<float>(packet.paramsObj.value("y").toDouble());
    float yaw = static_cast<float>(packet.paramsObj.value("yaw").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetSpeedometer(sm_seqNum, x, y, yaw);
    sm_seqNum++;
}

void  DProtocol3::handleGetSpeedometer(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetSpeedometer(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetStallProtection(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetStallProtection(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetOffGround(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetOffGround(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetBatteryVoltage(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetBatteryVoltage(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetImuAngle(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetImuAngle(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetAlarmInfo(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetAlarmInfo(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleCleanAlarmInfo(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->CleanAlarmInfo(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetTraceAuto(const P3Packet &packet)
{
    uint8_t isTrace = static_cast<uint8_t>(packet.paramsObj.value("isTrace").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetTraceAuto(sm_seqNum, isTrace);
    sm_seqNum++;
}

void  DProtocol3::handleSetTraceLineInfo(const P3Packet &packet)
{
    uint8_t lineInfo = static_cast<uint8_t>(packet.paramsObj.value("lineInfo").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetTraceLineInfo(sm_seqNum, lineInfo);
    sm_seqNum++;
}

void  DProtocol3::handleSetTraceSpeed(const P3Packet &packet)
{
    float speed = static_cast<float>(packet.paramsObj.value("speed").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetTraceSpeed(sm_seqNum, speed);
    sm_seqNum++;
}

void  DProtocol3::handleSetLineSensorState(const P3Packet &packet)
{
    uint8_t frontIsEnable = static_cast<uint8_t>(packet.paramsObj.value("frontIsEnable").toInt());
    uint8_t backIsEnable = static_cast<uint8_t>(packet.paramsObj.value("backIsEnable").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetLineSensorState(sm_seqNum, frontIsEnable, backIsEnable);
    sm_seqNum++;
}

void  DProtocol3::handleGetLineSensorState(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetLineSensorState(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetLineSensorAdjust(const P3Packet &packet)
{
    uint8_t deviceNum = static_cast<uint8_t>(packet.paramsObj.value("deviceNum").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetLineSensorAdjust(sm_seqNum, deviceNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetLineSensorDefault(const P3Packet &packet)
{
    uint8_t deviceNum = static_cast<uint8_t>(packet.paramsObj.value("deviceNum").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetLineSensorDefault(sm_seqNum, deviceNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetLineSensorData(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetLineSensorData(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetCmdQueueStart(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetCmdQueueStart(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetCmdQueueStop(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetCmdQueueStop(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetCmdQueueForcelyStop(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetCmdQueueForcelyStop(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleCleanCmdQueue(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->CleanCmdQueue(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetCmdQueueCurrentIndex(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetQueuedCmdCurrentIndex(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetCmdQueueAvailableSpace(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetCmdQueueAvailableSpace(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetArmCameraAngle(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->GetArmCameraAngle(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetArmCameraColor(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->GetArmCameraColor(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetArmCameraObj(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->GetArmCameraObj(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetArmCameraTag(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->GetArmCameraTag(sm_seqNum);
    sm_seqNum++;
}

void DProtocol3::handleSetArmCameraCalibrationMode(const P3Packet &packet)
{
    uint8_t isEnableCali = static_cast<uint8_t>(packet.paramsObj.value("isEnableCali").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->SetArmCameraCalibrationMode(sm_seqNum, isEnableCali);
    sm_seqNum++;
}

void DProtocol3::handleGetArmCameraCalibrationMode(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->GetArmCameraCalibrationMode(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetCarCameraAngle(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->GetCarCameraAngle(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetCarCameraColor(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->GetCarCameraColor(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetCarCameraObj(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->GetCarCameraObj(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetCarCameraTag(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->GetCarCameraTag(sm_seqNum);
    sm_seqNum++;
}

void DProtocol3::handleSetCarCameraCalibrationMode(const P3Packet &packet)
{
    uint8_t isEnableCali = static_cast<uint8_t>(packet.paramsObj.value("isEnableCali").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->SetCarCameraCalibrationMode(sm_seqNum, isEnableCali);
    sm_seqNum++;
}

void DProtocol3::handleGetCarCameraCalibrationMode(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->GetCarCameraCalibrationMode(sm_seqNum);
    sm_seqNum++;
}

void DProtocol3::handleGetCameraCalibrationData(const P3Packet &packet)
{
    QString april_list = static_cast<QString>(packet.paramsObj.value("april_list").toString());
    QString device_list = static_cast<QString>(packet.paramsObj.value("device_list").toString());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraCalibration::instance()->GOManualCalibration(sm_seqNum, april_list, device_list);
    sm_seqNum++;

}

void DProtocol3::handleSetCarCameraRunModel(const P3Packet &packet)
{
    uint8_t runModelIndex = static_cast<uint8_t>(packet.paramsObj.value("runModelIndex").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->SetCarCameraRunModel(sm_seqNum, runModelIndex);
    sm_seqNum++;
}

void DProtocol3::handleSetArmCameraRunModel(const P3Packet &packet)
{
    uint8_t runModelIndex = static_cast<uint8_t>(packet.paramsObj.value("runModelIndex").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->SetArmCameraRunModel(sm_seqNum, runModelIndex);
    sm_seqNum++;
}

void DProtocol3::handleGetCarCameraRunModel(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->GetCarCameraRunModel(sm_seqNum);
    sm_seqNum++;
}

void DProtocol3::handleGetArmCameraRunModel(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    CameraApi::instance()->GetArmCameraRunModel(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetMoveSpeedTime(const P3Packet &packet)
{
    float time = static_cast<float>(packet.paramsObj.value("time").toDouble());
    float x = static_cast<float>(packet.paramsObj.value("x").toDouble());
    float y = static_cast<float>(packet.paramsObj.value("y").toDouble());
    float r = static_cast<float>(packet.paramsObj.value("r").toDouble());
    bool isACK = packet.paramsObj.value("isAck").toBool();

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort, isACK));
    MagicianGoApi::instance()->SetMoveSpeedTime(sm_seqNum,isACK,time,x,y,r);
    sm_seqNum++;
}

void  DProtocol3::handleGetMagicBoxMode(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicBoxApi::instance()->GetMagicBoxMode(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleGetMagicBoxNum(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicBoxApi::instance()->GetMagicBoxNum(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetMagicBoxScriptMode(const P3Packet &packet)
{
    uint8_t isEnable = static_cast<uint8_t>(packet.paramsObj.value("isEnable").toInt());
    QString name = static_cast<QString>(packet.paramsObj.value("name").toString());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicBoxApi::instance()->SetMagicBoxScriptMode(sm_seqNum, isEnable, name);
    sm_seqNum++;

}

void  DProtocol3::handleSetCoordClosedLoop(const P3Packet &packet)
{
    uint8_t isEnable = static_cast<uint8_t>(packet.paramsObj.value("isEnable").toInt());
    float angle = static_cast<float>(packet.paramsObj.value("angle").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetCoordClosedLoop(sm_seqNum, isEnable, angle);
    sm_seqNum++;
}

void  DProtocol3::handleSetIncrementClosedLoop(const P3Packet &packet)
{
    float x = static_cast<float>(packet.paramsObj.value("x").toDouble());
    float y = static_cast<float>(packet.paramsObj.value("y").toDouble());
    float angle = static_cast<float>(packet.paramsObj.value("angle").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetIncrementClosedLoop(sm_seqNum, x, y, angle);
    sm_seqNum++;

}

void  DProtocol3::handleGetImuSpeed(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetImuSpeed(sm_seqNum);
    sm_seqNum++;
}

void  DProtocol3::handleSetTracePid(const P3Packet &packet)
{
    float p = static_cast<float>(packet.paramsObj.value("p").toDouble());
    float i = static_cast<float>(packet.paramsObj.value("i").toDouble());
    float d = static_cast<float>(packet.paramsObj.value("d").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetTracePid(sm_seqNum, p, i, d);
    sm_seqNum++;
}

void  DProtocol3::handleSetMoveSpeedDirect(const P3Packet &packet)
{
    float dir = static_cast<float>(packet.paramsObj.value("dir").toDouble());
    float speed = static_cast<float>(packet.paramsObj.value("speed").toDouble());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->SetMoveSpeedDirect(sm_seqNum, dir, speed);
    sm_seqNum++;

}

void DProtocol3::handleSetStopPointServer(const P3Packet &packet)
{
    int16_t PointX = static_cast<int16_t>(packet.paramsObj.value("PointX").toInt());
    int16_t PointY = static_cast<int16_t>(packet.paramsObj.value("PointY").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicBoxApi::instance()->SetStopPointServer(sm_seqNum, PointX, PointY);
    sm_seqNum++;

}

void DProtocol3::handleSetStopPointParam(const P3Packet &packet)
{
    int16_t scopeErr = static_cast<int16_t>(packet.paramsObj.value("scopeErr").toInt());
    int16_t stopErr = static_cast<int16_t>(packet.paramsObj.value("stopErr").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicBoxApi::instance()->SetStopPointParam(sm_seqNum, scopeErr, stopErr);
    sm_seqNum++;

}

void DProtocol3::handleGetStopPointState(const P3Packet &packet)
{
    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicBoxApi::instance()->GetStopPointState(sm_seqNum);
    sm_seqNum++;

}

void DProtocol3::handleGetImgToArmXY(const P3Packet &packet)
{
    float imgX = static_cast<float>(packet.paramsObj.value("imgX").toDouble());
    float imgY = static_cast<float>(packet.paramsObj.value("imgY").toDouble());
    uint8_t need_tranxy = static_cast<uint8_t>(packet.paramsObj.value("need_tranxy").toInt());
    uint8_t suck_apriltag = static_cast<uint8_t>(packet.paramsObj.value("suck_apriltag").toInt());

    m_idPortSeqMap.insert(sm_seqNum, IdPortPacket(packet.id, packet.wsPort));
    MagicianGoApi::instance()->GetImgToArmXY(sm_seqNum, imgX, imgY, need_tranxy, suck_apriltag);
    sm_seqNum++;

}

void  DProtocol3::receiveData_slot(bool isTimeOut, QJsonObject jsonObj, quint32 seqNum)
{
    if (!m_idPortSeqMap.contains(seqNum)) return;

    QMap<quint32, IdPortPacket>::iterator iter = m_idPortSeqMap.find(seqNum);
    P3ResPacket resPacket(iter->id, iter->wsPort);
    m_idPortSeqMap.remove(seqNum);

    if (isTimeOut) {
        resPacket.setErrorObj(ERROR_PROTOCOLFRAME_TIMEOUT);
    } else {
        resPacket.setResultObj(jsonObj);
    }
    emit recviceData_signal(resPacket.getResultObj());
}

HandleBytesState DProtocol3::handleRawData(QByteArray &rawData)
{
    if (rawData.size() < 4) {
        return TOO_SHORT;
    }

    QByteArray lenBA = rawData.mid(2, 2);
    QDataStream lenStream(&lenBA, QIODevice::ReadOnly);
    lenStream.setByteOrder(QDataStream::LittleEndian);
    quint16 len;
    lenStream >> len;
    if (rawData.size() < len + 15 + 2) {
        return TOO_SHORT;
    }

    // 获取checksum
    QByteArray chekSumBA = rawData.mid(len + 15, 2);
    QDataStream chekSumStream(&chekSumBA, QIODevice::ReadOnly);
    chekSumStream.setByteOrder(QDataStream::LittleEndian);
    quint16 checkSum;
    chekSumStream >> checkSum;

    // 计算checksum
    if (checkSum == _CalcCheckSum(rawData.left(len + 15))) {
        emit frameAddRawData_signal(rawData.left(len + 17));
        rawData.remove(0, len + 17);
        return OK;
    } else {
        return CHECKSUM_ERR;
    }
}

quint16 DProtocol3::_CalcCheckSum(const QByteArray &data)
{
    /* crc16计算表 */
    const static uint16_t gDobotV3Format_crc_ibm_table[256] = {
        0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
        0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
        0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
        0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
        0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
        0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
        0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
        0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
        0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
        0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
        0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
        0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
        0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
        0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
        0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
        0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
        0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
        0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
        0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
        0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
        0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
        0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
        0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
        0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
        0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
        0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
        0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
        0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
        0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
        0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
        0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
        0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
    };

    int len(data.size());
    quint16 crc = 0x0000;
    quint8 lut = 0;
    for(int index(0); index < len; ++index) {
        lut = (crc ^ data[index]) & 0xFF;
        crc = (crc >> 8) ^ gDobotV3Format_crc_ibm_table[lut];
    }
    return crc;
}
