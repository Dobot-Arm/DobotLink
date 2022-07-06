/********************** Copyright(c)***************************
** Shenzhen Yuejiang Technology Co., Ltd.
**
** http:// www.dobot.cc
**
**------File Info----------------------------------------------
** File name:               DobotV3Format.c
** Latest modified date:    2020-04-20
** Latest version:          V1.0.0
** Description:             Dobot V3 协议解析
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
#include "DobotV3Format.h"
#include "string.h"

/*************************************************************
    CRC 校验算法
*************************************************************/

/* CRC8计算表 */
uint8_t gDobotV3Format_crc8Table[256];

#define DobotV3Format_CRC8BYTE(CRC, C)  ((CRC = gDobotV3Format_crc8Table[CRC ^ C]) & 0xFF)
/*************************************************************
** Function name:       DobotV3Format_InitCRC8Table
** Descriptions:        初始化CRC8表
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
** Remarks:             None
*************************************************************/
void  DobotV3Format_InitCRC8Table(uint8_t *pTable)
{
    int i;
    int j;
    uint8_t crc;
    for(i = 0; i < 256; i++) {
        crc = i;
        for(j = 0; j < 8; j++) {
            crc = (crc << 1) ^ ((crc & 0x80) ? 0x07 : 0);
        }
        pTable[i] = crc & 0xFF;
    }
}

/*************************************************************
** Function name:       DobotV3Format_CRC8
** Descriptions:        CRC8校验算法
** Input parameters:    buff：需要校验的数据指针
**                      len：需要校验的长度
** Output parameters:   None
** Returned value:      校验结果
** Remarks:             None
*************************************************************/
uint8_t DobotV3Format_CRC8(uint8_t const *buff, uint8_t len)
{
    uint8_t i = 0;
    uint8_t crc = 0;
    static bool firstMark = false;
    if(firstMark == false) {
        DobotV3Format_InitCRC8Table(gDobotV3Format_crc8Table);
        firstMark = true;
    }
    for(i = 0; i < len; i++) {
        DobotV3Format_CRC8BYTE(crc, buff[i]);
    }
    return crc;
}

/* crc16计算表 */
const uint16_t gDobotV3Format_crc_ibm_table[256] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
    0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
    0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
    0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
    0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
    0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
    0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
    0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
    0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
    0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
    0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
    0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
    0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
};

/*************************************************************
** Function name:       DobotV3Format_CRC16
** Descriptions:        CRC16 校验算法
** Input parameters:    buffer：需要校验的buff指针
**                      len：需要校验的数据长度
** Output parameters:   None
** Returned value:      校验结果
** Remarks:             None
*************************************************************/
uint16_t DobotV3Format_CRC16(uint8_t const *buffer, uint16_t len)
{
    uint16_t crc = 0x0000;
    uint8_t lut = 0;
    while(len--) {
        lut = (crc ^ *buffer) & 0xFF;
        crc = (crc >> 8) ^ gDobotV3Format_crc_ibm_table[lut];
        buffer++;
    }
    return crc;
}

/*************************************************************
    END CRC 校验算法
*************************************************************/

/*************************************************************
    Dobot 错误代码
*************************************************************/

/*************************************************************
** Function name:       ErrCodeRead
** Descriptions:        读取指令的错误代码
** Input parameters:    pPacketParm：数据包参数指针
** Output parameters:   no
** min：                no
** Returned             no
*************************************************************/
void DobotV3Format_ErrCodeRead(PProtocolFrame_PacketPARM pPacketParm)
{
    PDobotV3Format_Packet pDobotV3 = (PDobotV3Format_Packet)pPacketParm->pPacket;
    if(pDobotV3->version == DobotV3Format_VER_10) {
        pPacketParm->errCode = 0;
        return;
    }
    if(pDobotV3->errflg == true) {
        memcpy(&pPacketParm->errCode, pDobotV3->payloadAndCRC + 1, 2);
    }
}
/*************************************************************
** Function name:       ErrCodeSend
** Descriptions:        发送指令的错误代码
** Input parameters:    pPacketParm：数据包参数指针
** Output parameters:   no
** min：                no
** Returned             no
*************************************************************/
void DobotV3Format_ErrCodeSend(PProtocolFrame_PacketPARM pPacketParm)
{
    PDobotV3Format_Packet pDobotV3 = (PDobotV3Format_Packet)pPacketParm->pPacket;
    if(pPacketParm->errCode == 0) {
        return;
    }
    memcpy(pDobotV3->payloadAndCRC + 1, &pPacketParm->errCode, 2);
    pDobotV3->payloadAndCRC[0] = pDobotV3->cmdSet;
    pDobotV3->errflg = true;
    pPacketParm->packetLength = DobotV3Format_PacketEncoder(pDobotV3, 3);
    pPacketParm->sendMark = true;
}

