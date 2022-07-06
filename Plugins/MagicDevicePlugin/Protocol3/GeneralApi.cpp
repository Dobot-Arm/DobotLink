#include "GeneralApi.h"
#include <QJsonArray>
#include <QTime>
#include "P3FrameAdapter.h"

typedef enum {
    HeartBeatId = 0,
    SetDeviceSNId = 10,
    GetDeviceSNId = 10,
    SetDeviceNameId = 11,
    GetDeviceNameId = 11,
    GetDeviceFwSoftwareVersionId = 12,
    GetDeviceFwHardwareVersionId = 13,
    GetDeviceTimeId = 14,
    GetDeviceIDId = 15,
    GetProductNameId = 17,
    DeviceRebootId = 20,
} GeneralCmdID;

typedef enum {
    CBHeartBeatId = 0x00,
    CBDeviceSNId = 0x0A,
    CBDeviceNameId = 0x0B,
    CBGetDeviceFwSoftwareVersionId = 0x0C,
    CBGetDeviceFwHardwareVersionId = 0x0D,
    CBGetDeviceTimeId = 0x0E,
    CBGetDeviceIDId = 0x0F,
    CBGetProductNameId = 0x11,
    CBDeviceRebootId = 0x14,
} CBGeneralID;


GeneralApi *GeneralApi::m_staticGeneralApi = new GeneralApi();

GeneralApi *GeneralApi::instance()
{
    return m_staticGeneralApi;
}

GeneralApi::GeneralApi()
{
    connect(this, &GeneralApi::sendCmd_signal, P3FrameAdapter::instance(), &P3FrameAdapter::sendCmd_slot);

    P3FrameAdapter::instance()->registerGeneralCB(CBHeartBeatId, HeartBeat_CB);
    P3FrameAdapter::instance()->registerGeneralCB(CBDeviceSNId, DeviceSN_CB);
    P3FrameAdapter::instance()->registerGeneralCB(CBDeviceNameId, DeviceName_CB);
    P3FrameAdapter::instance()->registerGeneralCB(CBGetDeviceFwSoftwareVersionId, GetDeviceFwSoftwareVersion_CB);
    P3FrameAdapter::instance()->registerGeneralCB(CBGetDeviceFwHardwareVersionId, GetDeviceFwHardwareVersion_CB);
    P3FrameAdapter::instance()->registerGeneralCB(CBGetDeviceTimeId, GetDeviceTime_CB);
    P3FrameAdapter::instance()->registerGeneralCB(CBGetDeviceIDId, GetDeviceID_CB);
    P3FrameAdapter::instance()->registerGeneralCB(CBGetProductNameId, GetProductName_CB);
    P3FrameAdapter::instance()->registerGeneralCB(CBDeviceRebootId, DeviceReboot_CB);
}

GeneralApi::~GeneralApi()
{

}

