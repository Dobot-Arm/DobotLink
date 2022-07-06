/****************************************Copyright(c)*****************************************************
**                            Shenzhen Yuejiang Technology Co., LTD.
**
**                                 http://www.dobot.cc
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           rtKinematicCalc.cpp
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
#include "rtKinematicCalc.h"
#include "variable.h"
#include "alarms.h"

#include <cmath>
#include <log/log.h>

/*********************************************************************************************************
** Function name:       InverseCal
** Descriptions:        逆解算
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
int InverseCal(TCPosition* pPositon, float theta2Ref, TRobotTheta* pTheta)
{
    float x, y, y1, z, r;
    float h, v, b, p;

    float l1, l2;
    float distV, distH, dist;
    float theta, theta1, theta2;

    x = pPositon->x;
    y = pPositon->y;
    z = pPositon->z;
    r = pPositon->r;
    y1 = gDobotMechPara.linkBlockY;
    h = sqrt(x * x + y * y - y1 * y1);
    v = z;
    b = (atan2(y, x) - atan2(y1, h)) / PI * 180;
    p = r - b;

    l1 = gDobotMechPara.joint1ArmLen;
    l2 = gDobotMechPara.joint2ArmLen;
    distV = v - gDobotMechPara.baseV + gDobotMechPara.linkBlockV;
    distH = h - gDobotMechPara.baseH - gDobotMechPara.linkBlockH;
    dist = sqrt(distH * distH + distV * distV);
    theta = atan(distV / distH);

    if (fabs((l1 * l1 + dist * dist - l2 * l2) / (2 * l1 * dist)) > 1)
    {
        DOBOT_ERR("Inverse failed");
        return ERR_PLAN_INV_CALC;
    }

    theta1 = acos((l1 * l1 + dist * dist - l2 * l2) / (2 * l1 * dist)) + theta;
    theta2 = atan2((l1 * sin(theta1) - distV), (distH - l1 * cos(theta1)));

    pTheta->theta[1] = 90 - theta1 / PI * 180;
    pTheta->theta[2] = theta2 / PI * 180;

    pTheta->theta[0] = b;
    pTheta->theta[3] = p;

    if (pTheta->theta[0] < gSoftLimit.jointNeg[0] || pTheta->theta[0] > gSoftLimit.jointPos[0] ||
        pTheta->theta[1] < gSoftLimit.jointNeg[1] || pTheta->theta[1] > gSoftLimit.jointPos[1] ||
        pTheta->theta[2] < gSoftLimit.jointNeg[2] || pTheta->theta[2] > gSoftLimit.jointPos[2] ||
        pTheta->theta[3] < gSoftLimit.jointNeg[3] || pTheta->theta[3] > gSoftLimit.jointPos[3] ||
        (90 + pTheta->theta[1] - pTheta->theta[2] > gSoftLimit.parallelJoint12Pos) ||
        (90 + pTheta->theta[1] - pTheta->theta[2] < gSoftLimit.parallelJoint12Neg))
    {
        DOBOT_ERR("Inverse failed, out of limited");
        return ERR_PLAN_INV_LIMIT;
    }

    return 0;
}

/*********************************************************************************************************
** Function name:       ForwardCal
** Descriptions:        正解算
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void ForwardCal(TRobotTheta* pTheta, TCPosition* pPostion)
{
    float l1, l2;
    float theta1, theta2, b, p;
    float h, v;

    b = pTheta->theta[0];
    p = pTheta->theta[3];

    l1 = gDobotMechPara.joint1ArmLen;
    l2 = gDobotMechPara.joint2ArmLen;
    theta1 = PI / 2 - pTheta->theta[1] / 180 * PI;
    theta2 = pTheta->theta[2] / 180 * PI;
    h = l1 * cos(theta1) + l2 * cos(theta2) + gDobotMechPara.baseH + gDobotMechPara.linkBlockH;
    v = l1 * sin(theta1) - l2 * sin(theta2) + gDobotMechPara.baseV - gDobotMechPara.linkBlockV;

    pPostion->x = h * cos(b / 180 * PI) - gDobotMechPara.linkBlockY * sin(b / 180 * PI);
    pPostion->y = h * sin(b / 180 * PI) + gDobotMechPara.linkBlockY * cos(b / 180 * PI);
    pPostion->z = v;
    pPostion->r = p + b;
}

unsigned char GetJointLimit(TRobotTheta *pTheta)
{
    for (uint32_t i = 0; i < ROBOT_AXIS; i++)
    {
        if (pTheta->theta[i] < gSoftLimit.jointPos[i] || pTheta->theta[i] > gSoftLimit.jointPos[i])
            return ERR_PLAN_INV_LIMIT;
    }

    return 0;
}

