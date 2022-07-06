/********************** Copyright(c)***************************
** Shenzhen Yuejiang Technology Co., Ltd.
**
** http:// www.dobot.cc
**
**------File Info----------------------------------------------
** File name:               K210ArmCmdRules.h
** Latest modified date:    2020-08-21
** Latest version:          V1.0.0
** Description:             K210Arm 指令规则
**
**------------------------------------------------------------
** Created by:              Chen JunTong
** Created date:            2020-08-21
** Version:                 V1.0.0
** Descriptions:            K210Arm 指令规则
**
**------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*************************************************************/
#ifndef K210ArmCmdRules_h
#define K210ArmCmdRules_h
#ifdef __cplusplus
extern "C" {
#endif

#include "DobotV3CmdInter.h"


DobotV3CmdInter_CMDCB_LS_EXTERN(K210ArmRulesCB)

extern uint8_t K210Arm_RulesSort(PDobotV3Format_Packet pSendPacket, uint16_t length);
extern uint8_t K210Arm_RulesMatch(PDobotV3Format_Packet pSendPacket, PDobotV3Format_Packet pReadPacket);
extern bool K210Arm_RegisterCB(uint16_t id, DobotV3CmdInter_CB CB);


#ifdef __cplusplus
}
#endif
#endif /* K210ArmCmdRules_h */