/*************************************************************
    END Dobot 错误代码
*************************************************************/


/*************************************************************
    Encode 数据编码
*************************************************************/

/*************************************************************
** Function name:       DobotV3Format_PacketEncoder
** Descriptions:        数据包编码
** Input parameters:    pParm：数据包参数指针
**                      pDataLen：payload段长度
** Output parameters:   no
** min：                no
** Returned             数据包总长度
*************************************************************/
uint16_t DobotV3Format_PacketEncoder(PDobotV3Format_Packet pParm, uint8_t pDataLen)
{
    pParm->sof1 = DobotV3Format_SOF1;
    pParm->sof2 = DobotV3Format_SOF2;
    pParm->length = pDataLen;
    pParm->headCheck = DobotV3Format_CRC8((uint8_t *)pParm, DobotV3Format_HEADLEN - 1);
    uint16_t crc = DobotV3Format_CRC16((uint8_t *)pParm, pDataLen + DobotV3Format_HEADLEN);
    memcpy(pParm->payloadAndCRC + pDataLen, &crc, 2);
    return pDataLen + DobotV3Format_HEADLEN + 2;
}

/*************************************************************
    END Encode 数据编码
*************************************************************/

/*************************************************************
    Decode 数据解码
*************************************************************/

/*************************************************************
** Function name:       DobotV3Format_Sof2Check
** Descriptions:        前导符2解析
** Input parameters:    pData：当前接收到的所有数据
**                      length:当前数据包长度
** Output parameters:   no
** min：                no
** Returned             返回值：ProtocolFrame_DEFALSE
**                              ProtocolFrame_DETRUE
**                              ProtocolFrame_DENEXT
*************************************************************/
uint8_t DobotV3Format_Sof2(uint8_t *pData, uint16_t length)
{
    PDobotV3Format_Packet pPacket = (PDobotV3Format_Packet)pData;
    if(pPacket->sof2 == DobotV3Format_SOF2) {
        return ProtocolFrame_DETRUE;
    }
    return ProtocolFrame_DEFALSE;
}

/*************************************************************
** Function name:       DobotV3Format_Sof2Len
** Descriptions:        前导符2步骤开始校验的长度
** Input parameters:    pData：当前接收到的所有数据
**                      length:当前数据包长度
** Output parameters:   no
** min：                no
** Returned             该步骤校验的长度
*************************************************************/
uint16_t DobotV3Format_Sof2Len(uint8_t *pData, uint16_t length)
{
    return 2;
}

/*************************************************************
** Function name:       DobotV3Format_Length
** Descriptions:        数据包长度解析
** Input parameters:    pData：当前接收到的所有数据
**                      length:当前数据包长度
** Output parameters:   no
** min：                no
** Returned             返回值：ProtocolFrame_DEFALSE
**                              ProtocolFrame_DETRUE
**                              ProtocolFrame_DENEXT
*************************************************************/
uint8_t DobotV3Format_Length(uint8_t *pData, uint16_t length)
{
    PDobotV3Format_Packet pPacket = (PDobotV3Format_Packet)pData;
    if(pPacket->length <= DobotV3Format_PAYLOAD_LENGTH) {
        return ProtocolFrame_DETRUE;
    }
    return ProtocolFrame_DEFALSE;
}

