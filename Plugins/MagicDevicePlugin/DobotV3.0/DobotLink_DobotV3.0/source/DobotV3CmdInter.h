/********************** Copyright(c)***************************
** Shenzhen Yuejiang Technology Co., Ltd.
**
** http:// www.dobot.cc
**
**------File Info----------------------------------------------
** File name:               DobotV3CmdInter.h
** Latest modified date:    2020-05-20
** Latest version:          V1.0.0
** Description:             指令交互功能
**
**------------------------------------------------------------
** Created by:              Chen JunTong
** Created date:            2020-05-20
** Version:                 V1.0.0
** Descriptions:            指令交互功能
**
**------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*************************************************************/
#ifndef DobotV3CmdInter_h
#define DobotV3CmdInter_h
#ifdef __cplusplus
extern "C" {
#endif

#include "ProtocolFrame.h"
#include "DobotV3Format.h"

typedef enum {
    DobotV3CmdInter_SUCCESS = 0,              /* 成功 */
    DobotV3CmdInter_FAIL,                     /* 失败 */
    DobotV3CmdInter_NOGETSPACE,               /* 还没有获取指令空间 */
    DobotV3CmdInter_NOWRITE,                  /* 还没有写入数据 */
    DobotV3CmdInter_MATCHLOST,                /* 指令不存在相应的匹配规则 */
    DobotV3CmdInter_SENDREADY,                /* 指令发送就绪 */
    DobotV3CmdInter_SENDING,                  /* 发送中 */
    DobotV3CmdInter_TIMEOUT,                  /* 发送超时 */
    DobotV3CmdInter_NOSPACE,                  /* 没有可以使用的指令空间 */
    DobotV3CmdInter_HANDLEERR,                /* 无效句柄 */
    DobotV3CmdInter_PKGLENERR,                /* 数据包长度错误，超出数据包大小 */
    DobotV3CmdInter_NOACK,                    /* 指令不需要应答 */
} DobotV3CmdInter_StatusList;

#pragma pack(1)

typedef struct DobotV3CmdInterParm DobotV3CmdInterPARM;
typedef DobotV3CmdInterPARM *PDobotV3CmdInterPARM;


struct SDobotV3CmdInter_SendParm {
    uint8_t cbMark:1;
    uint8_t ackMark:1;
    uint8_t setParmMark:1;      /* 注意setParmMark为true时，repeatNum 和 timeOut才生效 */
    uint8_t repeatNum;
    uint32_t timeOut;
};
typedef struct SDobotV3CmdInter_SendParm DobotV3CmdInter_SendParm; /* DobotV3CmdInter_SendParm 类型 */
typedef DobotV3CmdInter_SendParm *PDobotV3CmdInter_SendParm;      /* PDobotV3CmdInter_SendParm 指针类型 */


/* MasterPacketStatus */
struct DobotV3CmdInter_PcaketParm {
    uint8_t userMark: 1;            /* 空间使用标记, DobotV3CmdInter_GetCmdSpace里面置位 */
    uint8_t writeMark: 1;           /* 写入标记， */
    uint8_t ruleMark: 1;            /* 匹配规则标记，true存在相应的匹配规则,false,不存在 */
    uint8_t sendMark: 1;            /* 发送标记 */
    uint8_t autoFreeMark:1;         /* 自动释放指令空间 */
    uint8_t sendStatus;             /* 发送状态 */
    void *handle;                   /* 句柄指针，与用户传入句柄对比，主要为了判断句柄的有效性 */

    PProtocolFrame_LinkPARM pLink;  /* 链路指针 */
    uint16_t length;                /* 数据包长度 */
    uint8_t ruleIndex;              /* 匹配规则索引 */
    uint32_t timeStart;             /* 起始时间 */

    DobotV3CmdInter_SendParm sendParm;
};
typedef struct DobotV3CmdInter_PcaketParm DobotV3CmdInter_PcaketPARM;
typedef DobotV3CmdInter_PcaketPARM *PDobotV3CmdInter_PcaketPARM;

/* 指令发送句柄 */
struct DobotV3CmdInter_Handle {
    PDobotV3CmdInterPARM pParm;                     /* 参数指针 */
    uint16_t index;                             /* 指令索引 */
    bool userMark;                              /* 使用标记 */
};
typedef struct DobotV3CmdInter_Handle DobotV3CmdInter_HANDLE;
typedef DobotV3CmdInter_HANDLE *PDobotV3CmdInter_HANDLE;

struct SDobotV3CmdInterCBParm {
    DobotV3Format_Packet packet;
    uint16_t length;
    uint8_t error;
};
typedef struct SDobotV3CmdInterCBParm DobotV3CmdInterCBParm;  /* DobotV3CmdInterCBParm 类型 */
typedef DobotV3CmdInterCBParm *PDobotV3CmdInterCBParm;        /* PDobotV3CmdInterCBParm 指针类型 */

typedef void (*DobotV3CmdInter_CB)(DobotV3CmdInterCBParm packet);

struct SDobotV3CmdInterCmdCBList {
    uint16_t id;
    /*************************************************************
    ** Function name:       SuccessCB
    ** Descriptions:        指令回调
    ** Input parameters:    packet:传递给用户的参数，具体查看 DobotV3CmdInterSuccessCB
    ** Output parameters:   None
    ** Returned             None
    *************************************************************/
    void (*CallBack)(DobotV3CmdInterCBParm packet);
};
typedef struct SDobotV3CmdInterCmdCBList DobotV3CmdInterCmdCBList; /* DobotV3CmdInterCmdCBList 类型 */
typedef DobotV3CmdInterCmdCBList *PDobotV3CmdInterCmdCBList;       /* PDobotV3CmdInterCmdCBList 指针类型 */

struct SDobotV3CmdInterCmdCB {
    /*************************************************************
    ** Function name:       GetId
    ** Descriptions:        获取指令ID
    ** Input parameters:    pReadPacket:接收的数据包
    **                      length:数据包长度
    ** Output parameters:   None
    ** Returned             ID
    *************************************************************/
    uint16_t (*GetId)(PDobotV3Format_Packet pPacket, uint16_t length);
    PDobotV3CmdInterCmdCBList pCBList;                          /* 指令回调列表 */
    uint16_t CBListSize;                                    /* 指令回调列表大小 */
};
typedef struct SDobotV3CmdInterCmdCB DobotV3CmdInterCmdCB; /* DobotV3CmdInterCmdCB 类型 */
typedef DobotV3CmdInterCmdCB *PDobotV3CmdInterCmdCB;      /* PDobotV3CmdInterCmdCB 指针类型 */

/* 发送指令匹配规则结构 */
struct DobotV3CmdInter_MatchStruct {
    /*************************************************************
    ** Function name:       Sort
    ** Descriptions:        匹配分类
    ** Input parameters:    pSendPacket:发送的数据包
    **                      length:数据包长度
    ** Output parameters:   None
    ** Returned             true,属于，false，不属于
    *************************************************************/
    uint8_t (*Sort)(PDobotV3Format_Packet pSendPacket, uint16_t length);
    /*************************************************************
    ** Function name:       Match
    ** Descriptions:        指令匹配
    ** Input parameters:    pSendPacket:发送的数据包
    **                      pReceivePacket：接收的数据包
    ** Output parameters:   None
    ** Returned             true匹配成功，false失败
    *************************************************************/
    uint8_t (*Match)(PDobotV3Format_Packet pSendPacket, PDobotV3Format_Packet pReadPacket);
    PDobotV3CmdInterCmdCB pCmdCB;
};
typedef struct DobotV3CmdInter_MatchStruct DobotV3CmdInter_MatchSTRUCT;
typedef DobotV3CmdInter_MatchSTRUCT *PDobotV3CmdInter_MatchSTRUCT;


/* FCSend参数 */
struct DobotV3CmdInterParm {
    PDobotV3CmdInter_MatchSTRUCT pMatchList;                    /* 匹配规则列表 */
    uint8_t matchListSize;                                  /* 匹配规则个数 */

    uint8_t *pPacketList;                                   /* 数据包列表 */
    PDobotV3CmdInter_PcaketPARM pPacketParmList;                /* 数据包参数列表 */
    uint16_t packetListSize;                                /* 列表大小 */
    uint16_t packetSize;                                    /* 数据包大小 */

    DobotV3CmdInter_SendParm sendParm;
};

#pragma pack()


/*************************************************************
** Function name:       DobotV3CmdInter_CMDCB_LS_EXPORT
** Descriptions:        定义指令的回调函数列表及大小参数
** Input parameters:    name:回调函数列表名称，须符合变量命名规范
**                      GetId:（DobotV3CmdInterCmdCB）获取ID的回调函数接口，具体查看 DobotV3CmdInterCmdCB
**                      sizeCBList:(uint16_t) 回调列表大小
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define DobotV3CmdInter_CMDCB_LS_EXPORT(name,GetId,sizeCBList)                              \
        DobotV3CmdInterCmdCBList gDobotV3CmdInterCmdCBList##name[sizeCBList];               \
        DobotV3CmdInterCmdCB name =                                                         \
        {                                                                                   \
            GetId,                                                                          \
            gDobotV3CmdInterCmdCBList##name,                                               \
            sizeCBList,                                                                     \
        };

/*************************************************************
** Function name:       DobotV3CmdInter_CMDCB_LS_EXTERN
** Descriptions:        声明指令的回调函数列表及大小参数
** Input parameters:    name:回调函数列表名称，须符合变量命名规范
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define DobotV3CmdInter_CMDCB_LS_EXTERN(name)                          \
        extern DobotV3CmdInterCmdCB name;

/*************************************************************
** Function name:       DobotV3CmdInter_CMDCB_EXPORT
** Descriptions:        定义指令的回调函数列表
** Input parameters:    name:回调函数列表名称，须符合变量命名规范
**                      GetId:（DobotV3CmdInterCmdCB）获取ID的回调函数接口，具体查看 DobotV3CmdInterCmdCB
**                      CBList：（DobotV3CmdInterCmdCBList）回调函数列表
**                          id：（uint16_t）指令的id
**                          CallBack：指令对应的回调函数，具体查看 DobotV3CmdInterCmdCBList
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define DobotV3CmdInter_CMDCB_EXPORT(name,GetId,CBList...)                                  \
        DobotV3CmdInterCmdCBList gDobotV3CmdInterCmdCBList##name[] =                        \
        {                                                                                   \
            CBList                                                                          \
        };                                                                                  \
        DobotV3CmdInterCmdCB name =                                                         \
        {                                                                                   \
            GetId,                                                                          \
            gDobotV3CmdInterCmdCBList##name,                                                \
            sizeof(gDobotV3CmdInterCmdCBList##name) / sizeof(DobotV3CmdInterCmdCBList),     \
        };

/*************************************************************
** Function name:       DobotV3CmdInter_CMDCB_EXTERN
** Descriptions:        声明指令回调参数
** Input parameters:    name:回调函数列表名称，须符合变量命名规范
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define DobotV3CmdInter_CMDCB_EXTERN(name)                          \
        extern DobotV3CmdInterCmdCB name;

/*************************************************************
** Function name:       DobotV3CmdInter_EXPORT
** Descriptions:        定义指令发送功能
** Input parameters:    funcName：(ProtocolFrame_FuncSTRUCT)功能名称,需符合变量命名规范
**                      packetNum：(uint8_t)同时发送的数据包个数
**                      packetSize：(uint16_t)数据包大小
**                      timeout：(uint32_t)默认单次超时时间
**                      repeatNum：(uint8_t)默认重发次数
**                      match...：匹配规则
**                          Sort：分类回调函数
**                          Match：匹配回调函数
**                          PDobotV3CmdInterCmdCB：指令回调参数指针
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define DobotV3CmdInter_EXPORT(funcName,packetNum,packetSize,timeout,repeatNum,match...)        \
        uint8_t gDobotV3CmdInter_PacketList##funcName[packetNum*packetSize];                    \
        DobotV3CmdInter_PcaketPARM gDobotV3CmdInter_PacketParmList##funcName[packetNum];        \
        DobotV3CmdInter_MatchSTRUCT gDobotV3CmdInter_MatchList##funcName[] =                    \
        {                                                                                       \
            match                                                                               \
        };                                                                                      \
        DobotV3CmdInterPARM gDobotV3CmdInterParm##funcName =                                    \
        {                                                                                       \
            gDobotV3CmdInter_MatchList##funcName,                                               \
            sizeof(gDobotV3CmdInter_MatchList##funcName)                                        \
            / sizeof(DobotV3CmdInter_MatchSTRUCT),                                              \
            gDobotV3CmdInter_PacketList##funcName,                                              \
            gDobotV3CmdInter_PacketParmList##funcName,                                          \
            packetNum,                                                                          \
            packetSize,                                                                         \
            .sendParm = {                                                                       \
                true,                                                                           \
                true,                                                                           \
                true,                                                                           \
                repeatNum,                                                                      \
                timeout,                                                                        \
            }                                                                                   \
        };                                                                                      \
        ProtocolFrame_FUNC_EXPORT(funcName,                                                     \
                                  (void*)&gDobotV3CmdInterParm##funcName,                       \
                                  DobotV3CmdInter_PacketCallBack,                               \
                                  DobotV3CmdInter_Loop)


extern void DobotV3CmdInter_PacketCallBack(PProtocolFrame_FuncSTRUCT pFuncStruct, PProtocolFrame_PacketPARM pPacketParm);
extern void DobotV3CmdInter_Loop(PProtocolFrame_FuncSTRUCT pFuncStruct, PProtocolFrame_PacketPARM pPacketParm);
/*************************************************************
** Function name:       DobotV3CmdInter_EXTERN
** Descriptions:        声明指令发送功能
** Input parameters:    funcName：(ProtocolFrame_FuncSTRUCT)功能名称,需符合变量命名规范
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define DobotV3CmdInter_EXTERN(funcName)                                                     \
        ProtocolFrame_FUNC_EXTERN(funcName);

struct SDobotV3CmdInterAPI {
    /*************************************************************
    ** Function name:       GetCmdSpace
    ** Descriptions:        获取指令空间
    ** Input parameters:    pHandle:句柄参数指针
    **                      pFuncStruct:功能参数结构体
    **                      autoFreeMark:是否需要自动释放指令空间
    ** Output parameters:   None
    ** Returned value:      SUCCESS 获取成功
    **                      NOSPACE 没有可以使用的指令空间
    ** Remarks:             None
    *************************************************************/
    uint8_t (*GetCmdSpace)(PDobotV3CmdInter_HANDLE pHandle, PProtocolFrame_FuncSTRUCT pFuncStruct, bool autoFreeMark);
    /*************************************************************
    ** Function name:       SendData
    ** Descriptions:        发送数据
    ** Input parameters:    pHandle：句柄
    **                      pLink：需要发送的链路
    **                      pPacket：需要发送的数据包 只需要填写
    **                      payloadLen：需要发送的 payloadLen长度
    ** Output parameters:   None
    ** Returned value:      None
    ** Remarks:             None
    *************************************************************/
    uint8_t (*SendData)(PDobotV3CmdInter_HANDLE pHandle,
                        PProtocolFrame_LinkPARM pLink,
                        PDobotV3Format_Packet pPacket,
                        uint16_t payloadLen);
    /*************************************************************
    ** Function name:       StopSend
    ** Descriptions:        停止发送数据
    ** Input parameters:    pHandle：需要停止的句柄
    ** Output parameters:   None
    ** Returned value:      None
    ** Remarks:             需要注意句柄必须为有效句柄
    *************************************************************/
    uint8_t (*StopSend)(PDobotV3CmdInter_HANDLE pHandle);
    /*************************************************************
    ** Function name:       GetSendAck
    ** Descriptions:        获取发送的应答，在没有收到应答的时候，返回相应的状态，
    **                      返回SUCCESS表示已经收到应答
    ** Input parameters:    pHandle：句柄指针
    **                      pPacket：存放应答的数据指针,指针空间大小必须大于一帧数据包的大小
    **                          只有在返回值为 SUCCESS 时才有效
    **                          如果不需要获取应答数据，指针可以为NULL
    ** Output parameters:   None
    ** Returned value:      StatusList
    *************************************************************/
    uint8_t (*GetSendAck)(PDobotV3CmdInter_HANDLE pHandle, PDobotV3Format_Packet pPacket);
    /*************************************************************
    ** Function name:       FreeCmdSpace
    ** Descriptions:        释放指令空间
    ** Input parameters:    pHandle:句柄参数指针
    ** Output parameters:   None
    ** Returned value:      None
    ** Remarks:             当指令存在回调函数时，指令空间会自动释放
    *************************************************************/
    uint8_t (*FreeCmdSpace)(PDobotV3CmdInter_HANDLE pHandle);
    /*************************************************************
    ** Function name:       SetSendParm
    ** Descriptions:        设置发送参数
    ** Input parameters:    pHandle:需要判断的句柄
    **                      pSendParm:需要发送的参数
    ** Output parameters:   None
    ** Returned value:      DobotV3CmdInter_SUCCESS 成功，其它错误
    ** Remarks:             注意 pSendParm 必须初始化成员后缀为Mark的相关参数
    *************************************************************/
    uint8_t (*SetSendParm)(PDobotV3CmdInter_HANDLE pHandle,
                           PDobotV3CmdInter_SendParm pSendParm);
    /*************************************************************
    ** Function name:       DobotV3CmdInter_SetSendParm
    ** Descriptions:        设置发送参数
    ** Input parameters:    pFuncStruct：功能指针
    **                      pSendParm：发送的参数
    ** Output parameters:   None
    ** Returned value:      注意 pSendParm 必须初始化成员后缀为Mark的相关参数
    *************************************************************/
    void (*SetDefultSendParm)(PProtocolFrame_FuncSTRUCT pFuncStruct,
                                PDobotV3CmdInter_SendParm pSendParm);
    /*************************************************************
    ** Function name:       RegisterCB
    ** Descriptions:        注册回调函数
    ** Input parameters:    pCmdCB:需要注册的回调函数列表
    **                      id：需要注册的回调函数id
    **                      CB：需要注册的回调函数指针
    ** Output parameters:   None
    ** Returned value:      true 注册成功，false 注册失败
    ** Remarks:             None
    *************************************************************/
    bool (*RegisterCB)(PDobotV3CmdInterCmdCB pCmdCB,
                       uint16_t id,
                       DobotV3CmdInter_CB CB);
    /*************************************************************
    ** Function name:       IsHandleEffect
    ** Descriptions:        判断句柄是否有效
    ** Input parameters:    pHandle:需要判断的句柄
    **                      pFuncStruct:功能参数结构体
    ** Output parameters:   None
    ** Returned value:      DobotV3CmdInteract_SUCCESS 有效
    **                      DobotV3CmdInteract_HANDLEERR 句柄错误
    ** Remarks:             None
    *************************************************************/
    uint8_t (*IsHandleEffect)(PDobotV3CmdInter_HANDLE pHandle, PProtocolFrame_FuncSTRUCT pFuncStruct);

};
typedef struct SDobotV3CmdInterAPI DobotV3CmdInterAPI; /* DobotV3CmdInterAPI 类型 */
typedef DobotV3CmdInterAPI *PDobotV3CmdInterAPI;       /* PDobotV3CmdInterAPI 指针类型 */

/* 指令交互API */
extern DobotV3CmdInterAPI gDobotV3CmdInterAPI;


#ifdef __cplusplus
}
#endif
#endif  /* DobotV3CmdInter_h */



