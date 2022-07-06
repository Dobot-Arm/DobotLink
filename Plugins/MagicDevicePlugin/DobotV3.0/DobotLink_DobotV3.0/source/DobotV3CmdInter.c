/********************** Copyright(c)***************************
** Shenzhen Yuejiang Technology Co., Ltd.
**
** http:// www.dobot.cc
**
**------File Info----------------------------------------------
** File name:               DobotV3CmdInter.c
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
#include "DobotV3CmdInter.h"
#include "string.h"

/*************************************************************
** Function name:       GETPKGADDR
** Descriptions:        获取数据包地址
** Input parameters:    pParm：（PDobotV3CmdInterPARM）指令交互参数指针
**                      index：(uint8_t)索引
** Output parameters:   None
** Returned value:      None
*************************************************************/
#define GETPKGADDR(pParm,index)            \
    (pParm->pPacketList + (index) * pParm->packetSize)

/*************************************************************
** Function name:       PACKKETPARM_FILL
** Descriptions:        填充PACKKETPARM参数
** Input parameters:    pPacketParm：(PProtocolFrame_PacketPARM) 数据包参数指针
**                      Link：(PProtocolFrame_LinkPARM) 数据链路指针
**                      PPacket：(uint8_t*)数据包指针
**                      PacketLength：(uint16_t)数据包长度
** Output parameters:   None
** Returned value:      None
*************************************************************/
#define PACKKETPARM_FILL(pPacketParm,Link,PPacket,PacketLength)         \
    pPacketParm->pLink = Link;                                          \
    memcpy(pPacketParm->pPacket,PPacket,PacketLength);                  \
    pPacketParm->packetLength = PacketLength;                           \
    pPacketParm->errCode = 0;                                           \
    pPacketParm->sendMark = true

/*************************************************************
** Function name:       ISHANDLEERR
** Descriptions:        判断句柄是否出错
** Input parameters:    pHandle:(PHANDLE)需要判断的句柄
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define ISHANDLEERR(pHandle)                                                    \
    if(pHandle->index >= pHandle->pParm->packetListSize){                       \
        return DobotV3CmdInter_HANDLEERR;                                           \
    }                                                                           \
    if(pHandle->pParm->pPacketParmList[pHandle->index].userMark != true){       \
        return DobotV3CmdInter_HANDLEERR;                                           \
    }                                                                           \
    if(pHandle->pParm->pPacketParmList[pHandle->index].handle != pHandle){      \
        return DobotV3CmdInter_HANDLEERR;                                           \
    }

/*************************************************************
** Function name:       FILL_SENDPARM
** Descriptions:        填充发送参数
** Input parameters:    pFillSendParm：需要填充的发送参数指针
**                      pSendParm：发送参数指针
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define FILL_SENDPARM(pFillSendParm,pSendParm)                            \
    (pFillSendParm)->cbMark = (pSendParm)->cbMark;                        \
    (pFillSendParm)->ackMark = (pSendParm)->ackMark;                      \
    (pFillSendParm)->setParmMark = (pSendParm)->setParmMark;              \
    (pFillSendParm)->repeatNum = (pSendParm)->repeatNum ;                 \
    (pFillSendParm)->timeOut = (pSendParm)->timeOut;                          


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
static uint8_t GetCmdSpace(PDobotV3CmdInter_HANDLE pHandle, PProtocolFrame_FuncSTRUCT pFuncStruct, bool autoFreeMark)
{
    pHandle->pParm = (PDobotV3CmdInterPARM)pFuncStruct->pFuncParmPtr;
    for(uint16_t index = 0; index < pHandle->pParm->packetListSize; index++) {
        PDobotV3CmdInter_PcaketPARM pPkParm = &pHandle->pParm->pPacketParmList[index];
        if(pPkParm->userMark == true) {
            continue;
        }
        pPkParm->userMark = true;
        pPkParm->autoFreeMark = autoFreeMark;
        FILL_SENDPARM(&pPkParm->sendParm,&pHandle->pParm->sendParm)
        pHandle->index = index;
        pHandle->pParm->pPacketParmList[index].handle = pHandle;
        return DobotV3CmdInter_SUCCESS;
    }
    pHandle->userMark = false;
    return DobotV3CmdInter_NOSPACE;
}

/*************************************************************
** Function name:       ClearPkgParm
** Descriptions:        清楚指令参数，
** Input parameters:    pHandle:句柄参数指针
** Output parameters:   None
** Returned value:      None
** Remarks:             这里接口同时会释放指令空间
*************************************************************/
static void ClearPkgParm(PDobotV3CmdInter_PcaketPARM PkgParm)
{
    PkgParm->handle = NULL;
    PkgParm->writeMark = false;
    PkgParm->ruleMark = false;
    PkgParm->sendMark = false;
    PkgParm->userMark = false;
}

