#ifndef GENERALAPI_H
#define GENERALAPI_H

#include <QObject>
#include <QJsonObject>
#include <QMap>
#include <QTimer>
#include "DobotType.h"
#include "DP3Packet.h"
extern "C" {
#include "DobotV3API.h"
}

class GeneralApi : public QObject
{
        Q_OBJECT

public:
    static GeneralApi *instance();

    //处理发送的数据
    void HeartBeat(uint32_t seqNum);
    void SetDeviceSN(uint32_t seqNum, const char *deviceSN);
    void GetDeviceSN(uint32_t seqNum);
    void SetDeviceName(uint32_t seqNum, const char *deviceName);
    void GetDeviceName(uint32_t seqNum);
    void GetDeviceFwSoftwareVersion(uint32_t seqNum);
    void GetDeviceFwHardwareVersion(uint32_t seqNum);
    void GetDeviceTime(uint32_t seqNum);
    void GetDeviceID(uint32_t seqNum);
    void GetProductName(uint32_t seqNum);
    void DeviceReboot(uint32_t seqNum);
    void SetTimeout(quint16 timeout);

        //接受的回调函数
    static void HeartBeat_CB(DobotV3CmdInterCBParm packet);
    static void DeviceSN_CB(DobotV3CmdInterCBParm packet);
    static void DeviceName_CB(DobotV3CmdInterCBParm packet);
    static void GetDeviceFwSoftwareVersion_CB(DobotV3CmdInterCBParm packet);
    static void GetDeviceFwHardwareVersion_CB(DobotV3CmdInterCBParm packet);
    static void GetDeviceTime_CB(DobotV3CmdInterCBParm packet);
    static void GetDeviceID_CB(DobotV3CmdInterCBParm packet);
    static void GetProductName_CB(DobotV3CmdInterCBParm packet);
    static void DeviceReboot_CB(DobotV3CmdInterCBParm packet);


signals:
    void receiveData_signal(bool isTimeOut, QJsonObject jsonObj, quint32 seqNum);
    void sendCmd_signal(DobotV3Format_Packet pPacket, quint16 payloadLen, bool isAck);

private:
    static GeneralApi *m_staticGeneralApi;
    DobotV3Format_Packet m_packFrame;
    GeneralApi();
    ~GeneralApi();
};

#endif // GENERALAPI_H


