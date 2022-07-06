/********************** Copyright(c)***************************
** Shenzhen Yuejiang Technology Co., Ltd.
**
** http:// www.dobot.cc
**
**------File Info----------------------------------------------
** File name:               ProtocolFrame.h
** Latest modified date:    2019-12-16
** Latest version:          V2.0.0
** Description:             自定义协议框架
**
**------------------------------------------------------------
** Created by:              Chen JunTong
** Created date:            2019-09-19
** Version:                 V1.0.0
** Descriptions:            自定义协议框架
**
**------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
**------------------------------------------------------------
** ProtocolFrame Manual:
** 符号说明：
**              （NULL）  表示该参数可以为空
**              （NOFUNC）不可以在函数里面调用
**              （FUNC）  函数里面调用
**              （...）   不定长参数，按照一定的格式循环传递多个参数
**
** API接口列表：
**
**使用流程说明：
**              在周期为1ms的函数里面添加时基
**              ProtocolFrame_TIMEBASE                  （FUNC）
**
**              使用下列函数输出相应的框架和格式（不要放在函数里面）
**
*************************************************************/
#ifndef ProtocolFrame_h
#define ProtocolFrame_h

#include <stdint.h>
#include <stdbool.h>
#include "stddef.h"

/* 解析状态 */
#define ProtocolFrame_DEFALSE    0              /* 解析失败 */
#define ProtocolFrame_DETRUE     1              /* 解析成功 */
#define ProtocolFrame_DENEXT     2              /* 下一次解析 */

#pragma pack(1)

typedef struct ProtocolFrameParm ProtocolFramePARM;               /* 协议框架参数结构体类型*/
typedef ProtocolFramePARM *PProtocolFramePARM;                    /* 协议框架参数结构体指针类型*/

typedef struct ProtocolFrame_LinkParm ProtocolFrame_LinkPARM; /* 解析链路结构体类型 */
typedef ProtocolFrame_LinkPARM *PProtocolFrame_LinkPARM;      /* 解析链路结构体指针类型 */

typedef struct ProtocolFrame_FormatParm ProtocolFrame_FormatPARM; /* 解析的数据格式类型*/
typedef ProtocolFrame_FormatPARM *PProtocolFrame_FormatPARM;      /* 解析的数据格式指针类型*/

typedef struct ProtocolFrame_PacketParm ProtocolFrame_PacketPARM; /* 传递给用户的参数结构体类型 */
typedef ProtocolFrame_PacketPARM *PProtocolFrame_PacketPARM;      /* 传递给用户的参数结构体指针类型 */

/* 数据包数据 */
struct ProtocolFrame_PacketParm {
    PProtocolFrame_LinkPARM pLink;              /* 链路指针 */
    uint8_t *pPacket;                           /* 数据包指针 */
    uint16_t packetLength;                      /* 数据包长度 */
    uint8_t errCode;                            /* 错误代码 */
    uint8_t sendMark: 1;                        /* 数据发送标记，true发送，false不发送 */
    uint8_t exitCallPack: 1;                    /* 退出回调函数，true退出，false不退出 */
};

/* 解析步骤结构体 */
struct ProtocolFrame_StepParm {
    /*************************************************************
    ** Function name:       StepCheck
    ** Descriptions:        步骤校验，在接收到的数据个数大于步骤长度的时候调用
    ** Input parameters:    pData：当前接收到的所有数据
    **                      length:当前数据包长度
    ** Output parameters:   no
    ** min：                no
    ** Returned             返回值：ProtocolFrame_DEFALSE 校验失败
    **                              ProtocolFrame_DETRUE 校验成功
    *************************************************************/
    uint8_t (*StepCheck)(uint8_t *pData, uint16_t length);    /* 解析步骤校验 */
    /*************************************************************
    ** Function name:       GetLength
    ** Descriptions:        获取该步骤开始校验的长度
    ** Input parameters:    pData：当前接收到的所有数据
    **                      length:当前数据包长度
    ** Output parameters:   no
    ** min：                no
    ** Returned             该步骤校验的长度
    *************************************************************/
    uint16_t (*GetLength)(uint8_t *pData, uint16_t length);   /* 获取该步骤开始校验的长度 */
};
typedef struct ProtocolFrame_StepParm ProtocolFrame_StepPARM; /* 解析步骤结构体类型 */
typedef ProtocolFrame_StepPARM *PProtocolFrame_StepPARM;      /* 解析步骤结构体指针类型 */

