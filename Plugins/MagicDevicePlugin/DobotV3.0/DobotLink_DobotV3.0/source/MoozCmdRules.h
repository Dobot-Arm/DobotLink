/********************** Copyright(c)***************************
** Shenzhen Yuejiang Technology Co., Ltd.
**
** http:// www.dobot.cc
**
**------File Info----------------------------------------------
** File name:               MoozCmdRules.h
** Latest modified date:    2020-08-21
** Latest version:          V1.0.0
** Description:             模组指令集规则
**
**------------------------------------------------------------
** Created by:              Chen JunTong
** Created date:            2020-08-21
** Version:                 V1.0.0
** Descriptions:            模组指令集规则
**
**------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*************************************************************/
#ifndef MoozCmdRules_h
#define MoozCmdRules_h
#ifdef __cplusplus
extern "C" {
#endif
#include "DobotV3CmdInter.h"

DobotV3CmdInter_CMDCB_LS_EXTERN(MoozRulesCB)

extern uint8_t Mooz_RulesSort(PDobotV3Format_Packet pSendPacket, uint16_t length);
extern uint8_t Mooz_RulesMatch(PDobotV3Format_Packet pSendPacket, PDobotV3Format_Packet pReadPacket);
extern bool Mooz_RegisterCB(uint16_t id, DobotV3CmdInter_CB CB);

#ifdef __cplusplus
}
#endif
#endif /* MoozCmdRules_h */