/*************************************************************
** Function name:       FreeCmdSpace
** Descriptions:        释放指令空间
** Input parameters:    pHandle:句柄参数指针
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
static uint8_t FreeCmdSpace(PDobotV3CmdInter_HANDLE pHandle)
{
    ISHANDLEERR(pHandle)
    ClearPkgParm(&pHandle->pParm->pPacketParmList[pHandle->index]);
    return DobotV3CmdInter_SUCCESS;
}

/*************************************************************
** Function name:       RuleSort
** Descriptions:        数据包发送规则分类
** Input parameters:    pParm：指令交互参数指针
**                      index：索引
**                      pData：需要发送的数据包指针
**                      len:发送的数据包长度
** Output parameters:   None
** Returned value:      SUCCESS,成功，MATCHLOST，失败
*************************************************************/
static uint8_t RuleSort(PDobotV3CmdInterPARM pParm,
                        uint8_t index,
                        PDobotV3Format_Packet pData,
                        uint16_t len)

{
    uint8_t matchIndex = 0;
    uint8_t listNum = pParm->matchListSize;
    for(matchIndex = 0; matchIndex < listNum; matchIndex++) {
        if(pParm->pMatchList[matchIndex].Sort(pData, len) == true) {
            pParm->pPacketParmList[index].ruleIndex = matchIndex;
            pParm->pPacketParmList[index].ruleMark = true;
            return DobotV3CmdInter_SUCCESS;
        }
    }
    pParm->pPacketParmList[index].ruleMark = false;
    return DobotV3CmdInter_MATCHLOST;
}


/*************************************************************
** Function name:       WritePacket
** Descriptions:        写入数据包到缓存
** Input parameters:    pHandle：句柄
**                      pLink：需要发送的链路
**                      pData：需要发送的数据指针
**                      len：需要发送的数据长度
** Output parameters:   None
** Returned value:      true成功，false失败
*************************************************************/
static uint8_t WritePacket(PDobotV3CmdInter_HANDLE pHandle,
                           PProtocolFrame_LinkPARM pLink,
                           PDobotV3Format_Packet pPacket,
                           uint16_t length)
{
    PDobotV3CmdInterPARM pParm = (PDobotV3CmdInterPARM)pHandle->pParm;
    PDobotV3CmdInter_PcaketPARM pPkParm = &pParm->pPacketParmList[pHandle->index];
    if(pParm->packetSize < length) {
        return DobotV3CmdInter_PKGLENERR;
    }
    memcpy(GETPKGADDR(pParm, pHandle->index), pPacket, length);
    pPkParm->pLink = pLink;
    pPkParm->length = length;
    pPkParm->timeStart = gProtocolFrameAPI.GetStartTime() - pPkParm->sendParm.timeOut;       /* 保证第一次能发送 */
    pPkParm->sendStatus = DobotV3CmdInter_SENDING;
    pPkParm->writeMark = true;
    return DobotV3CmdInter_SUCCESS;
}

/*************************************************************
** Function name:       IsHandleEffect
** Descriptions:        判断句柄是否有效
** Input parameters:    pHandle:需要判断的句柄
**                      pFuncStruct:功能参数结构体
** Output parameters:   None
** Returned value:      CmdInteract_SUCCESS 有效
**                      CmdInteract_HANDLEERR 句柄错误
** Remarks:             None
*************************************************************/
static uint8_t IsHandleEffect(PDobotV3CmdInter_HANDLE pHandle, PProtocolFrame_FuncSTRUCT pFuncStruct)
{
    if(pHandle->pParm != (PDobotV3CmdInterPARM)pFuncStruct->pFuncParmPtr) {
        return DobotV3CmdInter_HANDLEERR;
    }
    ISHANDLEERR(pHandle);
    return DobotV3CmdInter_SUCCESS;
}

