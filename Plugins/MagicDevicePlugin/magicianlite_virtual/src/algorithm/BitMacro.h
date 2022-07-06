/****************************************Copyright(c)*****************************************************
**                            Shenzhen Yuejiang Technology Co., LTD.
**
**                                 http://www.dobot.cc
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           BitMacro.h
** Latest modified Date:
** Latest Version:      V1.0.0
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Liu Zhufu
** Created date:        2016-07-11
** Version:             V1.0.0
** Descriptions:        Bit operating macro
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#pragma once

#include <stdint.h>

#define SETBIT(dest,mask)     (dest |= mask)
#define CLRBIT(dest,mask)     (dest &= ~mask)
#define TGLBIT(dest,mask)     (dest ^= mask)
#define CHKBIT(dest,mask)     (dest & mask)
#define SETBITS(dest,bit1,bit2,num)    dest = ((dest) & (uint32_t)~(((1 << ((bit2)-(bit1)+1)) - 1) << (bit1))) | ((num) << (bit1))
