/****************************************Copyright(c)*****************************************************
**                            Shenzhen Yuejiang Technology Co., LTD.
**
**                                 http://www.dobot.cc
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           alarms.h
** Latest modified Date:
** Latest Version:      V1.0.0
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Liu Zhufu
** Created date:        2016-07-11
** Version:             V1.0.0
** Descriptions:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "BitMacro.h"

enum {
    ERR_GROUP_MIN,
    ERR_GROUP_COMMON = ERR_GROUP_MIN,
    ERR_GROUP_PLAN,
    ERR_GROUP_MOVE,
    ERR_GROUP_OVERSPEED,
    ERR_GROUP_LIMIT,
    ERR_GROUP_LOSE_STEP,
    ERR_GROUP_OTHER,
    ERR_GROUP_MAX = ERR_GROUP_OTHER
};

// The min and max are multiple of 8 bits!

#define ALARMS_BITS 128

enum {
    // Common error
    ERR_COMMON_MIN = 0x00,
    ERR_COMMON_RESET = ERR_COMMON_MIN,

    ERR_COMMON_MAX = 0x0f,

    // Plan error
    ERR_PLAN_MIN = 0x10,
    ERR_PLAN_INV_SINGULARITY = ERR_PLAN_MIN,
    ERR_PLAN_INV_CALC,
    ERR_PLAN_INV_LIMIT,
    ERR_PLAN_PUSH_DATA_REPEAT,
    ERR_PLAN_ARC_INPUT_PARAM,
    ERR_PLAN_JUMP_PARAM,

    ERR_PLAN_MAX = 0x1f,

    // Move error
    ERR_MOVE_MIN = 0x20,
    ERR_MOVE_INV_SINGULARITY = ERR_MOVE_MIN,
    ERR_MOVE_INV_CALC,
    ERR_MOVE_INV_LIMIT,

    ERR_MOVE_MAX = 0x2f,

    // Over speed error
    ERR_OVERSPEED_MIN = 0x30,
    ERR_OVERSPEED_AXIS1 = ERR_OVERSPEED_MIN,
    ERR_OVERSPEED_AXIS2,
    ERR_OVERSPEED_AXIS3,
    ERR_OVERSPEED_AXIS4,

    ERR_OVERSPEED_MAX = 0x3f,

    // Limit error
    ERR_LIMIT_MIN = 0x40,
    ERR_LIMIT_AXIS1_POS = ERR_LIMIT_MIN,
    ERR_LIMIT_AXIS1_NEG,

    ERR_LIMIT_AXIS2_POS,
    ERR_LIMIT_AXIS2_NEG,

    ERR_LIMIT_AXIS3_POS,
    ERR_LIMIT_AXIS3_NEG,

    ERR_LIMIT_AXIS4_POS,
    ERR_LIMIT_AXIS4_NEG,

    ERR_LIMIT_AXIS23_POS,
    ERR_LIMIT_AXIS23_NEG,

    ERR_LIMIT_MAX = 0x4f,

    // Lose Step error
    ERR_LOSE_STEP_MIN = 0x50,
    ERR_LOSE_STEP_AXIS1 = ERR_LOSE_STEP_MIN,
    ERR_LOSE_STEP_AXIS2,

    ERR_LOSE_STEP_AXIS3,
    ERR_LOSE_STEP_AXIS4,

    ERR_LOSE_STEP_MAX = 0x5f,

    // Other error
    ERR_OTHER_MIN = 0x60,
    ERR_OTHER_AXIS1_DRV_ALARM = ERR_OTHER_MIN,
    ERR_OTHER_AXIS1_OVERFLOW,
    ERR_OTHER_AXIS1_FOLLOW,

    ERR_OTHER_AXIS2_DRV_ALARM,
    ERR_OTHER_AXIS2_OVERFLOW,
    ERR_OTHER_AXIS2_FOLLOW,

    ERR_OTHER_AXIS3_DRV_ALARM,
    ERR_OTHER_AXIS3_OVERFLOW,
    ERR_OTHER_AXIS3_FOLLOW,

    ERR_OTHER_AXIS4_DRV_ALARM,
    ERR_OTHER_AXIS4_OVERFLOW,
    ERR_OTHER_AXIS4_FOLLOW,

    ERR_OTHER_MAX = 0x6f,

    ERR_ERROR_MAX = ALARMS_BITS - 1
};

extern uint8_t gAlarmSysState[];

/*********************************************************************************************************
** Function name:       AlarmSysCalcOfst
** Descriptions:        Calc the offset byte and the offset bit in byte
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
inline void AlarmSysCalcOfst(uint32_t index, uint32_t &offsetByte, uint32_t &offsetInByte)
{
    offsetByte = index / 8;
    offsetInByte = index % 8;
}

/*********************************************************************************************************
** Function name:       AlarmSysSetBit
** Descriptions:        Set the alarm bit to true or false
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
inline void AlarmSysSetBit(uint32_t index, bool isOn)
{
    if (index > ERR_ERROR_MAX) {
        return;
    }

    uint32_t offsetByte, offsetInByte;

    AlarmSysCalcOfst(index, offsetByte, offsetInByte);
    SETBITS(gAlarmSysState[offsetByte], offsetInByte, offsetInByte, isOn ? 1 : 0);
}

/*********************************************************************************************************
** Function name:       AlarmSysGetBit
** Descriptions:        Get the alarm bit
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
inline bool AlarmSysGetBit(uint32_t index)
{
    uint32_t offsetByte, offsetInByte;

    AlarmSysCalcOfst(index, offsetByte, offsetInByte);

    return (gAlarmSysState[offsetByte] & (0x01 << offsetInByte)) != 0;
}

/*********************************************************************************************************
** Function name:       AlarmSysGetGroupRange
** Descriptions:        Get the range of the group range
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
inline void AlarmSysGetGroupRange(uint32_t groupIndex, uint8_t &min, uint8_t &max)
{
    uint8_t groupMin[] = {ERR_COMMON_MIN, ERR_PLAN_MIN, ERR_MOVE_MIN, ERR_OVERSPEED_MIN, ERR_LIMIT_MIN, ERR_OTHER_MIN};
    uint8_t groupMax[] = {ERR_COMMON_MAX, ERR_PLAN_MAX, ERR_MOVE_MAX, ERR_OVERSPEED_MAX, ERR_LIMIT_MAX, ERR_OTHER_MAX};

    min = groupMin[groupIndex] / 8;
    max = groupMax[groupIndex] / 8;
}

/*********************************************************************************************************
** Function name:       AlarmSysCheckGroup
** Descriptions:        Check whether the alarm group has alarms
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
inline bool AlarmSysCheckGroup(uint32_t groupIndex)
{
    if (groupIndex > ERR_GROUP_MAX) {
        return false;
    }

    uint8_t min, max;
    AlarmSysGetGroupRange(groupIndex, min, max);

    bool hasAlarms = false;

    for (uint32_t i = min; i <= max; i++) {
        if (gAlarmSysState[i]) {
            hasAlarms = true;
            break;
        }
    }

    return hasAlarms;
}

/*********************************************************************************************************
** Function name:       AlarmSysCheckAll
** Descriptions:        Check whether there is any alarm
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
inline bool AlarmSysCheckAll(void)
{
    bool hasAlarms = false;

    for (uint32_t i = 1; i < ALARMS_BITS / 8; i++) {
        if (gAlarmSysState[i]) {
            hasAlarms = true;
            break;
        }
    }

    return hasAlarms;
}

/*********************************************************************************************************
** Function name:       AlarmSysClearGroup
** Descriptions:        Clear the group alarm status
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
inline void AlarmSysClearGroup(uint32_t groupIndex)
{
    uint8_t min, max;
    AlarmSysGetGroupRange(groupIndex, min, max);

    for (uint32_t i = min; i <= max; i++) {
        gAlarmSysState[i] = 0x00;
    }
}

/*********************************************************************************************************
** Function name:       AlarmSysClearAll
** Descriptions:        Clear all the system alarms
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
inline void AlarmSysClearAll(void)
{
    memset((void *)&gAlarmSysState[0], 0x00, ALARMS_BITS / 8);
}

/*********************************************************************************************************
** Function name:       AlarmSysGetAll
** Descriptions:        Get all the system alarms status
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
inline void AlarmSysGetAll(uint8_t **alarmsState, uint32_t *len)
{
    *alarmsState = gAlarmSysState;
    *len = ALARMS_BITS / 8;
}

/*********************************************************************************************************
** Function name:       AlarmSysInit
** Descriptions:        Alarm system initialization
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
inline void AlarmSysInit(void)
{
    AlarmSysClearAll();
}