/*************************************************************
** Function name:       DobotV3CmdInter_SetSendParm
** Descriptions:        设置默认发送参数，在GetCmdSpace时生效
** Input parameters:    pFuncStruct：功能指针
**                      pSendParm：发送的参数
** Output parameters:   None
** Returned value:      注意 pSendParm 必须初始化成员后缀为Mark的相关参数
*************************************************************/
static void SetDefultSendParm(PProtocolFrame_FuncSTRUCT pFuncStruct,
                                PDobotV3CmdInter_SendParm pSendParm)
{
    PDobotV3CmdInterPARM pParm = (PDobotV3CmdInterPARM)pFuncStruct->pFuncParmPtr;
    PDobotV3CmdInter_SendParm pDefaultSendParm = &pParm->sendParm;
    if(pSendParm->setParmMark == true){
        pDefaultSendParm->setParmMark = pSendParm->setParmMark;
        pDefaultSendParm->repeatNum = pSendParm->repeatNum;
        pDefaultSendParm->timeOut = pSendParm->timeOut;
    }
    pDefaultSendParm->cbMark = pSendParm->cbMark;
    pDefaultSendParm->ackMark = pSendParm->ackMark;
}

/*************************************************************
** Function name:       SetSendParm
** Descriptions:        设置发送参数，在GetCmdSpace之后生效
** Input parameters:    pHandle:需要判断的句柄
**                      pSendParm:需要发送的参数
** Output parameters:   None
** Returned value:      DobotV3CmdInter_SUCCESS 成功，其它错误
** Remarks:             注意 pSendParm 必须初始化成员后缀为Mark的相关参数
*************************************************************/
static uint8_t SetSendParm(PDobotV3CmdInter_HANDLE pHandle,
                           PDobotV3CmdInter_SendParm pSendParm)
{
    /* 判断句柄是否有效 */
    ISHANDLEERR(pHandle)
    PDobotV3CmdInterPARM pParm = (PDobotV3CmdInterPARM)pHandle->pParm;
    PDobotV3CmdInter_SendParm pPacSendParm = &pParm->pPacketParmList[pHandle->index].sendParm;
    if(pSendParm->setParmMark == true){
        pPacSendParm->setParmMark = pSendParm->setParmMark;
        pPacSendParm->repeatNum = pSendParm->repeatNum;
        pPacSendParm->timeOut = pSendParm->timeOut;
    }
    pPacSendParm->cbMark = pSendParm->cbMark;
    pPacSendParm->ackMark = pSendParm->ackMark;
    return DobotV3CmdInter_SUCCESS;
}

/*************************************************************
** Function name:       SendData
** Descriptions:        发送数据
** Input parameters:    pHandle：句柄
**                      pLink：需要发送的链路
**                      pPacket：需要发送的数据包 只需要填写
**                      payloadLen：需要发送的 payloadLen 长度
** Output parameters:   None
** Returned value:      DobotV3CmdInter_SUCCESS 成功，其它错误
** Remarks:             None
*************************************************************/
static uint8_t SendData(PDobotV3CmdInter_HANDLE pHandle,
                        PProtocolFrame_LinkPARM pLink,
                        PDobotV3Format_Packet pPacket,
                        uint16_t payloadLen)
{
    uint8_t result = 0;
    uint16_t len = 0;
    /* 判断句柄是否有效 */
    ISHANDLEERR(pHandle)
    pHandle->pParm->pPacketParmList[pHandle->index].sendStatus = DobotV3CmdInter_SENDREADY;
    /* 指令数据包写入缓存 */
    len = DobotV3Format_PacketEncoder(pPacket, payloadLen);
    result = WritePacket(pHandle, pLink, pPacket, len);
    if(result != DobotV3CmdInter_SUCCESS) {
        return result;
    }
    /* 指令规则分类 */
    result = RuleSort(pHandle->pParm, pHandle->index, pPacket, len);
    pHandle->pParm->pPacketParmList[pHandle->index].sendMark = true;
    return result;
}


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
static uint8_t GetSendAck(PDobotV3CmdInter_HANDLE pHandle, PDobotV3Format_Packet pPacket)
{
    ISHANDLEERR(pHandle)
    PDobotV3CmdInter_PcaketPARM pPkParm = &pHandle->pParm->pPacketParmList[pHandle->index];
    uint8_t sendStatus = pPkParm->sendStatus;
    if(pPkParm->userMark == false) {
        return DobotV3CmdInter_NOGETSPACE;
    }
    if(pPkParm->writeMark == false) {
        return DobotV3CmdInter_NOWRITE;
    }
    if(pPkParm->ruleMark == false) {
        if(sendStatus == DobotV3CmdInter_TIMEOUT) {
            return DobotV3CmdInter_TIMEOUT;
        }
        return DobotV3CmdInter_MATCHLOST;
    }
    if(sendStatus == DobotV3CmdInter_SUCCESS) {
        if(pPacket != NULL) {
            memcpy(pPacket, GETPKGADDR(pHandle->pParm, pHandle->index), pPkParm->length);
        }
    }
    return sendStatus;
}

