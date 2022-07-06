/********************** Copyright(c)***************************
** Shenzhen Yuejiang Technology Co., Ltd.
**
** http:// www.dobot.cc
**
**------File Info----------------------------------------------
** File name:               DobotV3Format.h
** Latest modified date:    2020-04-20
** Latest version:          V1.0.0
** Description:
**
**------------------------------------------------------------
** Created by:              Chen JunTong
** Created date:            2020-04-20
** Version:                 V1.0.0
** Descriptions:            Dobot V3 协议解析
**
**------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*************************************************************/
#ifndef DobotV3Format_h
#define DobotV3Format_h
#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "ProtocolFrame.h"

#define DobotV3Format_VER_10        0X10             /* 版本号 */
#define DobotV3Format_VER_11        0X11             /* 版本号 */

#define DobotV3Format_SRC           0X00            /* 源地址/发送者 */

#define DobotV3Format_PAYLOAD_LENGTH    512
#define DobotV3Format_HEADLEN       15
#define DobotV3Format_PACKET_LENGTH (DobotV3Format_PAYLOAD_LENGTH + DobotV3Format_HEADLEN)
#define DobotV3Format_SOF1          0XAA
#define DobotV3Format_SOF2          0XBB

#define DobotV3Format_COMMON        0X00
#define DobotV3Format_MAGICBOX      0X03
#define DobotV3Format_MOOZ          0X04
#define DobotV3Format_MAGICIANGO    0X05
#define DobotV3Format_K210          0X20

typedef enum {
    DobotV3Dev_PC = 0,
    DobotV3Dev_Magician,
    DobotV3Dev_MagicianLite,
    DobotV3Dev_MagicianBox,
    DobotV3Dev_MagicianGo,
    DobotV3Dev_K210Car = 0x20,
    DobotV3Dev_K210Arm = 0x21,
    DobotV3Dev_BroadCast = 0XFF,
}DobotV3DevVer11List;


#pragma pack(1)

struct SDobotV3Format_Packet {
    uint8_t sof1;
    uint8_t sof2;
    uint16_t length;
    uint8_t version;
    uint8_t needAck: 1;
    uint8_t isAck: 1;
    uint8_t rwType: 1;
    uint8_t queueType: 1;
    uint8_t encType: 2;
    uint8_t errflg: 1;
    uint8_t seqType: 1;
    uint32_t seqNum;
    uint8_t src;
    uint8_t des;
    uint8_t cmdSet;
    uint8_t cmdId;
    uint8_t headCheck;
    uint8_t payloadAndCRC[DobotV3Format_PAYLOAD_LENGTH + 2];
};
typedef struct SDobotV3Format_Packet DobotV3Format_Packet; /* DobotV3Format_Packet 类型 */
typedef DobotV3Format_Packet *PDobotV3Format_Packet;      /* PDobotV3Format_Packet 指针类型 */

#pragma pack()

ProtocolFrame_FORMAT_EXTERN(DobotV3Format)
extern uint16_t DobotV3Format_PacketEncoder(PDobotV3Format_Packet pParm, uint8_t pDataLen);

#ifdef __cplusplus
}
#endif
#endif /* DobotV3Format_h */


