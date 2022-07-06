#ifndef PROTOCOLFRAMEPLUGIN_H
#define PROTOCOLFRAMEPLUGIN_H


#include "DP3Packet.h"
#include "DobotType.h"
#include <QObject>
#include <QJsonObject>
#include <QMap>
#include "DProtocolInterface.h"

class IdPortPacket {
public:
    quint64 id;
    quint16 wsPort;
    bool isAck;
    IdPortPacket():
        id(0),
        wsPort(0),
        isAck(true)
        {}
    IdPortPacket(quint64 packetId, quint16 packetWsPort, bool packetIsAck=true):
        id(packetId),
        wsPort(packetWsPort),
        isAck(packetIsAck)
        {}
};

class  DProtocol3 : public DProtocolInterface
{
    Q_OBJECT

public:
    DProtocol3(QObject *parent = nullptr);
    ~DProtocol3();

    bool sendData(quint64 id, const QJsonObject &obj, bool needReply=true) override;
    HandleBytesState handleRawData(QByteArray &rawData) override;
    void stop() override;
    void start() override;
    void setCommPars(quint8 resendTimesMax, quint32 commTimeout) override;

signals:
    void frameAddRawData_signal(QByteArray data);
    void frameSetCommPars_signal(quint8, quint32);
    void frameStart_signal();
    void frameStop_signal();

private:
    void handlePortCommand(const P3Packet &packet);
    void handleGeneralCommand(const P3Packet &packet);
    void handleMagicianGoCommand(const P3Packet &packet);
    void handleCameraCommand(const P3Packet &packet);
    void handleMagicBoxCommand(const P3Packet &packet);

    void handleHeartBeat(const P3Packet &packet);
    void handleSetDeviceSN(const P3Packet &packet);
    void handleGetDeviceSN(const P3Packet &packet);
    void handleSetDeviceName(const P3Packet &packet);
    void handleGetDeviceName(const P3Packet &packet);
    void handleGetDeviceFwSoftwareVersion(const P3Packet &packet);
    void handleGetDeviceFwHardwareVersion(const P3Packet &packet);
    void handleGetDeviceTime(const P3Packet &packet);
    void handleGetDeviceID(const P3Packet &packet);
    void handleGetProductName(const P3Packet &packet);
    void handleDeviceReboot(const P3Packet &packet);

    void handleSetRunningState(const P3Packet &packet);
    void handleGetRunningState(const P3Packet &packet);
    void handleSetRunningMode(const P3Packet &packet);
    void handleGetRunningMode(const P3Packet &packet);
    void handleSetBaseMode(const P3Packet &packet);
    void handleSetBaseSpeedRatio(const P3Packet &packet);
    void handleGetBaseSpeedRatio(const P3Packet &packet);
    void handleSetMoveSpeed(const P3Packet &packet);
    void handleGetMoveSpeed(const P3Packet &packet);
    void handleSetRotate(const P3Packet &packet);
    void handleSetMoveDist(const P3Packet &packet);
    void handleSetMovePos(const P3Packet &packet);
    void handleSetArcRad(const P3Packet &packet);
    void handleSetArcCent(const P3Packet &packet);
    void handleSetLightRGB(const P3Packet &packet);
    void handleSetLightPrompt(const P3Packet &packet);
    void handleSetBuzzerSound(const P3Packet &packet);
    void handleSetOriginPoint(const P3Packet &packet);
    void handleGetUltrasoundData(const P3Packet &packet);
    void handleGetSwitchState(const P3Packet &packet);
    void handleSetSpeedometer(const P3Packet &packet);
    void handleGetSpeedometer(const P3Packet &packet);
    void handleGetStallProtection(const P3Packet &packet);
    void handleGetOffGround(const P3Packet &packet);
    void handleGetBatteryVoltage(const P3Packet &packet);
    void handleGetImuAngle(const P3Packet &packet);
    void handleGetAlarmInfo(const P3Packet &packet);
    void handleCleanAlarmInfo(const P3Packet &packet);
    void handleSetTraceAuto(const P3Packet &packet);
    void handleSetTraceLineInfo(const P3Packet &packet);
    void handleSetTraceSpeed(const P3Packet &packet);
    void handleSetLineSensorState(const P3Packet &packet);
    void handleGetLineSensorState(const P3Packet &packet);
    void handleSetLineSensorAdjust(const P3Packet &packet);
    void handleSetLineSensorDefault(const P3Packet &packet);
    void handleGetLineSensorData(const P3Packet &packet);
    void handleSetCmdQueueStart(const P3Packet &packet);
    void handleSetCmdQueueStop(const P3Packet &packet);
    void handleSetCmdQueueForcelyStop(const P3Packet &packet);
    void handleCleanCmdQueue(const P3Packet &packet);
    void handleGetCmdQueueCurrentIndex(const P3Packet &packet);
    void handleGetCmdQueueAvailableSpace(const P3Packet &packet);
    void handleSetMoveSpeedDirect(const P3Packet &packet);
    void handleSetCoordClosedLoop(const P3Packet &packet);
    void handleSetIncrementClosedLoop(const P3Packet &packet);
    void handleGetImuSpeed(const P3Packet &packet);
    void handleSetTracePid(const P3Packet &packet);
    void handleGetImgToArmXY(const P3Packet &packet);

    void handleGetCarCameraAngle(const P3Packet &packet);
    void handleGetCarCameraColor(const P3Packet &packet);
    void handleGetCarCameraObj(const P3Packet &packet);
    void handleGetCarCameraTag(const P3Packet &packet);
    void handleSetCarCameraCalibrationMode(const P3Packet &packet);
    void handleGetCarCameraCalibrationMode(const P3Packet &packet);
    void handleSetCarCameraRunModel(const P3Packet &packet);
    void handleGetCarCameraRunModel(const P3Packet &packet);

    void handleGetArmCameraAngle(const P3Packet &packet);
    void handleGetArmCameraColor(const P3Packet &packet);
    void handleGetArmCameraObj(const P3Packet &packet);
    void handleGetArmCameraTag(const P3Packet &packet);
    void handleSetArmCameraCalibrationMode(const P3Packet &packet);
    void handleGetArmCameraCalibrationMode(const P3Packet &packet);
    void handleSetArmCameraRunModel(const P3Packet &packet);
    void handleGetArmCameraRunModel(const P3Packet &packet);


    void handleGetCameraCalibrationData(const P3Packet &packet);

    void handleSetMoveSpeedTime(const P3Packet &packet);

    void handleGetMagicBoxMode(const P3Packet &packet);
    void handleGetMagicBoxNum(const P3Packet &packet);
    void handleSetMagicBoxScriptMode(const P3Packet &packet);

    void handleSetTraceLoop(const P3Packet &packet);

    void handleSetStopPointServer(const P3Packet &packet);
    void handleSetStopPointParam(const P3Packet &packet);
    void handleGetStopPointState(const P3Packet &packet);

    quint16 _CalcCheckSum(const QByteArray &data);

public slots:
    void receiveData_slot(bool isTimeOut, QJsonObject jsonObj, quint32 seqNum);

private:
    static uint32_t sm_seqNum;
    QMap<uint32_t, IdPortPacket> m_idPortSeqMap;
};

#endif // PROTOCOLFRAMEPLUGIN_H
