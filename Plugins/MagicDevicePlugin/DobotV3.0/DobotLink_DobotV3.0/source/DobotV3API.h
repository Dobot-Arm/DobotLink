/********************** Copyright(c)***************************
** Shenzhen Yuejiang Technology Co., Ltd.
**
** http:// www.dobot.cc
**
**------File Info----------------------------------------------
** File name:               DobotV3API.h
** Latest modified date:    2020-08-21
** Latest version:          V1.0.0
** Description:             DobotV3 API协议
**
**------------------------------------------------------------
** Created by:              Chen JunTong
** Created date:            2020-08-21
** Version:                 V1.0.0
** Descriptions:            DobotV3 API协议
**
**------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*************************************************************/
#ifndef DobotV3API_h
#define DobotV3API_h
#ifdef __cplusplus
extern "C" {
#endif
#include "ProtocolFrame.h"
#include "DobotV3CmdInter.h"
#include "DobotV3Func.h"

extern void DobotV3_TimeCNT(uint32_t ms);
extern void DobotV3_Exec(void);

extern PProtocolFrame_LinkPARM DobotV3_RegisterLink(DobotV3_SendData SendData, DobotV3_ReadData ReadData);
extern void DobotV3_DestoryLink(PProtocolFrame_LinkPARM pLink);
extern bool DobotV3_SendCmd(PProtocolFrame_LinkPARM pLink, PDobotV3Format_Packet pPacket, uint16_t payloadLen, bool isAck);
extern void DobotV3_SetSendParm(uint8_t repeatNum, uint32_t timeOut);

/* 回调注册函数 */
extern bool Mooz_RegisterCB(uint16_t id, DobotV3CmdInter_CB CB);
extern bool General_RegisterCB(uint16_t id, DobotV3CmdInter_CB CB);
extern bool MGo_RegisterCB(uint16_t id, DobotV3CmdInter_CB CB);
extern bool K210Arm_RegisterCB(uint16_t id, DobotV3CmdInter_CB CB);
extern bool K210Car_RegisterCB(uint16_t id, DobotV3CmdInter_CB CB);
extern bool MBox_RegisterCB(uint16_t id, DobotV3CmdInter_CB CB);

#ifdef __cplusplus
}
#endif
#endif /* DobotV3API_h */