/* 解析的数据格式 */
struct ProtocolFrame_FormatParm {
    /*************************************************************
    ** Function name:       ErrCodeRead
    ** Descriptions:        读取指令的错误代码
    ** Input parameters:    pPacketParm：数据包参数指针
    ** Output parameters:   no
    ** min：                no
    ** Returned             no
    *************************************************************/
    void (*ErrCodeRead)(PProtocolFrame_PacketPARM pPacketParm);
    /*************************************************************
    ** Function name:       ErrCodeSend
    ** Descriptions:        发送指令的错误代码
    ** Input parameters:    pPacketParm：数据包参数指针
    ** Output parameters:   no
    ** min：                no
    ** Returned             no
    *************************************************************/
    void (*ErrCodeSend)(PProtocolFrame_PacketPARM pPacketParm);
    uint8_t sof;                                                /* 前导符 */
    PProtocolFrame_StepPARM pStepList;                          /* 解析步骤指针 */
    uint16_t stepListSize;                                      /* 解析步骤个数 */
};



typedef struct ProtocolFrame_FuncStruct ProtocolFrame_FuncSTRUCT; /* 应用功能结构体类型 */
typedef ProtocolFrame_FuncSTRUCT *PProtocolFrame_FuncSTRUCT;      /* 应用功能结构体指针类型 */
/* 应用功能结构 */
struct ProtocolFrame_FuncStruct {
    /*************************************************************
    ** Function name:       PacketCallBack
    ** Descriptions:        解析到正确数据包的回调函数
    ** Input parameters:    pFuncStruct：应用功能结构指针
    **                      pPacketParm：数据包参数指针
    ** Output parameters:   no
    ** min：                no
    ** Returned             no
    *************************************************************/
    void (*PacketCallBack)(PProtocolFrame_FuncSTRUCT pFuncStruct, PProtocolFrame_PacketPARM pPacketParm);
    /*************************************************************
    ** Function name:       Loop
    ** Descriptions:        循环执行函数
    ** Input parameters:    pFuncStruct：应用功能结构指针
    **                      pPacketParm：数据包参数指针
    ** Output parameters:   no
    ** min：                no
    ** Returned             no
    *************************************************************/
    void (*Loop)(PProtocolFrame_FuncSTRUCT pFuncStruct, PProtocolFrame_PacketPARM pPacketParm);
    void *pFuncParmPtr;         /* 功能参数指针 */
};

/* 应用功能参数 */
struct ProtocolFrame_FuncParm {
    uint8_t loopMark: 1;
};
typedef struct ProtocolFrame_FuncParm ProtocolFrame_FuncPARM; /* 应用功能结构体类型 */
typedef ProtocolFrame_FuncPARM *PProtocolFrame_FuncPARM;      /* 应用功能结构体指针类型 */


/* 应用功能列表 */
struct ProtocolFrame_FuncList {
    PProtocolFrame_FuncSTRUCT *pFuncStructList;
    PProtocolFrame_FuncPARM pFuncParmList;
    uint8_t funcListSize;
};
typedef struct ProtocolFrame_FuncList ProtocolFrame_FuncLIST; /* 应用功能列表结构体类型 */
typedef ProtocolFrame_FuncLIST *PProtocolFrame_FuncLIST;      /* 应用功能列表结构体指针类型 */

/* 解析链路 */
struct ProtocolFrame_LinkParm {
    uint16_t deTimeOut;                                     /* 解包超时时间 */
    uint8_t *pDeMsgBuff;                                    /* 解析消息buf */
    uint32_t maxMsgNum;                                     /* 最大消息个数 */
    /*************************************************************
    ** Function name:       readData
    ** Descriptions:        读取数据
    ** Input parameters:    pData：存放读取数据的指针
    **                      length:读取的数据长度
    ** Output parameters:   no
    ** min：                no
    ** Returned             返回读取的数据个数
    *************************************************************/
    uint16_t (*readData)(uint8_t *pData, uint16_t length);               /* 读取数据函数 */
    /*************************************************************
    ** Function name:       SendData
    ** Descriptions:        发送数据
    ** Input parameters:    pData：存放读取数据的指针
    **                      length：发送的数据长度
    ** Output parameters:   no
    ** min：                no
    ** Returned             no
    *************************************************************/
    void (*SendData)(uint8_t *pData, uint16_t length);      /* 读取数据函数 */

    uint8_t enable: 1;                                      /* 链路使能 */
    uint8_t sofMark: 1;                                     /* 前导符解析标记,false未解析到，true已完成前导符解析 */

    PProtocolFrame_FormatPARM pFormat;                      /* 数据格式 */
    PProtocolFrame_FuncLIST pFuncList;                      /* 功能列表 */