/*************************************************************
** Function name:       DobotV3Format_LengthLen
** Descriptions:        数据包长度步骤开始校验的长度
** Input parameters:    pData：当前接收到的所有数据
**                      length:当前数据包长度
** Output parameters:   no
** min：                no
** Returned             该步骤校验的长度
*************************************************************/
uint16_t DobotV3Format_LengthLen(uint8_t *pData, uint16_t length)
{
    return 4;
}

/*************************************************************
** Function name:       DobotV3Format_Head
** Descriptions:        数据包包头校验
** Input parameters:    pData：当前接收到的所有数据
**                      length:当前数据包长度
** Output parameters:   no
** min：                no
** Returned             返回值：ProtocolFrame_DEFALSE
**                              ProtocolFrame_DETRUE
**                              ProtocolFrame_DENEXT
*************************************************************/
uint8_t DobotV3Format_Head(uint8_t *pData, uint16_t length)
{
    PDobotV3Format_Packet pPacket = (PDobotV3Format_Packet)pData;
    uint8_t crc =  DobotV3Format_CRC8(pData, DobotV3Format_HEADLEN - 1);
    if(crc == pPacket->headCheck) {
        return ProtocolFrame_DETRUE;
    }
    return ProtocolFrame_DEFALSE;
}

/*************************************************************
** Function name:       DobotV3Format_HeadLen
** Descriptions:        数据包长度步骤开始校验的长度
** Input parameters:    pData：当前接收到的所有数据
**                      length:当前数据包长度
** Output parameters:   no
** min：                no
** Returned             该步骤校验的长度
*************************************************************/
uint16_t DobotV3Format_HeadLen(uint8_t *pData, uint16_t length)
{
    return DobotV3Format_HEADLEN;
}

/*************************************************************
** Function name:       DobotV3Format_PacketCRC
** Descriptions:        数据包CRC校验
** Input parameters:    pData：当前接收到的所有数据
**                      length:当前数据包长度
** Output parameters:   no
** min：                no
** Returned             返回值：ProtocolFrame_DEFALSE
**                              ProtocolFrame_DETRUE
**                              ProtocolFrame_DENEXT
*************************************************************/
uint8_t DobotV3Format_PacketCRC(uint8_t *pData, uint16_t length)
{
    PDobotV3Format_Packet pPacket = (PDobotV3Format_Packet)pData;
    uint16_t packetNoCRCLen = DobotV3Format_HEADLEN + pPacket -> length;
    uint16_t crc =  DobotV3Format_CRC16(pData, packetNoCRCLen);
    if(memcmp(&crc, pPacket->payloadAndCRC + pPacket->length, 2) == 0) {
        return ProtocolFrame_DETRUE;
    }
    return ProtocolFrame_DEFALSE;
}

/*************************************************************
** Function name:       DobotV3Format_PacketCRCLen
** Descriptions:        数据包CRC校验长度
** Input parameters:    pData：当前接收到的所有数据
**                      length:当前数据包长度
** Output parameters:   no
** min：                no
** Returned             该步骤校验的长度
*************************************************************/
uint16_t DobotV3Format_PacketCRCLen(uint8_t *pData, uint16_t length)
{
    PDobotV3Format_Packet pPacket = (PDobotV3Format_Packet)pData;
    return pPacket->length + DobotV3Format_HEADLEN + 2;
}

/*************************************************************
    END Decode 数据解码
*************************************************************/


/* 输出格式信息 */
ProtocolFrame_FORMAT_EXPORT(DobotV3Format,
                            DobotV3Format_ErrCodeRead, DobotV3Format_ErrCodeSend,
                            DobotV3Format_SOF1,
{DobotV3Format_Sof2,         DobotV3Format_Sof2Len},
{DobotV3Format_Length,       DobotV3Format_LengthLen},
{DobotV3Format_Head,         DobotV3Format_HeadLen},
{DobotV3Format_PacketCRC,    DobotV3Format_PacketCRCLen},
                           )