/*************************************************************
** Function name:       CmdSuccessCBHandle
** Descriptions:        指令回调函数处理
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
static uint8_t CmdCallBackHandle(PDobotV3CmdInterPARM pParm,
                                 PDobotV3CmdInter_PcaketPARM pPkParm,
                                 DobotV3CmdInterCBParm CBParm)
{
    /* 判断是否存在指令回调函数列表 */
    PDobotV3CmdInterCmdCB pCmdCB = pParm->pMatchList[pPkParm->ruleIndex].pCmdCB;
    /* 不存在回调函数列表 */
    if(pCmdCB == NULL) {
        return false;
    }
    uint16_t id  = pCmdCB->GetId(&CBParm.packet, CBParm.length);
    for(uint16_t cir = 0; cir < pCmdCB->CBListSize; cir++) {
        if(id != pCmdCB->pCBList[cir].id) {
            continue;
        }
        if(pCmdCB->pCBList[cir].CallBack == NULL) {
            continue;
        }
        pCmdCB->pCBList[cir].CallBack(CBParm);
        return true;
    }
    return false;
}

/*************************************************************
** Function name:       StopSend
** Descriptions:        停止发送数据
** Input parameters:    pHandle：需要停止的句柄
** Output parameters:   None
** Returned value:      CmdInteract_SUCCESS 停止
**                      CmdInteract_HANDLEERR 句柄错误
** Remarks:             需要注意句柄必须为有效句柄
*************************************************************/
static uint8_t StopSend(PDobotV3CmdInter_HANDLE pHandle)
{
    ISHANDLEERR(pHandle)
    PDobotV3CmdInter_PcaketPARM pPkParm = &pHandle->pParm->pPacketParmList[pHandle->index];
    ClearPkgParm(pPkParm);
    return DobotV3CmdInter_SUCCESS;
}

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
static bool RegisterCB(PDobotV3CmdInterCmdCB pCmdCB,
                       uint16_t id,
                       DobotV3CmdInter_CB CB)
{
    /* 查找id是否已经存在 */
    for(uint16_t cir = 0; cir < pCmdCB->CBListSize; cir++) {
        if(pCmdCB->pCBList[cir].id == id) {
            pCmdCB->pCBList[cir].CallBack = CB;
            return true;
        }
    }
    /* 获取未使用空间 */
    for(uint16_t cir = 0; cir < pCmdCB->CBListSize; cir++) {
        if(pCmdCB->pCBList[cir].CallBack == NULL) {
            pCmdCB->pCBList[cir].id = id;
            pCmdCB->pCBList[cir].CallBack = CB;
            return true;
        }
    }
    return false;
}

/*************************************************************
** Function name:       DobotV3CmdInter_PacketCallBack
** Descriptions:        解析到正确数据包的回调函数
** Input parameters:    pFuncStruct：应用功能结构指针
**                      pPacketParm：数据包参数指针
** Output parameters:   no
** min：                no
** Returned             no
*************************************************************/
void DobotV3CmdInter_PacketCallBack(PProtocolFrame_FuncSTRUCT pFuncStruct, PProtocolFrame_PacketPARM pPacketParm)
{
    PDobotV3CmdInterPARM pParm = (PDobotV3CmdInterPARM)pFuncStruct->pFuncParmPtr;
    PDobotV3CmdInter_PcaketPARM pPkParm = NULL;
    uint16_t index = 0;
    uint8_t listSize = pParm->packetListSize;
    for(index = 0; index < listSize; index++) {
        pPkParm = &pParm->pPacketParmList[index];
        if(pPkParm->sendMark == false) {
            continue;
        }
        if(pPkParm->ruleMark == false) {
            continue;
        }
        /* 匹配链路 */
        if(pPacketParm->pLink != pPkParm->pLink) {
            continue;
        }
        if(pParm->pMatchList[pPkParm->ruleIndex].Match((PDobotV3Format_Packet)GETPKGADDR(pParm, index), (PDobotV3Format_Packet)pPacketParm->pPacket) == false) {
            continue;
        }
        /* 判断指令回调是否执行成功，如果执行了指令回调，则自动释放指令空间 */
        DobotV3CmdInterCBParm SuccessCB = {
            .length = pPacketParm->packetLength,
            .error = DobotV3CmdInter_SUCCESS,
        };
        memcpy(&SuccessCB.packet, pPacketParm->pPacket, pPacketParm->packetLength);
        /* 判断是否需要指令回调 */
        if(pPkParm->sendParm.cbMark == true && pPkParm->sendParm.ackMark == true){
            if(CmdCallBackHandle(pParm, pPkParm, SuccessCB) == true) {
                /* 判断是否需要自动释放指令空间 */
                if(pPkParm->autoFreeMark == true) {
                    ClearPkgParm(pPkParm);
                }
                pPacketParm->exitCallPack = true;
                break;
            }
        }
        pPkParm->length = pPacketParm->packetLength;
        memcpy(GETPKGADDR(pParm, index), pPacketParm->pPacket, pPacketParm->packetLength);
        pPkParm->sendMark = false;
        pPkParm->sendStatus = DobotV3CmdInter_SUCCESS;
        pPacketParm->exitCallPack = true;
        /* 判断是否需要自动释放指令空间 */
        if(pPkParm->autoFreeMark == true) {
            ClearPkgParm(pPkParm);
        }
        break;
    }
}

