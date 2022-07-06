/********************** Copyright(c)***************************
** Shenzhen Yuejiang Technology Co., Ltd.
**
** http:// www.dobot.cc
**
**------File Info----------------------------------------------
** File name:               ProtocolFrame.c
** Latest modified date:    2019-12-16
** Latest version:          V2.2.0
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
*************************************************************/
#include "ProtocolFrame.h"
#include <string.h>


uint32_t gProtocolFrame_TimeCNT;                        /* 时间计数 */


/*************************************************************
** Function name:       GetStartTime
** Descriptions:        获取起始时间
** Input parameters:    None
** Output parameters:   None
** Returned value:      返回当前时间
** Remarks:             None
*************************************************************/
static uint32_t GetStartTime(void)
{
    return gProtocolFrame_TimeCNT;
}

/*************************************************************
** Function name:       IsTimeOut
** Descriptions:        判断是否超时
** Input parameters:    timeOut：超时时间
**                      timeStart：起始时间
** Output parameters:   None
** Returned value:      true 超时，false 未超时
** Remarks:             None
*************************************************************/
static bool IsTimeOut(uint32_t timeOut, uint32_t timeStart)
{
    return ((gProtocolFrame_TimeCNT - (timeStart)) > (timeOut) ? true : false);
}

/*************************************************************
** Function name:       GetTimeElapse
** Descriptions:        获取距离timeStart的时间
** Input parameters:    None
** Output parameters:   None
** Returned value:      返回时间间隔
** Remarks:             None
*************************************************************/
static uint32_t GetTimeElapse(uint32_t timeStart)
{
    return (gProtocolFrame_TimeCNT - (timeStart));
}


/*************************************************************
** Function name:       MSGCLEAR
** Descriptions:        消息清除
** Input parameters:    pLink：(PLinkPARM)链路指针
** Output parameters:   None
** Returned value:      None
*************************************************************/
#define MSGCLEAR(pLink)             \
    pLink->msgReadIndex = 0;        \
    pLink->msgWriteIndex = 0

/*************************************************************
** Function name:       MSGWRITE
** Descriptions:        解析链路消息缓存写入
** Input parameters:    pLink：(PLinkPARM)链路指针
**                      msg：(uint8_t)写入消息
** Output parameters:   None
** Returned value:      None
*************************************************************/
#define MSGWRITE(pLink,msg)                             \
    pLink->pDeMsgBuff[pLink->msgWriteIndex] = msg;      \
    pLink->msgWriteIndex++;

/*************************************************************
** Function name:       MSGREAD
** Descriptions:        解析链路消息读取
** Input parameters:    pLink：(PLinkPARM)链路指针
**                      msg：(uint8_t)读取消息
** Output parameters:   None
** Returned value:      None
*************************************************************/
#define MSGREAD(pLink,msg)                                  \
    msg = pLink->pDeMsgBuff[pLink->msgReadIndex];           \
    pLink->msgReadIndex++;                                  \


/*************************************************************
** Function name:       MSGSETREADINDEX
** Descriptions:        设置读取索引指针
** Input parameters:    pLink：解析链路指针
**                      execIndex：读取索引
** Output parameters:   None
** Returned value:      None
*************************************************************/
#define MSGSETREADINDEX(pLink,execIndex)  \
    pLink->msgReadIndex = execIndex;

/*************************************************************
** Function name:       STEPCLEAR
** Descriptions:        清除解析链路的解析步骤
** Input parameters:    pLink：(PLinkPARM)链路指针
** Output parameters:   None
** Returned value:      None
*************************************************************/
#define STEPCLEAR(pLink)            \
    pLink->deStep = 0;              \
    pLink->deLenght = 0;            \
    pLink->sofMark = false

/*************************************************************
** Function name:       JUDGE_DELENGHT_IS_LIMIT
** Descriptions:        判断解析长度是否超过限制
** Input parameters:    pLink:链路指针
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define JUDGE_DELENGHT_IS_LIMIT(pLink)                      \
    if(pLink->deStepLength > pLink->maxMsgNum){             \
        STEPCLEAR(pLink);                                   \
        MSGSETREADINDEX(pLink, 1);                          \
    }

/*************************************************************
** Function name:       LinkMsgToZero
** Descriptions:        将消息移动到0地址位置
** Input parameters:    pLink：解析链路指针
**                      moveIndex：移动的索引起始地址
**                      lenght：移动的长度
** Output parameters:   None
** Returned value:      None
*************************************************************/
static void LinkMsgToZero(PProtocolFrame_LinkPARM pLink, uint32_t moveIndex)
{
    uint32_t cir;
    uint32_t lenght = pLink->msgWriteIndex - moveIndex;
    for(cir = 0; cir < lenght; cir++) {
        pLink->pDeMsgBuff[cir] = pLink->pDeMsgBuff[moveIndex];
        moveIndex++;
    }
    pLink->msgWriteIndex = lenght;
    pLink->msgReadIndex = 0;
}

