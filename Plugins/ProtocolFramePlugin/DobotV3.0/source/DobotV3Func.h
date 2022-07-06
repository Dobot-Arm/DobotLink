/********************** Copyright(c)***************************
** Shenzhen Yuejiang Technology Co., Ltd.
**
** http:// www.dobot.cc
**
**------File Info----------------------------------------------
** File name:               DobotV3Func.h
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
#ifndef DobotV3Func_h
#define DobotV3Func_h
#include "ProtocolFrame.h"
#include "DobotV3CmdInter.h"
#include "DobotV3Format.h"

typedef uint16_t (*DobotV3_ReadData)(uint8_t *, uint16_t);
typedef void (*DobotV3_SendData)(uint8_t *, uint16_t);

extern void DobotV3_SetSendParm(uint8_t repeatNum, uint32_t timeOut);
extern bool DobotV3_SendCmd(PDobotV3Format_Packet pPacket, uint16_t payloadLen);
extern bool DobotV3_RegisterGRCB(uint16_t id, DobotV3CmdInter_CB CB);
extern void DobotV3_RegisterLink(DobotV3_SendData SendData, DobotV3_ReadData ReadData);
extern void DobotV3_Init(void);
extern void DobotV3_Exec(void);
extern void DobotV3_TimeCNT(uint32_t ms);

#endif /* DobotV3Func_h */
