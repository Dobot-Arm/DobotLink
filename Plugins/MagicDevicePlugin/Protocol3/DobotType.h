#ifndef DOBOTTYPE_H
#define DOBOTTYPE_H

#include <QString>

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;

#ifndef Q_OS_LINUX
typedef long long          int64_t;
typedef unsigned long long uint64_t;
#endif

/*********************************************************************************************************
** API result
*********************************************************************************************************/
enum {
    SendCmd_Success,
    Connect_Interrupt,
    SendCmd_Failed
};

enum {
    PortStatus_Connected,
    PortStatus_UnConnected,
    PortStatus_Occupied,
    PortStatus_NotFound,
    PortStatus_Unknown
};

#define PORT_CONNECTED         "Connected"
#define PORT_UNCONNECTED    "UnConnected"
#define PORT_OCCUPIED             "Occupied"
#define PORT_NOTFOUND           "NotFound"
#define PORT_UNKNOWN            "UnKnown"


const QString SerialError[] = {
    "NOERROR",
    "ERROR_DEVICE_NOT_FOUND",
    "ERROR_DL_PERMISSION",
    "ERROR_DL_OPEN",
    "ERROR_DL_PARITY",
    "ERROR_DL_FRAMING",
    "ERROR_DL_BREAK_CONDITION",
    "ERROR_DL_WRITE",
    "ERROR_DL_READ",
    "ERROR_DL_RESOURCE",
    "ERROR_DL_UNSUPPORTED_OPERATION",
    "ERROR_DL_UNKOWN",
    "ERROR_DL_TIMEOUT",
    "ERROR_DL_NOT_OPENE"
};


typedef enum {
    SetRunningStateId = 0,
    GetRunningStateId = 0,
    SetRunningModeId = 1,
    GetRunningModeId = 1,
    SetBaseModeId = 3,
    SetBaseSpeedRatioId = 4,
    GetBaseSpeedRatioId = 4,
    SetMoveSpeedTimeId = 8,
    SetMoveSpeedDirectId = 9,
    SetMoveSpeedId = 10,
    GetMoveSpeedId = 10,
    SetRotateId = 11,
    SetMoveDistId = 12,
    SetMovePosId = 13,
    SetArcRadId = 15,
    SetArcCentId = 16,
    SetCoordClosedLoopId = 17,
    SetIncrementClosedLoopId = 18,
    SetLightRGBId = 20,
    SetLightPromptId = 21,
    SetBuzzerSoundId = 25,
    SetOriginPointId = 26,
    GetUltrasoundDataId = 30,
    GetSwitchStateId = 31,
    SetSpeedometerId = 33,
    GetSpeedometerId = 33,
    GetStallProtectionId = 34,
    GetOffGroundId = 35,
    GetBatteryVoltageId = 36,
    GetImuAngleId = 37,
    GetImuSpeedId = 38,
    GetAlarmInfoId = 40,
    CleanAlarmInfoId = 40,
    SetTraceAutoId = 50,
    SetTraceLineInfoId = 51,
    SetTraceSpeedId = 52,
    SetLineSensorStateId = 53,
    GetLineSensorStateId = 53,
    SetLineSensorAdjustId = 54,
    SetLineSensorDefaultId = 55,
    GetLineSensorDataId = 56,
    SetTracePidId = 57,
    SetTraceLoopId = 217,
    GetImgToArmXYId = 217,
    SetCmdQueueStartId = 240,
    SetCmdQueueStopId = 241,
    SetCmdQueueForcelyStopId = 242,
    CleanCmdQueueId = 245,
    GetCmdQueueCurrentIndexId = 246,
    GetCmdQueueAvailableSpaceId = 247,
} CmdID;

typedef enum {
    CBRunningStateId = 0x00,
    CBRunningModeId = 0x01,
    CBSetBaseModeId = 0x03,
    CBBaseSpeedRatioId = 0x04,
    CBSetMoveSpeedTimeId = 0x08,
    CBSetMoveSpeedDirectId = 0x09,
    CBMoveSpeedId = 0x0A,
    CBSetRotateId = 0x0B,
    CBSetMoveDistId = 0x0C,
    CBSetMovePosId = 0x0D,
    CBSetArcRadId = 0x0F,
    CBSetArcCentId = 0x10,
    CBSetCoordClosedLoopId = 0x11,
    CBSetIncrementClosedLoopId = 0x12,
    CBSetLightRGBId = 0x14,
    CBSetLightPromptId = 0x15,
    CBSetBuzzerSoundId = 0x19,
    CBSetOriginPointId = 0x1A,
    CBGetUltrasoundDataId = 0x1E,
    CBGetSwitchStateId = 0x1F,
    CBSetSpeedometerId = 0x21,
    CBGetSpeedometerId = 0x21,
    CBGetStallProtectionId = 0x22,
    CBGetOffGroundId = 0x23,
    CBGetBatteryVoltageId = 0x24,
    CBGetImuAngleId = 0x25,
    CBGetImuSpeedId = 0x26,
    CBAlarmInfoId = 0x28,
    CBSetTraceAutoId = 0x32,
    CBSetTraceLineInfoId = 0x33,
    CBSetTraceSpeedId = 0x34,
    CBLineSensorStateId = 0x35,
    CBSetLineSensorAdjustId = 0x36,
    CBSetLineSensorDefaultId = 0x37,
    CBGetLineSensorDataId = 0x38,
    CBSetTracePidId = 0x39,
    CBSetTraceLoopId = 0xD9,
    CBGetImgToArmXYId = 0xD9,
    CBSetCmdQueueStartId = 0xF0,
    CBSetCmdQueueStopId = 0xF1,
    CBSetCmdQueueForcelyStopId = 0xF2,
    CBCleanCmdQueueId = 0xF5,
    CBGetCmdQueueCurrentIndexId = 0xF6,
    CBGetCmdQueueAvailableSpaceId = 0xF7,
} CBID;

/** 摄像头命令集 **/
typedef  enum {
    GetCameraAngleId = 10,
    GetCameraColorId = 20,
    GetCameraObjId = 30,
    GetCameraTagId = 50,
    CameraRunModelId = 102,
    CameraCalibrationModeId = 103
}CameraID;

typedef enum {
    CBGetCameraAngleId = 0x0A,
    CBGetCameraColorId = 0x14,
    CBGetCameraObjId = 0x1E,
    CBGetCameraTagId = 0x32,
    CBCameraRunModelId = 0x66,
    CBCameraCalibrationModeId = 0x67
}CBCameraID;

enum {
    FILA_IDLE,           //耗材正常
    FILA_STOP,          //停止打印
    FILA_EOUT,          //耗材退出
    FILA_DETECT,     //耗材正在退出
    FILA_WAITOUT,   //耗材等待移除
    FILA_WAITIN,       //等待新耗材装入
    FILA_WAITHEAT,  //耗材加热
    FILA_EIN,              //等待耗材挤出
    FILA_RESUME,    //恢复打印
};

enum {
    PRINT_IDLE,           //打印停止
    PRINT_EXCUTE,    //打印中
    PRINT_PAUSE,      //打印暂停
};

#endif // DOBOTTYPE_H