/*************************************************************
** Function name:       PacketParmFill
** Descriptions:        填充提交给用户的数据
** Input parameters:    pPacketParm：数据包参数指针
**                      pLink:解析链路
**                      pPacket:数据包指针
**                      packetLength：数据包长度
** Output parameters:   no
** Returned             no
*************************************************************/
static void PacketParmFill(PProtocolFramePARM pFrame,
                           PProtocolFrame_LinkPARM pLink,
                           uint8_t *pPacket,
                           uint16_t packetLength)
{
    PProtocolFrame_PacketPARM pPacketParm = pFrame->pPacketParm;
    pPacketParm->pPacket = pFrame->pPacket;
    if(pPacket != NULL) {
        memcpy(pPacketParm->pPacket, pPacket, packetLength);
    }
    pPacketParm->pFrame = pFrame;
    pPacketParm->packetLength = packetLength;
    pPacketParm->sendMark = false;
    pPacketParm->pLink = pLink;
    pPacketParm->exitCallPack = false;
    pPacketParm->errCode = 0;
    pPacketParm->exitLoop = true;
}

/*************************************************************
** Function name:       FuncMarkClear
** Descriptions:        功能标记清除
** Input parameters:    p：参数结构体
**                      pLink：链路参数结构体
** Output parameters:   None
** Returned value:      None
*************************************************************/
static void FuncMarkClear(PProtocolFramePARM pFrame)
{
    uint8_t cir = 0;
    PProtocolFrame_FuncLIST pFuncList = NULL;                      /* 功能列表 */
    for(cir = 0; cir < pFrame->linkNum; cir++) {
        pFuncList = pFrame->pLinkList[cir]->pFuncList;
        memset(pFuncList->pFuncParmList, 0, sizeof(ProtocolFrame_FuncPARM) * pFuncList->funcListSize);
    }
}

/*************************************************************
** Function name:       FuncBeforeDecoderLoop
** Descriptions:        开始解析之前循环执行
** Input parameters:    p：参数结构体
**                      pLink：链路参数结构体
** Output parameters:   None
** Returned value:      None
*************************************************************/
static void FuncLoop(PProtocolFramePARM pFrame)
{
    PProtocolFrame_FuncLIST pFuncList = NULL;
    PProtocolFrame_PacketPARM pPacketParm = pFrame->pPacketParm;
    uint8_t listSize = 0;
    uint8_t cir1 = 0;
    uint8_t cir2 = 0;
    FuncMarkClear(pFrame);
    for(cir1 = 0; cir1 < pFrame->linkMaxNum; cir1++) {
        if(pFrame->pLinkList[cir1] == NULL){
            continue;
        }
        pFuncList = pFrame->pLinkList[cir1]->pFuncList;
        listSize = pFuncList->funcListSize;
        for(cir2 = 0; cir2 < listSize; cir2++) {
            /* 检查功能参数指针是否为空 */
            if(pFuncList->pFuncStructList[cir2] == NULL) {
                continue;
            }
            /* 检查功能是否已经执行过循环 */
            if(pFuncList->pFuncParmList[cir2].loopMark == true) {
                continue;
            }
            if(pFuncList->pFuncStructList[cir2]->Loop == NULL) {
                pFuncList->pFuncParmList[cir2].loopMark = true;
                continue;
            }
            while(1) {
                PacketParmFill(pFrame, pFrame->pLinkList[cir1], NULL, 0);
                pFuncList->pFuncStructList[cir2]->Loop(pFuncList->pFuncStructList[cir2], pPacketParm);
                /* 判断发送链路，发送标记 */
                if(pPacketParm->pLink != NULL && pPacketParm->sendMark == true) {
                    /* 判断是否需要发送错误代码 */
                    if(pPacketParm->errCode != 0) {
                        if(pPacketParm->pLink->pFormat->ErrCodeSend != NULL) {
                            pPacketParm->pLink->pFormat->ErrCodeSend(pPacketParm);
                        }
                    }
                    pPacketParm->pLink->SendData(pPacketParm->pPacket, pPacketParm->packetLength);
                }
                if(pPacketParm->exitCallPack == true) {
                    break;
                }
            }
            if(pPacketParm->exitLoop == true) {
                pFuncList->pFuncParmList[cir2].loopMark = true;
            }
        }
    }
}

