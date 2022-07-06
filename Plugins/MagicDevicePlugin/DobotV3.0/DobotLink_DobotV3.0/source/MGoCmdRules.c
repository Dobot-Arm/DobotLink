/********************** Copyright(c)***************************
** Shenzhen Yuejiang Technology Co., Ltd.
**
** http:// www.dobot.cc
**
**------File Info----------------------------------------------
** File name:               MGoCmdRules.c
** Latest modified date:    2020-08-21
** Latest version:          V1.0.0
** Description:             Magician Go 指令规则
**
**------------------------------------------------------------
** Created by:              Chen JunTong
** Created date:            2020-08-21
** Version:                 V1.0.0
** Descriptions:            Magician Go 指令规则
**
**------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*************************************************************/
#include "MGoCmdRules.h"
#include "DobotV3Format.h"

/*************************************************************
** Function name:       MGo_RegisterCB
** Descriptions:        注册通用功能回调函数
** Input parameters:    id：注册的功能id
**                      CB：注册的回调指针
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
bool MGo_RegisterCB(uint16_t id, DobotV3CmdInter_CB CB)
{
    return gDobotV3CmdInterAPI.RegisterCB(&MGoRulesCB, id, CB);
}

/*************************************************************
** Function name:       MGo_RulesSort
** Descriptions:        匹配分类
** Input parameters:    pSendPacket:发送的数据包
**                      length:数据包长度
** Output parameters:   None
** Returned             true,属于，false，不属于
*************************************************************/
uint8_t MGo_RulesSort(PDobotV3Format_Packet pSendPacket, uint16_t length)
{
    if(pSendPacket->version != DobotV3Format_VER_11) {
        return false;
    }
    return pSendPacket->cmdSet == DobotV3Format_MAGICIANGO;
}
/*************************************************************
** Function name:       MGo_RulesMatch
** Descriptions:        指令匹配
** Input parameters:    pSendPacket:发送的数据包
**                      pReceivePacket：接收的数据包
** Output parameters:   None
** Returned             true匹配成功，false失败
*************************************************************/
uint8_t MGo_RulesMatch(PDobotV3Format_Packet pSendPacket, PDobotV3Format_Packet pReadPacket)
{
    if(pSendPacket->queueType == pReadPacket->queueType
            && pSendPacket->rwType == pReadPacket->rwType
            && pSendPacket->cmdId == pReadPacket->cmdId
            && pSendPacket->seqNum == pReadPacket->seqNum) {
        return true;
    }
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
static uint16_t GetId(PDobotV3Format_Packet pPacket, uint16_t length)
{
    return  pPacket->cmdId;
}

/* 定义通用规则回调函数列表参数 */
DobotV3CmdInter_CMDCB_LS_EXPORT(MGoRulesCB, GetId, 255)




