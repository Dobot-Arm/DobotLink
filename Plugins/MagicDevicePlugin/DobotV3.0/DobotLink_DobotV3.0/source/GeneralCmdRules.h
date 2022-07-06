/********************** Copyright(c)***************************
** Shenzhen Yuejiang Technology Co., Ltd.
**
** http:// www.dobot.cc
**
**------File Info----------------------------------------------
** File name:               GeneralCmdRules.h
** Latest modified date:    2020-08-21
** Latest version:          V1.0.0
** Description:             通用指令集规则
**
**------------------------------------------------------------
** Created by:              Chen JunTong
** Created date:            2020-08-21
** Version:                 V1.0.0
** Descriptions:            通用指令集规则
**
**------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*************************************************************/
#ifndef GeneralCmdRules_h
#define GeneralCmdRules_h
#ifdef __cplusplus
extern "C" {
#endif
#include "DobotV3CmdInter.h"


DobotV3CmdInter_CMDCB_LS_EXTERN(GeneralRulesCB)

extern uint8_t General_RulesSort(PDobotV3Format_Packet pSendPacket, uint16_t length);
extern uint8_t General_RulesMatch(PDobotV3Format_Packet pSendPacket, PDobotV3Format_Packet pReadPacket);
extern bool General_RegisterCB(uint16_t id, DobotV3CmdInter_CB CB);

#ifdef __cplusplus
}
#endif
#endif /* GeneralCmdRules_h */