/*************************************************************
** Function name:       FuncCallBack
** Descriptions:        数据包处理
** Input parameters:    p：参数结构体
**                      pLink：链路参数结构体
** Output parameters:   None
** Returned value:      None
*************************************************************/
static void FuncCallBack(PProtocolFrame_PacketPARM pPacketParm)
{
    PProtocolFrame_FuncLIST pFuncList = pPacketParm->pLink->pFuncList;
    uint8_t listSize = pFuncList->funcListSize;
    uint8_t cir = 0;
    for(cir = 0; cir < listSize; cir++) {
        if(pFuncList->pFuncStructList[cir]->PacketCallBack == NULL) {
            continue;
        }
        pFuncList->pFuncStructList[cir]->PacketCallBack(pFuncList->pFuncStructList[cir], pPacketParm);
        if(pPacketParm->sendMark == true) {
            if(pPacketParm->errCode != 0) {
                /* 发送错误代码 */
                if(pPacketParm->pLink->pFormat->ErrCodeSend != NULL) {
                    pPacketParm->pLink->pFormat->ErrCodeSend(pPacketParm);
                }
            }
            pPacketParm->pLink->SendData(pPacketParm->pPacket, pPacketParm->packetLength);
        }
        if(pPacketParm->exitCallPack == true) {
            return;
        }
    }
}


/*************************************************************
** Function name:       DecodePacket
** Descriptions:        从数据链路解析数据包
** Input parameters:    pFrame：框架参数结构体指针
**                      pLink：数据链路
** Output parameters:   no
** min：                no
** Returned             no
*************************************************************/
static void DecodeLinkPacket(PProtocolFramePARM pFrame, PProtocolFrame_LinkPARM pLink)
{
    volatile uint8_t rLen;
    uint8_t data = 0;
    uint8_t stepCheckStatus = 0;
    /* 检查链路是否使能 */
    if(pLink->enable == false) {
        STEPCLEAR(pLink);       /* 清空解析步骤 */
        MSGCLEAR(pLink);        /* 清空消息内容 */
        return;
    }
    PProtocolFrame_FormatPARM pDeFormat = pLink->pFormat;
    while(1) {
        /* 将数据读取到链路缓存里面，当读取的数据为 0 时，退出 */
        rLen = pLink->readData(pLink->pDeMsgBuff + pLink->msgWriteIndex, pLink->maxMsgNum - pLink->msgWriteIndex);
        if(rLen != 0){
            pLink->deStartTime = GetStartTime();
        }else{
            /* 判断解析是否超时 */
            if(IsTimeOut(pLink->deTimeOut, pLink->deStartTime) == true
                && pLink->msgWriteIndex != 0) {
                LinkMsgToZero(pLink, 1);
                STEPCLEAR(pLink);       /* 清空解析步骤 */
            }
        }
        pLink->msgWriteIndex += rLen;
        if(pLink->msgReadIndex == pLink->msgWriteIndex) {
            break;
        }
        /* 如果缓存存在未解析的消息则循环解析 */
        while(pLink->msgReadIndex < pLink->msgWriteIndex) {
            MSGREAD(pLink, data);
            /* 判断是否接收到前导符 */
            if(pLink->sofMark == false) {
                if(data == pDeFormat->sof) {
                    LinkMsgToZero(pLink, pLink->msgReadIndex - 1);
                    pLink->deLenght = 0;
                    pLink->sofMark = true;      /* 置位sofMark标记 */
                    pLink->deStepLength = pDeFormat->pStepList[pLink->deStep].GetLength(pLink->pDeMsgBuff, pLink->deLenght);
                    // pLink->deStartTime = GetStartTime();
                    JUDGE_DELENGHT_IS_LIMIT(pLink)
                }
                continue;
            }
            pLink->deLenght++;
            /* 判断是否接收到解析长度的数据 */
            if(pLink->deLenght < pLink->deStepLength) {
                continue;
            }
            /* 步骤校验 */
            stepCheckStatus = pDeFormat->pStepList[pLink->deStep].StepCheck(pLink->pDeMsgBuff, pLink->deLenght);
            /* 判断解析是否错误 */
            if(stepCheckStatus == ProtocolFrame_DEFALSE) {
                STEPCLEAR(pLink);               /* 清空解析步骤 */
                MSGSETREADINDEX(pLink, 1);      /* 重置解析数据读取的索引 */
                continue;
            }
            pLink->deStep++;
            /* 判断是否解析完所有步骤 */
            if(pLink->deStep < pDeFormat->stepListSize) {
                pLink->deStepLength = pDeFormat->pStepList[pLink->deStep].GetLength(pLink->pDeMsgBuff, pLink->deLenght);
                JUDGE_DELENGHT_IS_LIMIT(pLink)
                continue;
            }
            PacketParmFill(pFrame, pLink, pLink->pDeMsgBuff, pLink->deLenght);
            if(pDeFormat->ErrCodeRead != NULL) {
                pDeFormat->ErrCodeRead(pFrame->pPacketParm);
            }
            FuncCallBack(pFrame->pPacketParm);
            STEPCLEAR(pLink);       /* 清空解析步骤 */
            LinkMsgToZero(pLink, pLink->msgReadIndex);
        }
    }
    /* 判断当前是否在解析过程 */
    if(pLink->sofMark == false) {
        MSGCLEAR(pLink);        /* 清空消息内容 */
        return;
    }
}