    uint32_t msgReadIndex;                                  /* 读取的消息索引 */
    uint32_t msgWriteIndex;                                 /* 写入的消息索引 */
    uint8_t  deStep;                                        /* 当前解析步骤 */
    uint16_t deLenght;                                      /* 当前解析长度 */
    uint16_t deStepLength;                                  /* 步骤的解析长度 */
    uint32_t deStartTime;                                   /* 解包开始时间 */

};

/* 数据包解析参数结构体 */
struct ProtocolFrameParm {
    PProtocolFrame_LinkPARM *pLinkList;                  /* 解析链路表 */
    uint8_t linkMaxNum;                                  /* 解析链路最大个数 */
    uint8_t linkNum;                                     /* 已注册解析链路个数 */

    uint8_t *pPacket;                                    /* 数据包指针 */
    PProtocolFrame_PacketPARM pPacketParm;               /* 传递给用户的参数 */
};

#pragma pack()

extern uint32_t gProtocolFrame_TimeCNT;                  /* 时间计数 */
/*************************************************************
** Function name:       ProtocolFrame_TIMECNT
** Descriptions:        时基,放在周期为1ms的函数里面执行
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*************************************************************/
#define ProtocolFrame_TIMECNT(ms)                \
        gProtocolFrame_TimeCNT += (ms)

/*************************************************************
** Function name:       ProtocolFrame_FRAME_EXPORT
** Descriptions:        定义一个框架
** Input parameters:    frameName：（ProtocolFramePARM）框架名称,需符合变量命名规范
**                      packetMaxSize：（uint16_t）能够解析的数据包最大长度
**                                      该值需要设置为所有框架包含的链路里面数据包的最大值
**                                      即ProtocolFrame_LINK_EXPORT里面packetMaxSize的最大值
**                      linkMaxNum：(uint8_t)可以容纳的最大链路个数
** Output parameters:   None
** Returned value:      None
*************************************************************/
#define ProtocolFrame_FRAME_EXPORT(frameName,packetMaxSize,linkMaxNum)                  \
        uint8_t gProtocolFrame_Packet##frameName[packetMaxSize];                        \
        PProtocolFrame_LinkPARM gProtocolFrame_LinkList##frameName[linkMaxNum];         \
        ProtocolFrame_PacketPARM gProtocolFrame_packetParm##frameName;                  \
        ProtocolFramePARM frameName =                                                   \
        {                                                                               \
            gProtocolFrame_LinkList##frameName,                                         \
            linkMaxNum,                                                                 \
            0,                                                                          \
            gProtocolFrame_Packet##frameName,                                           \
            &gProtocolFrame_packetParm##frameName,                                      \
        };

/*************************************************************
** Function name:       ProtocolFrame_FRAME_EXTERN
** Descriptions:        声明框架
** Input parameters:    frameName：（ProtocolFramePARM）框架名称,需符合变量命名规范
** Output parameters:   None
** Returned value:      None
*************************************************************/
#define ProtocolFrame_FRAME_EXTERN(frameName)                                           \
        extern ProtocolFramePARM frameName;

