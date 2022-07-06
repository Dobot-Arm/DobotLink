/********************** Copyright(c)***************************
** Shenzhen Yuejiang Technology Co., Ltd.
**
** http:// www.dobot.cc
**
**------File Info----------------------------------------------
** File name:               MBoxCmdRules.h
** Latest modified date:    2020-08-21
** Latest version:          V1.0.0
** Description:             MBox 指令规则
**
**------------------------------------------------------------
** Created by:              Chen JunTong
** Created date:            2020-08-21
** Version:                 V1.0.0
** Descriptions:            MBox 指令规则
**
**------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*************************************************************/
#ifndef MBoxCmdRules_h
#define MBoxCmdRules_h
#ifdef __cplusplus
extern "C" {
#endif

#include "DobotV3CmdInter.h"


DobotV3CmdInter_CMDCB_LS_EXTERN(MBoxRulesCB)

extern uint8_t MBox_RulesSort(PDobotV3Format_Packet pSendPacket, uint16_t length);
extern uint8_t MBox_RulesMatch(PDobotV3Format_Packet pSendPacket, PDobotV3Format_Packet pReadPacket);
extern bool MBox_RegisterCB(uint16_t id, DobotV3CmdInter_CB CB);


#ifdef __cplusplus
}
#endif
#endif /* MBoxCmdRules_h */