/*************************************************************
** Function name:       DeLinkDecode
** Descriptions:        链路解析
** Input parameters:    pFrame：框架参数结构体指针
** Output parameters:   None
** Returned value:      None
*************************************************************/
static void DeLinkDecode(PProtocolFramePARM pFrame)
{
    uint8_t cir = 0;
    for(cir = 0; cir < pFrame->linkMaxNum; cir++) {
        if(pFrame->pLinkList[cir] == NULL){
            continue;
        }
        DecodeLinkPacket(pFrame, pFrame->pLinkList[cir]);
    }
}

/*************************************************************
** Function name:       GetLinkInFrame
** Descriptions:        获取链路在列表的位置
** Input parameters:    pFrame：框架参数结构体指针
**                      pLink:查找的链路参数结构体指针
**                      pIndex:接收位置索引的指针
** Output parameters:   no
** min：                no
** Returned             true，链路存在，false，链路不存在
*************************************************************/
static uint8_t GetLinkInFrame(PProtocolFramePARM pFrame, PProtocolFrame_LinkPARM pLink, uint8_t *pIndex)
{
    for(uint8_t index = 0; index < pFrame->linkMaxNum; index++) {
        if(pFrame->pLinkList[index] != pLink) {
            continue;
        }
        if(pIndex != NULL) {
            *pIndex = index;
        }
        return true;
    }
    return false;
}

/*************************************************************
** Function name:       GetNullLinkInFrame
** Descriptions:        获取空链路在列表的位置
** Input parameters:    pFrame：框架参数结构；体指针
**                      pIndex:接收位置索引的指针
** Output parameters:   no
** min：                no
** Returned             true，空链路存在，false，空链路不存在
*************************************************************/
static uint8_t GetNullLinkInFrame(PProtocolFramePARM pFrame, uint8_t *pIndex)
{
    *pIndex = 0;
    for(; (*pIndex) < pFrame->linkMaxNum; (*pIndex)++) {
        if(pFrame->pLinkList[*pIndex] == NULL) {
            return true;
        }
    }
    return false;
}

/*************************************************************
** Function name:       GetLinkIsEnable
** Descriptions:        获取链路是否使能
** Input parameters:    pLink：需要获取的链路
** Output parameters:   None
** Returned value:      true 使能，false 禁能
** Remarks:             None
*************************************************************/
static bool GetLinkIsEnable(PProtocolFrame_LinkPARM pLink)
{
    return pLink->enable;
}

