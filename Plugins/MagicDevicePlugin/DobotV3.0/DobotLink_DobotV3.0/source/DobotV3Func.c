/********************** Copyright(c)***************************
** Shenzhen Yuejiang Technology Co., Ltd.
**
** http:// www.dobot.cc
**
**------File Info----------------------------------------------
** File name:               DobotV3Func.c
** Latest modified date:    2020-06-03
** Latest version:          V1.0.0
** Description:             DobotV3 功能
**
**------------------------------------------------------------
** Created by:              Chen JunTong
** Created date:            2020-06-03
** Version:                 V1.0.0
** Descriptions:            DobotV3 功能
**
**------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*************************************************************/
#include "DobotV3Func.h"
#include <stdio.h>

#define DobotV3Func_LINK_MAXNUM     10

/* 定义协议框架 */
ProtocolFrame_FRAME_EXPORT(ProtocolFrame, DobotV3Format_PACKET_LENGTH, DobotV3Func_LINK_MAXNUM)


/*************************************************************
    DobotV3 Function List
*************************************************************/
#include "MoozCmdRules.h"
#include "GeneralCmdRules.h"
#include "MGoCmdRules.h"
#include "K210ArmCmdRules.h"
#include "K210CarCmdRules.h"
#include "MBoxCmdRules.h"

/* Mooz 的指令规则必须放在最前面 */
DobotV3CmdInter_EXPORT(DobotV3CmdInter, 100, 256, 1000, 3,
                        {Mooz_RulesSort,        Mooz_RulesMatch,        &MoozRulesCB},
                        {General_RulesSort,     General_RulesMatch,     &GeneralRulesCB},
                        {MGo_RulesSort,         MGo_RulesMatch,         &MGoRulesCB},
                        {K210Arm_RulesSort,     K210Arm_RulesMatch,     &K210ArmRulesCB},
                        {K210Car_RulesSort,     K210Car_RulesMatch,     &K210CarRulesCB},
                        {MBox_RulesSort,        MBox_RulesMatch,        &MBoxRulesCB},
                      )

ProtocolFrame_FUNCLIST_EXPORT(DobotV3FuncList, &DobotV3CmdInter)

/*************************************************************
    END DobotV3 Function List
*************************************************************/

/*************************************************************
    DobotV3 Link
*************************************************************/

/*************************************************************
** Function name:       GET_LINK_IS_UNUSE_RETURN
** Descriptions:        获取链路未被使用的并return返回
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
#define GET_LINK_IS_UNUSE_RETURN(link)                                                  \
        if(gProtocolFrameAPI.GetLinkInFrame(&ProtocolFrame,&link,NULL) == false){       \
            return &link;                                                               \
        }

/* 定义链路结构体 */
ProtocolFrame_LINK_EXPORT(DobotV3Link_1, 500, DobotV3Format_PACKET_LENGTH, NULL, NULL);
ProtocolFrame_LINK_EXPORT(DobotV3Link_2, 500, DobotV3Format_PACKET_LENGTH, NULL, NULL);
ProtocolFrame_LINK_EXPORT(DobotV3Link_3, 500, DobotV3Format_PACKET_LENGTH, NULL, NULL);
ProtocolFrame_LINK_EXPORT(DobotV3Link_4, 500, DobotV3Format_PACKET_LENGTH, NULL, NULL);
ProtocolFrame_LINK_EXPORT(DobotV3Link_5, 500, DobotV3Format_PACKET_LENGTH, NULL, NULL);
ProtocolFrame_LINK_EXPORT(DobotV3Link_6, 500, DobotV3Format_PACKET_LENGTH, NULL, NULL);
ProtocolFrame_LINK_EXPORT(DobotV3Link_7, 500, DobotV3Format_PACKET_LENGTH, NULL, NULL);
ProtocolFrame_LINK_EXPORT(DobotV3Link_8, 500, DobotV3Format_PACKET_LENGTH, NULL, NULL);
ProtocolFrame_LINK_EXPORT(DobotV3Link_9, 500, DobotV3Format_PACKET_LENGTH, NULL, NULL);
ProtocolFrame_LINK_EXPORT(DobotV3Link_10, 500, DobotV3Format_PACKET_LENGTH, NULL, NULL);