void GeneralApi::HeartBeat(uint32_t seqNum){

    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x00;
    m_packFrame.cmdId = HeartBeatId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void GeneralApi::SetDeviceSN(uint32_t seqNum, const char *deviceSN){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x00;
    m_packFrame.cmdId = SetDeviceSNId;
    uint16_t payloadLen = 64;
    memcpy(&m_packFrame.payloadAndCRC[0], &deviceSN, sizeof(char) * 64);

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void GeneralApi::GetDeviceSN(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x00;
    m_packFrame.cmdId = GetDeviceSNId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void GeneralApi::SetDeviceName(uint32_t seqNum, const char *deviceName){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x00;
    m_packFrame.cmdId = SetDeviceNameId;
    uint16_t payloadLen = 64;
    memcpy(&m_packFrame.payloadAndCRC[0], &deviceName, sizeof(char) * 64);

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void GeneralApi::GetDeviceName(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x00;
    m_packFrame.cmdId = GetDeviceNameId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void GeneralApi::GetDeviceFwSoftwareVersion(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x00;
    m_packFrame.cmdId = GetDeviceFwSoftwareVersionId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void GeneralApi::GetDeviceFwHardwareVersion(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x00;
    m_packFrame.cmdId = GetDeviceFwHardwareVersionId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void GeneralApi::GetDeviceTime(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x00;
    m_packFrame.cmdId = GetDeviceTimeId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void GeneralApi::GetDeviceID(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x00;
    m_packFrame.cmdId = GetDeviceIDId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void GeneralApi::GetProductName(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 0;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x00;
    m_packFrame.cmdId =GetProductNameId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void GeneralApi::DeviceReboot(uint32_t seqNum){
    
    memset(&m_packFrame, 0, sizeof(DobotV3Format_Packet));
    m_packFrame.version = 0x11;
    m_packFrame.needAck = 0;
    m_packFrame.isAck = 0;
    m_packFrame.queueType = 0;
    m_packFrame.rwType = 1;
    m_packFrame.encType = 0;
    m_packFrame.errflg = 0;
    m_packFrame.seqType = 1;
    m_packFrame.seqNum = seqNum;
    m_packFrame.src = 0x00;
    m_packFrame.des = 0x04;
    m_packFrame.cmdSet = 0x00;
    m_packFrame.cmdId = DeviceRebootId;
    uint16_t payloadLen = 0;

    emit sendCmd_signal(m_packFrame, payloadLen, true);
}

void GeneralApi::HeartBeat_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticGeneralApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void GeneralApi::DeviceSN_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;

        if(packet.packet.rwType == 0) {
            uint8_t deviceSN[64];
            memcpy(&deviceSN, &packet.packet.payloadAndCRC[0], sizeof(uint8_t) * 64);
            resObj.insert("deviceSN", (char *)(deviceSN));
        }else if(packet.packet.rwType == 1) {
            resObj.insert("result", "true");
        }
    }

    emit m_staticGeneralApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void GeneralApi::DeviceName_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;

        if(packet.packet.rwType == 0) {
            uint8_t deviceName[64];
            memcpy(&deviceName, &packet.packet.payloadAndCRC[0], sizeof(uint8_t) * 64);
            resObj.insert("deviceName", (char*)(deviceName));
        }else if(packet.packet.rwType == 1) {
            resObj.insert("result", "true");
        }
    }

    emit m_staticGeneralApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void GeneralApi::GetDeviceFwSoftwareVersion_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;

        uint8_t previousVersionNum;
        uint8_t revisionVersionNum;
        uint8_t secondVersionNum;
        uint8_t majorVersionNum;
        memcpy(&previousVersionNum, &packet.packet.payloadAndCRC[3], sizeof(uint8_t));
        memcpy(&revisionVersionNum, &packet.packet.payloadAndCRC[2], sizeof(uint8_t));
        memcpy(&secondVersionNum, &packet.packet.payloadAndCRC[1], sizeof(uint8_t));
        memcpy(&majorVersionNum, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        resObj.insert("previousVersionNum", previousVersionNum);
        resObj.insert("revisionVersionNum", revisionVersionNum);
        resObj.insert("secondVersionNum", secondVersionNum);
        resObj.insert("majorVersionNum", majorVersionNum);
    }

    emit m_staticGeneralApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void GeneralApi::GetDeviceFwHardwareVersion_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;

        uint8_t previousVersionNum;
        uint8_t revisionVersionNum;
        uint8_t secondVersionNum;
        uint8_t majorVersionNum;
        memcpy(&previousVersionNum, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        memcpy(&revisionVersionNum, &packet.packet.payloadAndCRC[1], sizeof(uint8_t));
        memcpy(&secondVersionNum, &packet.packet.payloadAndCRC[2], sizeof(uint8_t));
        memcpy(&majorVersionNum, &packet.packet.payloadAndCRC[3], sizeof(uint8_t));
        resObj.insert("previousVersionNum", previousVersionNum);
        resObj.insert("revisionVersionNum", revisionVersionNum);
        resObj.insert("secondVersionNum", secondVersionNum);
        resObj.insert("majorVersionNum", majorVersionNum);
    }

    emit m_staticGeneralApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void GeneralApi::GetDeviceTime_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;

        uint32_t gSystick;
        memcpy(&gSystick, &packet.packet.payloadAndCRC[0], sizeof(uint32_t));
        resObj.insert("deviceTime", static_cast<int>(gSystick));
        QTime t = QTime::fromMSecsSinceStartOfDay(static_cast<int>(gSystick));
        resObj.insert("passtime", t.toString("hh:mm:ss.z"));
    }

    emit m_staticGeneralApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void GeneralApi::GetDeviceID_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;

        uint32_t deviceId[3];
//        memcpy(&deviceId[0], &packet.packet.payloadAndCRC[0], sizeof(uint32_t));
//        memcpy(&deviceId[1], &packet.packet.payloadAndCRC[4], sizeof(uint32_t));
//        memcpy(&deviceId[2], &packet.packet.payloadAndCRC[8], sizeof(uint32_t));
//        resObj.insert("deviceId1", static_cast<int>(deviceId[0]));
//        resObj.insert("deviceId2", static_cast<int>(deviceId[1]));
//        resObj.insert("deviceId3", static_cast<int>(deviceId[2]));
        QJsonArray idArrary;
        for (int i = 0; i < 3; ++i) {
            memcpy(&deviceId[i], &packet.packet.payloadAndCRC[i*4], sizeof(uint32_t));
            idArrary.append(static_cast<int>(deviceId[i]));
        }
        resObj.insert("deviceID", idArrary);
    }

    emit m_staticGeneralApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void GeneralApi::GetProductName_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;

        char productName[64];
        memcpy(&productName, &packet.packet.payloadAndCRC[0], sizeof(char) * 64);
        resObj.insert("productName", productName);
    }

    emit m_staticGeneralApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}

void GeneralApi::DeviceReboot_CB(DobotV3CmdInterCBParm packet){
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        isTimeOut = true;
    } else {
        isTimeOut = false;
        resObj.insert("result", "true");
    }

    emit m_staticGeneralApi->receiveData_signal(isTimeOut, resObj, packet.packet.seqNum);
}



