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

/*************************************************************
    ProtocolFrame
*************************************************************/

ProtocolFrame_FRAME_EXPORT(ProtocolFrame, DobotV3Format_PACKET_LENGTH, 2)

/*************************************************************
    END ProtocolFrame
*************************************************************/

/*************************************************************
    DobotV3 Link
*************************************************************/

ProtocolFrame_LINK_EXPORT(DobotV3Link, 500, DobotV3Format_PACKET_LENGTH, NULL, NULL);

/*************************************************************
    END DobotV3 Link
*************************************************************/

/*************************************************************
    DobotV3 CmdInter
*************************************************************/

/*************************************************************
** Function name:       GeneralRulesSort
** Descriptions:        匹配分类
** Input parameters:    pSendPacket:发送的数据包
**                      length:数据包长度
** Output parameters:   None
** Returned             true,属于，false，不属于
*************************************************************/
static uint8_t GeneralRulesSort(PDobotV3Format_Packet pSendPacket, uint16_t length)
{
    return true;
}
/*************************************************************
** Function name:       GeneralRulesMatch
** Descriptions:        指令匹配
** Input parameters:    pSendPacket:发送的数据包
**                      pReceivePacket：接收的数据包
** Output parameters:   None
** Returned             true匹配成功，false失败
*************************************************************/
static uint8_t GeneralRulesMatch(PDobotV3Format_Packet pSendPacket, PDobotV3Format_Packet pReadPacket)
{
    if(pSendPacket->cmdId == pReadPacket->cmdId) {
        return true;
    }
    printf("%d,%d",pSendPacket->cmdId,pReadPacket->cmdId);
    return false;
}

/*************************************************************
** Function name:       GetId
** Descriptions:        获取指令ID
** Input parameters:    pReadPacket:接收的数据包
**                      length:数据包长度
** Output parameters:   None
** Returned             ID
*************************************************************/
static uint16_t GeneralRulesGetId(PDobotV3Format_Packet pPacket, uint16_t length)
{
    return  pPacket->cmdId;
}

/* 定义通用规则回调函数列表参数 */
DobotV3CmdInter_CMDCB_LS_EXPORT(GeneralRulesCB, GeneralRulesGetId, 255)


DobotV3CmdInter_EXPORT(DobotV3CmdInter, 100, 512, 1000, 3,
                       GeneralRulesSort, GeneralRulesMatch, &GeneralRulesCB)

/*************************************************************
    END DobotV3 CmdInter
*************************************************************/

/*************************************************************
    DobotV3 Function List
*************************************************************/

ProtocolFrame_FUNCLIST_EXPORT(DobotV3FuncList, &DobotV3CmdInter)

/*************************************************************
    END DobotV3 Function List
*************************************************************/

/*************************************************************
    User API
*************************************************************/

/*************************************************************
** Function name:       ProtocolLink_Register
** Descriptions:        注册链路
** Input parameters:    sendData：链路的发送数据接口
**                      readData：链路的读取数据接口
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void DobotV3_RegisterLink(DobotV3_SendData SendData, DobotV3_ReadData ReadData)
{
    DobotV3Link.readData = ReadData;
    DobotV3Link.SendData = SendData;
}

/*************************************************************
** Function name:       DobotV3_RegisterGRCB
** Descriptions:        注册通用功能回调函数
** Input parameters:    id：注册的功能id
**                      CB：注册的回调指针
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
bool DobotV3_RegisterGRCB(uint16_t id, DobotV3CmdInter_CB CB)
{
    return gDobotV3CmdInterAPI.RegisterCB(&GeneralRulesCB, id, CB);
}

/*************************************************************
** Function name:       DobotV3_SendCmd
** Descriptions:        DobotV3指令发送
** Input parameters:    pPacket:需要发送的数据包指针
**                      payloadLen：需要发送的patload字段长度
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
bool DobotV3_SendCmd(PDobotV3Format_Packet pPacket, uint16_t payloadLen)
{
    DobotV3CmdInter_HANDLE handle;
    if(gDobotV3CmdInterAPI.GetCmdSpace(&handle, &DobotV3CmdInter) == DobotV3CmdInter_NOSPACE) {
        return false;
    }
    gDobotV3CmdInterAPI.SendData(&handle, &DobotV3Link, pPacket, payloadLen);
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
    gDobotV3CmdInterAPI.SetSendParm(&DobotV3CmdInter, repeatNum, timeOut);
}

/*************************************************************
** Function name:       DobotV3_Init
** Descriptions:        DobotV3协议初始化
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void DobotV3_Init(void)
{
    gProtocolFrameAPI.ConnectFLFF(&ProtocolFrame, &DobotV3Link, &DobotV3Format, &DobotV3FuncList);
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
    END User API
*************************************************************/
