#ifndef DOBOTTYPE_H
#define DOBOTTYPE_H

#include <QString>

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;


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
    "NoError",
    "DeviceNotFoundError",
    "PermissionError",
    "OpenError",
    "ParityError",
    "FramingError",
    "BreakConditionError",
    "WriteError",
    "ReadError",
    "ResourceError",
    "UnsupportedOperationError",
    "UnknownError",
    "TimeoutError",
    "NotOpenError"
};


typedef enum {
    SendGcodeId = 0,
    GetLevellingInfoId = 1,
    GetCurrentPositionId = 2,
    GetCurrentTemperatureId = 3,
    GetCurrentPowerId = 4,
    GetCurrentTerminalTypeId = 5,
    GetCurrentRunningStatusId = 6,
    SetAutoLevellingParamsId = 7,
    GetCurrentZeroDeviationValueId = 9,
    GetCurrentMemoryStatusId = 10,
    GetCurrentPrintPageDataId = 11,
    GetCurrentInfoPageDataId = 12,
    GetFractureStatusId = 13,
    GetCurrentLanguageId = 14,
    SetCurrentLanguageId = 15,
    SetAutoShutdownId = 16,
    GetCurrentPageIDId = 19,
    GetCurrentFileNumId = 20,
    GetCurrentFileNameId = 21,
    SetFileTransferConfigId = 22,
    SendFileTransferConfigId = 23,
    CheckFileId = 24,
    CheckPackageId = 25,
    SetTCPClientId = 36,
    DisconnectWifiId = 37,
} CmdID;

typedef enum {
    CBSendGcodeId = 0x00,
    CBGetLevellingInfoId = 0x01,
    CBGetCurrentPositionId = 0x02,
    CBGetCurrentTemperatureId = 0x03,
    CBGetCurrentPowerId = 0x04,
    CBGetCurrentTerminalTypeId = 0x05,
    CBGetCurrentRunningStatusId = 0x06,
    CBSetAutoLevellingParamsId = 0x07,
    CBGetCurrentZeroDeviationValueId = 0x09,
    CBGetCurrentMemoryStatusId = 0x0A,
    CBGetCurrentPrintPageDataId = 0x0B,
    CBGetCurrentInfoPageDataId = 0x0C,
    CBGetFractureStatusId = 0x0D,
    CBGetCurrentLanguageId = 0x0E,
    CBSetCurrentLanguageId = 0x0F,
    CBSetAutoShutdownId = 0x10,
    CBGetCurrentPageIDId = 0x13,
    CBGetCurrentFileNumId = 0x14,
    CBGetCurrentFileNameId = 0x15,
    CBSetFileTransferConfigId = 0x16,
    CBSendFileTransferConfigId = 0x17,
    CBCheckFileId = 0x18,
    CBCheckPackageId = 0x19,
    CBSetTCPClientId = 0x24,
    CBDisconnectWifiId = 0x25,
} CBID;

enum {
    OK,             //执行正常
    FILEEXIST,      //文件已存在
    FILEERROR,     //文件错误
    MEMORYERROR,    //储存器错误
    MEMORYNOENOUGH, //储存空间不足
    SIZEERROR,      //文件写入异常；
    VERIFYERROR,    //校验码错误；
} errorCode;

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