/*************************************************************
** Function name:       SetLinkIsEnable
** Descriptions:        设置链路是否使能
** Input parameters:    pLink：需要设置的链路
**                      status：状态，true使能，false禁能
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
static void SetLinkIsEnable(PProtocolFrame_LinkPARM pLink, bool status)
{
    if(status == pLink->enable) {
        return;
    }
    if(status == true) {
        STEPCLEAR(pLink);
        MSGCLEAR(pLink);
    }
    pLink->enable = status;
}

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
static void SetLinkEnableOnly(PProtocolFramePARM pFrame, PProtocolFrame_LinkPARM pLink)
{
    uint8_t cir = 0;
    for(cir = 0; cir < pFrame->linkNum; cir++) {
        if(pFrame->pLinkList[cir] != pLink) {
            pFrame->pLinkList[cir]->enable = false;
        } else {
            if(pFrame->pLinkList[cir]->enable == false) {
                STEPCLEAR(pLink);
                MSGCLEAR(pLink);
            }
            pFrame->pLinkList[cir]->enable = true;
        }
    }
}

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
static void SetLinkDisableOnly(PProtocolFramePARM pFrame, PProtocolFrame_LinkPARM pLink)
{
    uint8_t cir = 0;
    for(cir = 0; cir < pFrame->linkNum; cir++) {
        if(pFrame->pLinkList[cir] != pLink) {
            if(pFrame->pLinkList[cir]->enable == false) {
                STEPCLEAR(pLink);
                MSGCLEAR(pLink);
            }
            pFrame->pLinkList[cir]->enable = true;
        } else {
            pFrame->pLinkList[cir]->enable = false;
        }
    }
}

/*************************************************************
** Function name:       RegisterLink
** Descriptions:        注册链路
** Input parameters:    pFrame：框架参数结构体指针
**                      pLink:需要注册的链路指针
** Output parameters:   no
** min：                no
** Returned             true，成功
**                      false，失败
*************************************************************/
static uint8_t RegisterLink(PProtocolFramePARM pFrame, PProtocolFrame_LinkPARM pLink)
{
    uint8_t index = 0;
    if(GetLinkInFrame(pFrame, pLink, &index) == false) {
        if(GetNullLinkInFrame(pFrame, &index) == false) {
            return false;
        } else {
            pFrame->linkNum++;
        }
    }
    STEPCLEAR(pLink);
    MSGCLEAR(pLink);
    pFrame->pLinkList[index] = pLink;
    return true;
}

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
static bool ConnectFLFF(PProtocolFramePARM pFrame,
                        PProtocolFrame_LinkPARM pLink,
                        PProtocolFrame_FormatPARM pFormat,
                        PProtocolFrame_FuncLIST pFuncList)
{
    if(RegisterLink(pFrame, pLink) == true) {
        pLink->pFormat = pFormat;
        pLink->pFuncList = pFuncList;
        return true;
    }
    return false;
}

/*************************************************************
** Function name:       DisConnectFLFF
** Descriptions:        断开Frame，link，format，func连接
** Input parameters:    pFrame：框架参数结构体指针
**                      pLink:链路指针
**                      pFormat:解析格式,可以为NULL
**                      pFuncList：功能列表,可以为NULL
** Output parameters:   None
** min：                None
** Returned             true，成功
**                      false，链路不存在或者信息不匹配
** Remarks:             注意如果该动作与Exec不处于同一线程，需要在执行的时候与Exec线程斥
*************************************************************/
static bool DisConnectFLFF(PProtocolFramePARM pFrame,
                    PProtocolFrame_LinkPARM pLink,
                    PProtocolFrame_FormatPARM pFormat,
                    PProtocolFrame_FuncLIST pFuncList)
{
    uint8_t index = 0;
    if(GetLinkInFrame(pFrame, pLink, &index) == false) {
        return false;
    }
    if(pFormat != NULL && pLink->pFormat != pFormat) {
        return false;
    }
    if(pFuncList != NULL && pLink->pFuncList != pFuncList) {
        return false;
    }
    pFrame->pLinkList[index] = NULL;
    pFrame->linkNum--;
    pLink->pFormat = NULL;
    pLink->pFuncList = NULL;
    return true;
}

/*************************************************************
** Function name:       exec
** Descriptions:        协议包框架的执行程序
** Input parameters:    pFrame：框架参数结构体指针
** Output parameters:   no
** min：                no
** Returned             no
*************************************************************/
static void Exec(PProtocolFramePARM pFrame)
{
    DeLinkDecode(pFrame);
    FuncLoop(pFrame);
}


/* 对外提供的API接口列表 */
ProtocolFrameAPI gProtocolFrameAPI = {
    .GetStartTime = GetStartTime,
    .IsTimeOut = IsTimeOut,
    .GetTimeElapse = GetTimeElapse,
    .GetLinkIsEnable = GetLinkIsEnable,
    .SetLinkIsEnable = SetLinkIsEnable,
    .SetLinkEnableOnly = SetLinkEnableOnly,
    .SetLinkDisableOnly = SetLinkDisableOnly,
    .GetLinkInFrame = GetLinkInFrame,
    .ConnectFLFF = ConnectFLFF,
    .DisConnectFLFF = DisConnectFLFF,
    .Exec = Exec,
};