/*************************************************************
** Function name:       DobotV3CmdInter_Loop
** Descriptions:        循环执行函数
** Input parameters:    pFuncStruct：应用功能结构指针
**                      pPacketParm：数据包参数指针
** Output parameters:   no
** min：                no
** Returned             no
*************************************************************/
void DobotV3CmdInter_Loop(PProtocolFrame_FuncSTRUCT pFuncStruct, PProtocolFrame_PacketPARM pPacketParm)
{
    static uint8_t index = 0;
    PDobotV3CmdInterPARM pParm = (PDobotV3CmdInterPARM)pFuncStruct->pFuncParmPtr;
    PDobotV3CmdInter_PcaketPARM pPkParm = &pParm->pPacketParmList[index];
    /* 判断发送是否完成 */
    if(index == pParm->packetListSize) {
        index = 0;
        pPacketParm->exitCallPack = true;
        return;
    }
    index++;
    /* 判断发送标记 */
    if(pPkParm->sendMark == false) {
        return;
    }
    /* 判断发送超时 */
    if(gProtocolFrameAPI.IsTimeOut(pPkParm->sendParm.timeOut, pPkParm->timeStart) == false) {
        return;
    }
    /* 判断重发次数 */
    if(pPkParm->sendParm.repeatNum == 0) {
        /* 判断是否需要指令回调 */
        if(pPkParm->sendParm.cbMark == true){
            /* 判断指令回调是否执行成功，如果执行了指令回调，则自动释放指令空间 */
            DobotV3CmdInterCBParm ErrorCB = {
                .length = pPkParm->length,
                .error = DobotV3CmdInter_TIMEOUT,
            };
            memcpy(&ErrorCB.packet, GETPKGADDR(pParm, index - 1), pPkParm->length);
            CmdCallBackHandle(pParm, pPkParm, ErrorCB);
        }
        /* 判断是否需要自动释放指令空间 */
        if(pPkParm->autoFreeMark == true) {
            ClearPkgParm(pPkParm);
        } else {
            pPkParm->sendMark = false;
            pPkParm->sendStatus = DobotV3CmdInter_TIMEOUT;
        }
        return;
    }
    pPkParm->sendParm.repeatNum--;
    pPkParm->timeStart = gProtocolFrameAPI.GetStartTime();
    PACKKETPARM_FILL(pPacketParm,
                     pPkParm->pLink,
                     GETPKGADDR(pParm, index - 1),
                     pPkParm->length);
    /* 判断指令是否需要应答，指令不需要应答的时候直接清空发送参数 */
    if(pPkParm->sendParm.ackMark == false){
//        if(pPkParm->sendParm.cbMark == true){
//            /* 判断指令回调是否执行成功，如果执行了指令回调，则自动释放指令空间 */
//            DobotV3CmdInterCBParm ErrorCB = {
//                .length = pPkParm->length,
//                .error = DobotV3CmdInter_NOACK,
//            };
//            memcpy(&ErrorCB.packet, GETPKGADDR(pParm, index - 1), pPkParm->length);
//            CmdCallBackHandle(pParm, pPkParm, ErrorCB);
//        }
        ClearPkgParm(pPkParm);
    }
}

/* 定义API接口体 */
DobotV3CmdInterAPI gDobotV3CmdInterAPI = {
    .GetCmdSpace = GetCmdSpace,
    .SendData = SendData,
    .GetSendAck = GetSendAck,
    .FreeCmdSpace = FreeCmdSpace,
    .SetSendParm = SetSendParm,
    .SetDefultSendParm = SetDefultSendParm,
    .RegisterCB = RegisterCB,
    .IsHandleEffect = IsHandleEffect,
    .StopSend = StopSend,
};