/*************************************************************
** Function name:       GetUnUseLink
** Descriptions:        获取未被使用的链路
** Input parameters:    None
** Output parameters:   None
** Returned value:      NULL,不存在，其它，链路指针
** Remarks:             None
*************************************************************/
static PProtocolFrame_LinkPARM GetUnUseLink(void)
{
    GET_LINK_IS_UNUSE_RETURN(DobotV3Link_1);
    GET_LINK_IS_UNUSE_RETURN(DobotV3Link_2);
    GET_LINK_IS_UNUSE_RETURN(DobotV3Link_3);
    GET_LINK_IS_UNUSE_RETURN(DobotV3Link_4);
    GET_LINK_IS_UNUSE_RETURN(DobotV3Link_5);
    GET_LINK_IS_UNUSE_RETURN(DobotV3Link_6);
    GET_LINK_IS_UNUSE_RETURN(DobotV3Link_7);
    GET_LINK_IS_UNUSE_RETURN(DobotV3Link_8);
    GET_LINK_IS_UNUSE_RETURN(DobotV3Link_9);
    GET_LINK_IS_UNUSE_RETURN(DobotV3Link_10);
    return NULL;
}

/*************************************************************
** Function name:       ProtocolLink_Register
** Descriptions:        注册链路
** Input parameters:    sendData：链路的发送数据接口
**                      readData：链路的读取数据接口
** Output parameters:   None
** Returned value:      NULL,失败，其它，链路指针
** Remarks:             None
*************************************************************/
PProtocolFrame_LinkPARM DobotV3_RegisterLink(DobotV3_SendData SendData, DobotV3_ReadData ReadData)
{
    PProtocolFrame_LinkPARM pLink = GetUnUseLink();
    if(pLink == NULL) {
        return NULL;
    }
    pLink->readData = ReadData;
    pLink->SendData = SendData;
    gProtocolFrameAPI.ConnectFLFF(&ProtocolFrame, pLink, &DobotV3Format, &DobotV3FuncList);
    return pLink;
}

/*************************************************************
** Function name:       ProtocolLink_Register
** Descriptions:        注册链路
** Input parameters:    pLink:需要销毁的链路
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void DobotV3_DestoryLink(PProtocolFrame_LinkPARM pLink)
{
    gProtocolFrameAPI.DisConnectFLFF(&ProtocolFrame, pLink, &DobotV3Format, &DobotV3FuncList);
}

/*************************************************************
    END DobotV3 Link
*************************************************************/

/*************************************************************
** Function name:       DobotV3_SendCmd
** Descriptions:        DobotV3指令发送
** Input parameters:    pLink:需要发送的链路
**                      pPacket:需要发送的数据包指针
**                      payloadLen：需要发送的patload字段长度
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
bool DobotV3_SendCmd(PProtocolFrame_LinkPARM pLink, PDobotV3Format_Packet pPacket, uint16_t payloadLen, bool isAck)
{
    DobotV3CmdInter_HANDLE handle;
    if(gDobotV3CmdInterAPI.GetCmdSpace(&handle, &DobotV3CmdInter, true) == DobotV3CmdInter_NOSPACE) {
        return false;
    }
    DobotV3CmdInter_SendParm sendParm;
    sendParm.ackMark = isAck;
    sendParm.cbMark = true;
    sendParm.setParmMark = false;
    gDobotV3CmdInterAPI.SetSendParm(&handle, &sendParm);
    gDobotV3CmdInterAPI.SendData(&handle, pLink, pPacket, payloadLen);
    return true;
}

/*************************************************************
** Function name:       DobotV3_SetSendParm
** Descriptions:        设置DobotV3指令发送参数
** Input parameters:    repeatNum:指令重发次数
**                      timeOut：指令超时时间
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void DobotV3_SetSendParm(uint8_t repeatNum, uint32_t timeOut)
{
    DobotV3CmdInter_SendParm sendParm;
    sendParm.ackMark = true;
    sendParm.cbMark = true;
    sendParm.setParmMark = true;
    sendParm.repeatNum = repeatNum;
    sendParm.timeOut = timeOut;
    gDobotV3CmdInterAPI.SetDefultSendParm(&DobotV3CmdInter, &sendParm);
}

/*************************************************************
** Function name:       DobotV3_TimeCNT
** Descriptions:        时基，时间计数
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void DobotV3_TimeCNT(uint32_t ms)
{
    ProtocolFrame_TIMECNT(ms);
}

/*************************************************************
** Function name:       DobotV3_Exec
** Descriptions:        DobotV3 后台执行函数
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void DobotV3_Exec(void)
{
    gProtocolFrameAPI.Exec(&ProtocolFrame);
}

/*************************************************************
    END User API
*************************************************************/
