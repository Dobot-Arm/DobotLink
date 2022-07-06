/****************************************Copyright(c)*****************************************************
**                            Shenzhen Yuejiang Technology Co., LTD.
**
**                                 http://www.dobot.cc
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           teach.cpp
** Latest modified Date:
** Latest Version:      V1.0.0
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Lang Xulin
** Created date:        2016-03-01
** Version:             V1.0.0
** Descriptions:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "teach.h"
#include "rtKinematicCalc.h"
#include "playBack.h"
//#include "../MC/MC.h"
#include "alarms.h"

/*********************************************************************************************************
** Function name:       SinglePos
** Descriptions:        正向点动
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void SinglePos()
{
    gSingleMove.singleVel += gSingleMove.singleAcc * periodTime;
    if (gSingleMove.singleVel > gSingleMove.singleVelMax)
        gSingleMove.singleVel = gSingleMove.singleVelMax;
}

/*********************************************************************************************************
** Function name:       SingleNeg
** Descriptions:        反向点动
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void SingleNeg()
{
    if (gSingleMove.singleVel > -gSingleMove.singleVelMax)
    {
        gSingleMove.singleVel -= gSingleMove.singleAcc * periodTime;
    }
    else
    {
        gSingleMove.singleVel = -gSingleMove.singleVelMax;
    }

    finishFlag = true;
}

/*********************************************************************************************************
** Function name:       SingleStop
** Descriptions:        点动停止
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void SingleStop()
{
    if (gSingleMove.lastState == AP_DOWN || gSingleMove.lastState == BP_DOWN || gSingleMove.lastState == CP_DOWN ||
        gSingleMove.lastState == DP_DOWN)
    {
        gSingleMove.singleVel -= gSingleMove.singleAcc * periodTime;
        if (gSingleMove.singleVel < 0)
        {
            gSingleMove.singleVel = 0;
            gSingleMove.lastState = 0;
            finishFlag = true;
        }
    }
    else if (gSingleMove.lastState == AN_DOWN || gSingleMove.lastState == BN_DOWN || gSingleMove.lastState == CN_DOWN ||
             gSingleMove.lastState == DN_DOWN)
    {
        if (gSingleMove.singleVel < 0)
        {
            gSingleMove.singleVel += gSingleMove.singleAcc * periodTime;
        }
        else
        {
            gSingleMove.singleVel = 0;
            gSingleMove.lastState = 0;
            finishFlag = true;
        }
    }
    else
    {
        gSingleMove.singleVel = 0;
        gSingleMove.lastState = 0;
        finishFlag = true;
    }

    switch (gSingleMove.lastState)
    {
        case IDEL:
            gJointMove.speed[0] = 0;
            gJointMove.speed[1] = 0;
            gJointMove.speed[2] = 0;
            gJointMove.speed[3] = 0;
            break;

        case AP_DOWN:    // NOLINT(bugprone-branch-clone)
            gJointMove.speed[0] = gSingleMove.singleVel;
            gJointMove.speed[1] = 0;
            gJointMove.speed[2] = 0;
            gJointMove.speed[3] = 0;
            break;

        case AN_DOWN:
            gJointMove.speed[0] = gSingleMove.singleVel;
            gJointMove.speed[1] = 0;
            gJointMove.speed[2] = 0;
            gJointMove.speed[3] = 0;
            break;

        case BP_DOWN:    // NOLINT(bugprone-branch-clone)
            gJointMove.speed[0] = 0;
            gJointMove.speed[1] = gSingleMove.singleVel;
            gJointMove.speed[2] = 0;
            gJointMove.speed[3] = 0;
            break;

        case BN_DOWN:
            gJointMove.speed[0] = 0;
            gJointMove.speed[1] = gSingleMove.singleVel;
            gJointMove.speed[2] = 0;
            gJointMove.speed[3] = 0;
            break;

        case CP_DOWN:    // NOLINT(bugprone-branch-clone)
            gJointMove.speed[0] = 0;
            gJointMove.speed[1] = 0;
            gJointMove.speed[2] = gSingleMove.singleVel;
            gJointMove.speed[3] = 0;
            break;

        case CN_DOWN:
            gJointMove.speed[0] = 0;
            gJointMove.speed[1] = 0;
            gJointMove.speed[2] = gSingleMove.singleVel;
            gJointMove.speed[3] = 0;
            break;

        case DP_DOWN:    // NOLINT(bugprone-branch-clone)
            gJointMove.speed[0] = 0;
            gJointMove.speed[1] = 0;
            gJointMove.speed[2] = 0;
            gJointMove.speed[3] = gSingleMove.singleVel;
            break;

        case DN_DOWN:
            gJointMove.speed[0] = 0;
            gJointMove.speed[1] = 0;
            gJointMove.speed[2] = 0;
            gJointMove.speed[3] = gSingleMove.singleVel;
            break;

        default:
            break;
    }
}

/*********************************************************************************************************
** Function name:       Single
** Descriptions:        关节点动
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void Single(int state)
{
    switch (state)
    {
        case AP_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.jointParams.velocity[0] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.jointParams.acceleration[0];
            SinglePos();
            gJointMove.speed[0] = gSingleMove.singleVel;
            gJointMove.speed[1] = 0;
            gJointMove.speed[2] = 0;
            gJointMove.speed[3] = 0;
            gSingleMove.lastState = AP_DOWN;
            break;

        case AN_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.jointParams.velocity[0] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.jointParams.acceleration[0];
            SingleNeg();
            gJointMove.speed[0] = gSingleMove.singleVel;
            gJointMove.speed[1] = 0;
            gJointMove.speed[2] = 0;
            gJointMove.speed[3] = 0;
            gSingleMove.lastState = AN_DOWN;
            break;

        case BP_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.jointParams.velocity[1] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.jointParams.acceleration[1];
            SinglePos();
            gJointMove.speed[0] = 0;
            gJointMove.speed[1] = gSingleMove.singleVel;
            gJointMove.speed[2] = 0;
            gJointMove.speed[3] = 0;
            gSingleMove.lastState = BP_DOWN;
            break;

        case BN_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.jointParams.velocity[1] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.jointParams.acceleration[1];
            SingleNeg();
            gJointMove.speed[0] = 0;
            gJointMove.speed[1] = gSingleMove.singleVel;
            gJointMove.speed[2] = 0;
            gJointMove.speed[3] = 0;
            gSingleMove.lastState = BN_DOWN;
            break;

        case CP_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.jointParams.velocity[2] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.jointParams.acceleration[2];
            SinglePos();
            gJointMove.speed[0] = 0;
            gJointMove.speed[1] = 0;
            gJointMove.speed[2] = gSingleMove.singleVel;
            gJointMove.speed[3] = 0;
            gSingleMove.lastState = CP_DOWN;
            break;

        case CN_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.jointParams.velocity[2] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.jointParams.acceleration[2];
            SingleNeg();
            gJointMove.speed[0] = 0;
            gJointMove.speed[1] = 0;
            gJointMove.speed[2] = gSingleMove.singleVel;
            gJointMove.speed[3] = 0;
            gSingleMove.lastState = CN_DOWN;
            break;

        case DP_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.jointParams.velocity[3] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.jointParams.acceleration[3];
            SinglePos();
            gJointMove.speed[0] = 0;
            gJointMove.speed[1] = 0;
            gJointMove.speed[2] = 0;
            gJointMove.speed[3] = gSingleMove.singleVel;
            gSingleMove.lastState = DP_DOWN;
            break;

        case DN_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.jointParams.velocity[3] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.jointParams.acceleration[3];
            SingleNeg();
            gJointMove.speed[0] = 0;
            gJointMove.speed[1] = 0;
            gJointMove.speed[2] = 0;
            gJointMove.speed[3] = gSingleMove.singleVel;
            gSingleMove.lastState = DN_DOWN;
            break;

        case IDEL:
            SingleStop();
            break;

        default:
            break;
    }
}

/*********************************************************************************************************
** Function name:       SingleXYStop
** Descriptions:        坐标轴点动停止
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void SingleXYStop()
{
    if (gSingleMove.lastState == AP_DOWN || gSingleMove.lastState == BP_DOWN || gSingleMove.lastState == CP_DOWN ||
        gSingleMove.lastState == DP_DOWN)
    {
        gSingleMove.singleVel -= gSingleMove.singleAcc * periodTime;
        if (gSingleMove.singleVel < 0)
        {
            gSingleMove.singleVel = 0;
            gSingleMove.lastState = 0;
        }
    }
    else if (gSingleMove.lastState == AN_DOWN || gSingleMove.lastState == BN_DOWN || gSingleMove.lastState == CN_DOWN ||
             gSingleMove.lastState == DN_DOWN)
    {
        gSingleMove.singleVel += gSingleMove.singleAcc * periodTime;
        if (gSingleMove.singleVel > 0)
        {
            gSingleMove.singleVel = 0;
            gSingleMove.lastState = 0;
        }
    }
    else
    {
        gSingleMove.singleVel = 0;
        gSingleMove.lastState = 0;
    }

    switch (gSingleMove.lastState)
    {
        case IDEL:
            gDistPos.x = gSysParams.pose.x;
            gDistPos.y = gSysParams.pose.y;
            gDistPos.z = gSysParams.pose.z;
            gDistPos.r = gSysParams.pose.rHead;
            break;

        case AP_DOWN:    // NOLINT(bugprone-branch-clone)
            gDistPos.x = gSysParams.pose.x + gSingleMove.singleVel * periodTime;
            gDistPos.y = gSysParams.pose.y;
            gDistPos.z = gSysParams.pose.z;
            gDistPos.r = gSysParams.pose.rHead;
            break;

        case AN_DOWN:
            gDistPos.x = gSysParams.pose.x + gSingleMove.singleVel * periodTime;
            gDistPos.y = gSysParams.pose.y;
            gDistPos.z = gSysParams.pose.z;
            gDistPos.r = gSysParams.pose.rHead;
            break;

        case BP_DOWN:    // NOLINT(bugprone-branch-clone)
            gDistPos.x = gSysParams.pose.x;
            gDistPos.y = gSysParams.pose.y + gSingleMove.singleVel * periodTime;
            gDistPos.z = gSysParams.pose.z;
            gDistPos.r = gSysParams.pose.rHead;
            break;

        case BN_DOWN:
            gDistPos.x = gSysParams.pose.x;
            gDistPos.y = gSysParams.pose.y + gSingleMove.singleVel * periodTime;
            gDistPos.z = gSysParams.pose.z;
            gDistPos.r = gSysParams.pose.rHead;
            break;

        case CP_DOWN:    // NOLINT(bugprone-branch-clone)
            gDistPos.x = gSysParams.pose.x;
            gDistPos.y = gSysParams.pose.y;
            gDistPos.z = gSysParams.pose.z + gSingleMove.singleVel * periodTime;
            gDistPos.r = gSysParams.pose.rHead;
            break;

        case CN_DOWN:
            gDistPos.x = gSysParams.pose.x;
            gDistPos.y = gSysParams.pose.y;
            gDistPos.z = gSysParams.pose.z + gSingleMove.singleVel * periodTime;
            gDistPos.r = gSysParams.pose.rHead;
            break;

        case DP_DOWN:    // NOLINT(bugprone-branch-clone)
            gDistPos.x = gSysParams.pose.x;
            gDistPos.y = gSysParams.pose.y;
            gDistPos.z = gSysParams.pose.z;
            gDistPos.r = gSysParams.pose.rHead + gSingleMove.singleVel * periodTime;
            break;

        case DN_DOWN:
            gDistPos.x = gSysParams.pose.x;
            gDistPos.y = gSysParams.pose.y;
            gDistPos.z = gSysParams.pose.z;
            gDistPos.r = gSysParams.pose.rHead + gSingleMove.singleVel * periodTime;
            break;

        default:
            break;
    }
}

/*********************************************************************************************************
** Function name:       SingleXY
** Descriptions:        坐标轴点动
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void SingleXY(int state)
{
    TRobotTheta theta;
    int32_t calInvErr;

    switch (state)
    {
        case AP_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.coordinateParams.velocity[0] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.coordinateParams.acceleration[0];
            SinglePos();
            gDistPos.x = gSysParams.pose.x + gSingleMove.singleVel * periodTime;
            gDistPos.y = gSysParams.pose.y;
            gDistPos.z = gSysParams.pose.z;
            gDistPos.r = gSysParams.pose.rHead;
            gSingleMove.lastState = AP_DOWN;
            break;

        case AN_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.coordinateParams.velocity[0] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.coordinateParams.acceleration[0];
            SingleNeg();
            gDistPos.x = gSysParams.pose.x + gSingleMove.singleVel * periodTime;
            gDistPos.y = gSysParams.pose.y;
            gDistPos.z = gSysParams.pose.z;
            gDistPos.r = gSysParams.pose.rHead;
            gSingleMove.lastState = AN_DOWN;
            break;

        case BP_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.coordinateParams.velocity[1] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.coordinateParams.acceleration[1];
            SinglePos();
            gDistPos.x = gSysParams.pose.x;
            gDistPos.y = gSysParams.pose.y + gSingleMove.singleVel * periodTime;
            gDistPos.z = gSysParams.pose.z;
            gDistPos.r = gSysParams.pose.rHead;
            gSingleMove.lastState = BP_DOWN;
            break;

        case BN_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.coordinateParams.velocity[1] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.coordinateParams.acceleration[1];
            SingleNeg();
            gDistPos.x = gSysParams.pose.x;
            gDistPos.y = gSysParams.pose.y + gSingleMove.singleVel * periodTime;
            gDistPos.z = gSysParams.pose.z;
            gDistPos.r = gSysParams.pose.rHead;
            gSingleMove.lastState = BN_DOWN;
            break;

        case CP_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.coordinateParams.velocity[2] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.coordinateParams.acceleration[2];
            SinglePos();
            gDistPos.x = gSysParams.pose.x;
            gDistPos.y = gSysParams.pose.y;
            gDistPos.z = gSysParams.pose.z + gSingleMove.singleVel * periodTime;
            gDistPos.r = gSysParams.pose.rHead;
            gSingleMove.lastState = CP_DOWN;
            break;

        case CN_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.coordinateParams.velocity[2] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.coordinateParams.acceleration[2];
            SingleNeg();
            gDistPos.x = gSysParams.pose.x;
            gDistPos.y = gSysParams.pose.y;
            gDistPos.z = gSysParams.pose.z + gSingleMove.singleVel * periodTime;
            gDistPos.r = gSysParams.pose.rHead;
            gSingleMove.lastState = CN_DOWN;
            break;

        case DP_DOWN:
            // 20160713 R轴点动速度加速度
            gSingleMove.singleVelMax = gSysParams.jog.params.coordinateParams.velocity[3] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.coordinateParams.acceleration[3];
            SinglePos();
            gDistPos.x = gSysParams.pose.x;
            gDistPos.y = gSysParams.pose.y;
            gDistPos.z = gSysParams.pose.z;
            gDistPos.r = gSysParams.pose.rHead + gSingleMove.singleVel * periodTime;
            gSingleMove.lastState = DP_DOWN;
            break;

        case DN_DOWN:
            gSingleMove.singleVelMax = gSysParams.jog.params.coordinateParams.velocity[3] * singleVelRatio;
            gSingleMove.singleAcc = gSysParams.jog.params.coordinateParams.acceleration[3];
            SingleNeg();
            gDistPos.x = gSysParams.pose.x;
            gDistPos.y = gSysParams.pose.y;
            gDistPos.z = gSysParams.pose.z;
            gDistPos.r = gSysParams.pose.rHead + gSingleMove.singleVel * periodTime;
            gSingleMove.lastState = DN_DOWN;
            break;

        case IDEL:
            SingleXYStop();
            break;

        default:
            break;
    }

    calInvErr = InverseCal(&gDistPos, 0, &theta);

    if (ERR_PLAN_INV_CALC == calInvErr)
    {
        AlarmSysSetBit(ERR_MOVE_INV_CALC, true);
        finishFlag = true;
        ClearSpeed();
        return;
    }

    gJointMove.speed[0] = (theta.theta[0] - gSysParams.pose.jointAngle[0]) / periodTime;
    gJointMove.speed[1] = (theta.theta[1] - gSysParams.pose.jointAngle[1]) / periodTime;
    gJointMove.speed[2] = (theta.theta[2] - gSysParams.pose.jointAngle[2]) / periodTime;

    if (HoldRHeadFlag == 0 || (gSingleMove.lastState == DP_DOWN) || (gSingleMove.lastState == DN_DOWN))
    {
        gJointMove.speed[3] = (theta.theta[3] - gSysParams.pose.jointAngle[3]) / periodTime;
    }
    else
    {
        gJointMove.speed[3] = 0;
    }
}
