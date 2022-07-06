#ifndef MAGICIANGOCAMERAAPI_H
#define MAGICIANGOCAMERAAPI_H

#include <QObject>
#include <QJsonObject>
#include "DobotType.h"
#include "DP3Packet.h"

extern "C" {
#include "DobotV3API.h"
}

class CameraApi : public QObject
{
    Q_OBJECT
public:
    static CameraApi *instance();

    //处理发送的数据
    //机械臂mo末端摄像头
    void GetArmCameraAngle(uint32_t seqNum);
    void GetArmCameraColor(uint32_t seqNum);
    void GetArmCameraObj(uint32_t seqNum);
    void GetArmCameraTag(uint32_t seqNum);
    void SetArmCameraCalibrationMode(uint32_t seqNum, uint8_t isEnableCali);
    void GetArmCameraCalibrationMode(uint32_t seqNum);
    void SetArmCameraRunModel(uint32_t seqNum, uint8_t runModelIndex);
    void GetArmCameraRunModel(uint32_t seqNum);

    //小车底盘摄像头
    void GetCarCameraAngle(uint32_t seqNum);
    void GetCarCameraColor(uint32_t seqNum);
    void GetCarCameraObj(uint32_t seqNum);
    void GetCarCameraTag(uint32_t seqNum);
    void SetCarCameraRunModel(uint32_t seqNum, uint8_t runModelIndex);
    void GetCarCameraRunModel(uint32_t seqNum);

    void SetCarCameraCalibrationMode(uint32_t seqNum, uint8_t isEnableCali);
    void GetCarCameraCalibrationMode(uint32_t seqNum);

    //接受的回调函数
    static void GetArmCameraAngle_CB(DobotV3CmdInterCBParm packet);
    static void GetArmCameraColor_CB(DobotV3CmdInterCBParm packet);
    static void GetArmCameraObj_CB(DobotV3CmdInterCBParm packet);
    static void GetArmCameraTag_CB(DobotV3CmdInterCBParm packet);
    static void SetArmCameraCalibrationMode_CB(DobotV3CmdInterCBParm packet);
    static void ArmCameraCalibrationMode_CB(DobotV3CmdInterCBParm packet);
    static void SetArmCameraRunModel_CB(DobotV3CmdInterCBParm packet);
    static void ArmCameraRunModel_CB(DobotV3CmdInterCBParm packet);

    static void GetCarCameraAngle_CB(DobotV3CmdInterCBParm packet);
    static void GetCarCameraColor_CB(DobotV3CmdInterCBParm packet);
    static void GetCarCameraObj_CB(DobotV3CmdInterCBParm packet);
    static void GetCarCameraTag_CB(DobotV3CmdInterCBParm packet);
    static void SetCarCameraCalibrationMode_CB(DobotV3CmdInterCBParm packet);
    static void CarCameraCalibrationMode_CB(DobotV3CmdInterCBParm packet);
    static void SetCarCameraRunModel_CB(DobotV3CmdInterCBParm packet);
    static void CarCameraRunModel_CB(DobotV3CmdInterCBParm packet);


public:

#pragma pack(push)
#pragma pack(1)
    struct Obj_t{
        uint8_t x;//x坐标
        uint8_t y;//Y坐标
        uint8_t w;//物体W宽度
        uint8_t h;//物体H高度
        uint8_t id;//类型
        float rot;//旋转角度
    };
#pragma pack(pop)


#pragma pack(push)
#pragma pack(1)
    struct colorobj{
        uint8_t x;//x坐标
        uint8_t y;//Y坐标
        uint8_t w;//物体W宽度
        uint8_t h;//物体H高度
        uint8_t id;//类型
    };
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
    struct dlobj{
        uint8_t x;//x坐标
        uint8_t y;//Y坐标
        uint8_t w;//物体W宽度
        uint8_t h;//物体H高度
        uint8_t id;//类型
    };
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
    struct aprilobj{
        uint8_t x;//x坐标
        uint8_t y;//Y坐标
        uint8_t w;//物体W宽度
        uint8_t h;//物体H高度
        uint8_t id;//类型
        float rot;//旋转角度
    };
#pragma pack(pop)

signals:
    void receiveData_signal(bool isTimeOut, QJsonObject jsonObj, quint32 seqNum);
    void sendCmd_signal(DobotV3Format_Packet pPacket, quint16 payloadLen, bool isAck);

private:
    static CameraApi *m_staticCameraApi;
    DobotV3Format_Packet m_packFrame;

    CameraApi();
    ~CameraApi();
};



#endif // MAGICIANGOCAMERAAPI_H