/*************************************************************
** Function name:       ProtocolFrame_LINK_EXPORT
** Descriptions:        链路参数定义
** Input parameters:    linkName：(ProtocolFrame_LinkPARM)链路名称，需符合变量命名规范
**                      timeOut：（uint32_t）超时时间
**                      packetMaxSize：（uint16_t）能够解析的数据包最大长度
**                      ReadData：
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define ProtocolFrame_LINK_EXPORT(linkName,timeOut,packetMaxSize,ReadData,SendData)     \
        uint8_t gProtocolFrame_LinkDeMsgBuf##linkName[packetMaxSize];                   \
        ProtocolFrame_LinkPARM linkName = {                                             \
            timeOut,                                                                    \
            gProtocolFrame_LinkDeMsgBuf##linkName,                                      \
            packetMaxSize,                                                              \
            ReadData,                                                                   \
            SendData,                                                                   \
            true,                                                                       \
            false,                                                                      \
        };

/*************************************************************
** Function name:       ProtocolFrame_LINK_EXTERN
** Descriptions:        链路参数声明
** Input parameters:    linkName：(ProtocolFrame_LinkPARM)链路名称，需符合变量命名规范
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define ProtocolFrame_LINK_EXTERN(linkName)                                             \
        extern ProtocolFrame_LinkPARM linkName;

/*************************************************************
** Function name:       ProtocolFrame_FORMAT_EXPORT
** Descriptions:        解析格式定义
** Input parameters:    formatName（ProtocolFrame_FormatPARM）解析格式名称，需符合变量命名规范
**                      ErrCodeRead：错误代码读取回调函数，具体查看 ProtocolFrame_FormatPARM
**                      ErrCodeSend：错误代码发送回调函数，具体查看 ProtocolFrame_FormatPARM
**                      sof：(uint8_t)数据包前导符
**                      deStep:解析步骤
**                          StepCheck：校验回调函数，具体查看 ProtocolFrame_StepPARM
**                          GetLength：获取校验长度回调函数，具体查看 ProtocolFrame_StepPARM
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define ProtocolFrame_FORMAT_EXPORT(formatName,                                             \
                                    ErrCodeRead,ErrCodeSend,                                \
                                    sof,deStep...)                                          \
        ProtocolFrame_StepPARM gProtocol_FormatDeStep##formatName[] =                       \
        {                                                                                   \
            deStep                                                                          \
        };                                                                                  \
        ProtocolFrame_FormatPARM formatName =                                               \
        {                                                                                   \
            ErrCodeRead,                                                                    \
            ErrCodeSend,                                                                    \
            sof,                                                                            \
            gProtocol_FormatDeStep##formatName,                                             \
            sizeof(gProtocol_FormatDeStep##formatName) / sizeof(ProtocolFrame_StepPARM)     \
        };

/*************************************************************
** Function name:       ProtocolFrame_FORMAT_EXTERN
** Descriptions:        声明解析格式
** Input parameters:    formatName（ProtocolFrame_FormatPARM）解析格式名称，需符合变量命名规范
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define ProtocolFrame_FORMAT_EXTERN(formatName)                                             \
        extern ProtocolFrame_FormatPARM formatName;

/*************************************************************
** Function name:       ProtocolFrame_FUNC_EXPORT
** Descriptions:        定义功能参数
** Input parameters:    funcName：（ProtocolFrame_FuncSTRUCT）功能参数名称，需符合变量命名规范
**                      pFuncParmPtr：（void*）用户功能参数指针，指向用户参数存储区域
**                      PacketCallBack：用户数据包回调函数，具体查看 ProtocolFrame_FuncSTRUCT
**                      Loop：框架循环执行回调函数，具体查看 ProtocolFrame_FuncSTRUCT
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define ProtocolFrame_FUNC_EXPORT(funcName,pFuncParmPtr,PacketCallBack,Loop)    \
        ProtocolFrame_FuncSTRUCT funcName =                                     \
        {                                                                       \
            PacketCallBack,                                                     \
            Loop,                                                               \
            pFuncParmPtr,                                                       \
        };

/*************************************************************
** Function name:       ProtocolFrame_FUNC_EXTERN
** Descriptions:        声明功能参数
** Input parameters:    funcName：（ProtocolFrame_FuncSTRUCT）功能参数名称，需符合变量命名规范
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define ProtocolFrame_FUNC_EXTERN(funcName)                             \
        extern ProtocolFrame_FuncSTRUCT funcName;

/*************************************************************
** Function name:       ProtocolFrame_FUNCLIST_EXPORT
** Descriptions:        定义功能列表参数
** Input parameters:    funcListName：（ProtocolFrame_FuncLIST）功能列表名称，需符合变量命名规范
**                      pFuncName：(PProtocolFrame_FuncSTRUCT)功能名称指针,不定长参数
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define ProtocolFrame_FUNCLIST_EXPORT(funcListName,pFuncName...)                    \
        PProtocolFrame_FuncSTRUCT gProtocolFrame_FuncStructList##funcListName[] =   \
        {                                                                           \
            pFuncName                                                               \
        };                                                                          \
        ProtocolFrame_FuncPARM gProtocolFrame_FuncStrucParm##funcListName           \
        [                                                                           \
            sizeof(gProtocolFrame_FuncStructList##funcListName)                     \
            / sizeof(PProtocolFrame_FuncSTRUCT)                                     \
        ];                                                                          \
        ProtocolFrame_FuncLIST funcListName =                                       \
        {                                                                           \
            gProtocolFrame_FuncStructList##funcListName,                            \
            gProtocolFrame_FuncStrucParm##funcListName,                             \
            sizeof(gProtocolFrame_FuncStructList##funcListName)                     \
            / sizeof(PProtocolFrame_FuncSTRUCT),                                    \
        };

/*************************************************************
** Function name:       ProtocolFrame_FUNCLIST_EXTERN
** Descriptions:        声明功能列表
** Input parameters:    funcListName：（ProtocolFrame_FuncLIST）功能列表名称，需符合变量命名规范
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define ProtocolFrame_FUNCLIST_EXTERN(funcListName)                                 \
        extern ProtocolFrame_FuncLIST funcListName;

/* 协议API列表 */
struct SProtocolFrameAPI {
    /*************************************************************
    ** Function name:       GetStartTime
    ** Descriptions:        获取起始时间
    ** Input parameters:    None
    ** Output parameters:   None
    ** Returned value:      返回当前时间
    ** Remarks:             None
    *************************************************************/
    uint32_t (*GetStartTime)(void);
    /*************************************************************
    ** Function name:       IsTimeOut
    ** Descriptions:        判断是否超时
    ** Input parameters:    timeOut：超时时间
    **                      timeStart：起始时间
    ** Output parameters:   None
    ** Returned value:      true 超时，false 未超时
    ** Remarks:             None
    *************************************************************/
    bool (*IsTimeOut)(uint32_t timeOut, uint32_t timeStart);
    /*************************************************************
    ** Function name:       GetTimeElapse
    ** Descriptions:        距离timeStart的时间
    ** Input parameters:    None
    ** Output parameters:   None
    ** Returned value:      返回时间间隔
    ** Remarks:             None
    *************************************************************/
    uint32_t (*GetTimeElapse)(uint32_t timeStart);
    /*************************************************************
    ** Function name:       GetLinkIsEnable
    ** Descriptions:        获取链路是否使能
    ** Input parameters:    pLink：需要获取的链路
    ** Output parameters:   None
    ** Returned value:      true 使能，false 禁能
    ** Remarks:             None
    *************************************************************/
    bool (*GetLinkIsEnable)(PProtocolFrame_LinkPARM pLink);
    /*************************************************************
    ** Function name:       SetLinkIsEnable
    ** Descriptions:        设置链路是否使能
    ** Input parameters:    pLink：需要设置的链路
    **                      status：状态，true使能，false禁能
    ** Output parameters:   None
    ** Returned value:      None
    ** Remarks:             None
    *************************************************************/
    void (*SetLinkIsEnable)(PProtocolFrame_LinkPARM pLink, bool status);
    /*************************************************************
    ** Function name:       LinkEnableOnly
    ** Descriptions:        仅使能 协议框架 指定链路
    ** Input parameters:    pFrame：框架参数结构体指针
    **                      pLink：不禁能的链路
    ** Output parameters:   no
    ** min：                no
    ** Returned             true，成功
    **                      false，失败
    *************************************************************/
    void (*SetLinkEnableOnly)(PProtocolFramePARM pFrame, PProtocolFrame_LinkPARM pLink);
    /*************************************************************
    ** Function name:       LinkDisableOnly
    ** Descriptions:        仅禁能 协议框架 指定链路
    ** Input parameters:    pFrame：框架参数结构体指针
    **                      pLink：不禁能的链路
    ** Output parameters:   no
    ** min：                no
    ** Returned             true，成功
    **                      false，失败
    *************************************************************/
    void (*SetLinkDisableOnly)(PProtocolFramePARM pFrame, PProtocolFrame_LinkPARM pLink);
    /*************************************************************
    ** Function name:       ConnectFLFF
    ** Descriptions:        连接Frame，link，format，func
    ** Input parameters:    pFrame：框架参数结构体指针
    **                      pLink:链路指针
    **                      pFormat:解析格式
    **                      pFuncList：功能列表
    ** Output parameters:   no
    ** min：                no
    ** Returned             true，成功
    **                      false，失败
    *************************************************************/
    void (*ConnectFLFF)(PProtocolFramePARM pFrame,
                        PProtocolFrame_LinkPARM pLink,
                        PProtocolFrame_FormatPARM pFormat,
                        PProtocolFrame_FuncLIST pFuncList);
    /*************************************************************
    ** Function name:       exec
    ** Descriptions:        协议包框架的执行程序
    ** Input parameters:    pFrame：框架参数结构体指针
    ** Output parameters:   no
    ** min：                no
    ** Returned             no
    *************************************************************/
    void (*Exec)(PProtocolFramePARM pFrame);
};
typedef struct SProtocolFrameAPI ProtocolFrameAPI;  /* ProtocolFrameAPI 类型 */
typedef ProtocolFrameAPI *PProtocolFrameAPI;        /* PProtocolFrameAPI 指针类型 */

/* 协议框架API */
extern ProtocolFrameAPI gProtocolFrameAPI;

#endif  /* ProtocolFrame_h */



