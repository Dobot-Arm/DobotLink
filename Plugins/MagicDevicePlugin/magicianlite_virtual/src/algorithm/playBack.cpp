/****************************************Copyright(c)*****************************************************
**                            Shenzhen Yuejiang Technology Co., LTD.
**
**                                 http://www.dobot.cc
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           playBack.cpp
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
#include <cmath>
#include "playBack.h"
#include "rtKinematicCalc.h"
#include "alarms.h"

#include <cassert>

/*********************************************************************************************************
** Function name:       ClearSpeed
** Descriptions:        ClearSpeed
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void ClearSpeed()
{
    gJointMove.speed[0] = 0;
    gJointMove.speed[1] = 0;
    gJointMove.speed[2] = 0;
    gJointMove.speed[3] = 0;
    gLineMove.speedLine = 0;
}

/*********************************************************************************************************
** Function name:       Stop
** Descriptions:        Stop
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void Stop()
{
    if (fabs(gJointMove.speed[0]) > ZERO || fabs(gJointMove.speed[1]) > ZERO || fabs(gJointMove.speed[2]) > ZERO ||
        fabs(gJointMove.speed[3]) > ZERO)
    {
        if (!stopPlanFlag)
        {
            float T = 0;
            PTPJointParams* ptpJointParams = &gSysParams.ptp.params.jointParams;

            for (uint32_t i = 0; i < 4; i++)
            {
                float stopAcc = 2 * ptpJointParams->acceleration[i];
                float time = fabs(gJointMove.speed[i] / stopAcc);

                if (i == 0)
                {
                    T = time;
                }

                if (time > T)
                {
                    T = time;
                }
            }

            for (uint32_t i = 0; i < 4; i++)
            {
                JointAxis[i].a = -gJointMove.speed[i] / T;
            }

            stopPlanFlag = true;
            finishFlag = false;
            periodCount = 0;
            AllAxis.T = T;
        }

        if (periodCount < int(AllAxis.T / periodTime))
        {
            for (uint32_t i = 0; i < 4; i++)
            {
                gJointMove.speed[i] += JointAxis[i].a * periodTime;

                if (i == 3)
                {
                    gJointMove.speed[i] = 0;
                }
            }

            periodCount++;
        }
        else
        {
            finishFlag = true;
            ClearSpeed();
            vbg = 0;
            periodCount = 0;
            gRobotPrfMode = ROBOT_MODE_NONE;
        }
    }
    else
    {
        finishFlag = true;
        ClearSpeed();
        vbg = 0;
        periodCount = 0;
        gRobotPrfMode = ROBOT_MODE_NONE;
        stopPlanFlag = true;
    }
}

/*********************************************************************************************************
** Function name:       PlaybackStop
** Descriptions:        PlaybackStop
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void PlaybackStop()
{
    if (gMotionType == MOVL_XYZ || gMotionType == MOVL_ANGLE || gMotionType == MOVL_INC)
    {
        TCPosition position;
        TRobotTheta theta;
        int32_t calInvErr;

        if (fabs(gLineMove.speedLine) > ZERO)
        {
            if (!stopPlanFlag)
            {
                float stopAccL = 2 * gSysParams.ptp.params.coordinateParams.xyzAcceleration;

                if (gLineMove.speedLine > 0)
                {
                    Line.a = -stopAccL;
                }
                else
                {
                    Line.a = stopAccL;
                }

                AllAxis.T = fabs(gLineMove.speedLine / Line.a);
                gOrigPos.r = gSysParams.pose.rHead;

                stopPlanFlag = true;
                periodCount = 0;
            }

            if (periodCount < int(AllAxis.T / periodTime))
            {
                periodCount++;
                gLineMove.speedLine += Line.a * periodTime;
                Pose* pose = &gSysParams.pose;

                position.x = pose->x + gLineMove.speedLine * periodTime * gLineMove.dir[0];
                position.y = pose->y + gLineMove.speedLine * periodTime * gLineMove.dir[1];
                position.z = pose->z + gLineMove.speedLine * periodTime * gLineMove.dir[2];
                position.r = gOrigPos.r;

                calInvErr = InverseCal(&position, 0, &theta);

                if (0 != calInvErr)
                {
                    ClearSpeed();
                    periodCount = 0;
                    gRobotPrfMode = ROBOT_MODE_NONE;
                    return;
                }

                for (uint32_t i = 0; i < 4; i++)
                {
                    gJointMove.speed[i] = (theta.theta[i] - pose->jointAngle[i]) / periodTime;
                }
            }
            else
            {
                ClearSpeed();
                periodCount = 0;
                gRobotPrfMode = ROBOT_MODE_NONE;
            }
        }
        else
        {
            ClearSpeed();
            periodCount = 0;
            gRobotPrfMode = ROBOT_MODE_NONE;
            stopPlanFlag = true;
        }
    }
    else
    {
        if (fabs(gJointMove.speed[0]) > ZERO || fabs(gJointMove.speed[1]) > ZERO || fabs(gJointMove.speed[2]) > ZERO ||
            fabs(gJointMove.speed[3]) > ZERO)
        {
            if (!stopPlanFlag)
            {
                float T = 0;
                PTPJointParams* jointParams = &gSysParams.ptp.params.jointParams;

                for (uint32_t i = 0; i < 4; i++)
                {
                    float stopAcc = 2 * jointParams->acceleration[i];
                    float time = fabs(gJointMove.speed[i] / stopAcc);

                    if (i == 0)
                    {
                        T = time;
                    }

                    if (time > T)
                    {
                        T = time;
                    }
                }

                for (uint32_t i = 0; i < 4; i++)
                {
                    JointAxis[i].a = -gJointMove.speed[i] / T;
                }

                stopPlanFlag = true;
                periodCount = 0;
                AllAxis.T = T;
            }

            if (periodCount < int(AllAxis.T / periodTime))
            {
                for (uint32_t i = 0; i < 4; i++)
                {
                    gJointMove.speed[i] += JointAxis[i].a * periodTime;
                }

                periodCount++;
            }
            else
            {
                ClearSpeed();
                periodCount = 0;
                gRobotPrfMode = ROBOT_MODE_NONE;
            }
        }
        else
        {
            ClearSpeed();
            periodCount = 0;
            gRobotPrfMode = ROBOT_MODE_NONE;
            stopPlanFlag = true;
        }
    }
}

/*********************************************************************************************************
** Function name:       GetSpeedLine
** Descriptions:        GetSpeedLine
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void GetSpeedLine()
{
    float t0, T;
    TCPosition position;
    TRobotTheta theta;
    int32_t calInvErr;

    periodCount++;
    t0 = static_cast<float>(periodCount) * periodTime;
    T = AllAxis.T;

    if (!Line.finishFlag)
    {
        if (t0 >= T)
        {
            gLineMove.speedLine = (Line.s - gLineMove.stepLastL) / periodTime;
            gLineMove.stepLastL = Line.s;
            Line.finishFlag = true;
        }
        else
        {
            if (t0 <= Line.t1)
            {
                gLineMove.speedLine = Line.a * t0;
            }
            else if (t0 <= Line.t1 + Line.t2)
            {
                gLineMove.speedLine = Line.v;
            }
            else
            {
                gLineMove.speedLine = Line.v - Line.a * (t0 - Line.t1 - Line.t2);
            }

            gLineMove.stepLastL += gLineMove.speedLine * periodTime;
        }
    }
    else
    {
        gLineMove.speedLine = 0;
    }

    if (!Gesture.finishFlag)
    {
        if (t0 >= T)
        {
            gLineMove.speedRot = (Gesture.s - gLineMove.stepLastR) / periodTime;
            gLineMove.stepLastR = Gesture.s;
            Gesture.finishFlag = true;
        }
        else
        {
            if (t0 <= Gesture.t1)
            {
                gLineMove.speedRot = Gesture.a * t0;
            }
            else if (t0 <= Gesture.t1 + Gesture.t2)
            {
                gLineMove.speedRot = Gesture.v;
            }
            else
            {
                gLineMove.speedRot = Gesture.v - Gesture.a * (t0 - Gesture.t1 - Gesture.t2);
            }

            gLineMove.stepLastR += gLineMove.speedRot * periodTime;
        }
    }
    else
    {
        gLineMove.speedRot = 0;
    }

    position.x = gOrigPos.x + gLineMove.stepLastL * gLineMove.dir[0];
    position.y = gOrigPos.y + gLineMove.stepLastL * gLineMove.dir[1];
    position.z = gOrigPos.z + gLineMove.stepLastL * gLineMove.dir[2];
    position.r = gOrigPos.r + gLineMove.stepLastR * gLineMove.dirR;

    calInvErr = InverseCal(&position, 0, &theta);

    if (0 != calInvErr)
    {
        if (ERR_PLAN_INV_CALC == calInvErr)
        {
            AlarmSysSetBit(ERR_MOVE_INV_CALC, true);
        }
        else if (ERR_PLAN_INV_LIMIT == calInvErr)
        {
            AlarmSysSetBit(ERR_MOVE_INV_LIMIT, true);
        }

        finishFlag = true;
        planFlag = false;
        ClearSpeed();
        return;
    }

    gJointMove.speed[0] = (theta.theta[0] - gSysParams.pose.jointAngle[0]) / periodTime;
    gJointMove.speed[1] = (theta.theta[1] - gSysParams.pose.jointAngle[1]) / periodTime;
    gJointMove.speed[2] = (theta.theta[2] - gSysParams.pose.jointAngle[2]) / periodTime;
    gJointMove.speed[3] = (theta.theta[3] - gSysParams.pose.jointAngle[3]) / periodTime;
}

/*********************************************************************************************************
** Function name:       GetSpeedJoint
** Descriptions:        GetSpeedJoint
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void GetSpeedJoint()
{
    float t0, T1, T2, T;

    periodCount++;
    t0 = (float)periodCount * periodTime;

    T = AllAxis.T;

    for (int i = 0; i < ROBOT_AXIS; i++)
    {
        T1 = JointAxis[i].t1;
        T2 = JointAxis[i].t2;

        if (t0 <= T1)
        {
            gJointMove.speed[i] = JointAxis[i].a * t0;
        }
        else if (t0 <= T1 + T2)
        {
            gJointMove.speed[i] = JointAxis[i].v;
        }
        else
        {
            gJointMove.speed[i] = JointAxis[i].v - JointAxis[i].a * (t0 - T1 - T2);
        }

        gJointMove.speed[i] = gJointMove.speed[i] * (float)JointAxis[i].dir;

        if (!JointAxis[i].finishFlag)
        {
            if (t0 >= T)
            {
                gJointMove.speed[i] = (gJointMove.target[i] - gJointMove.stepLast[i]) / periodTime;
                gJointMove.stepLast[i] = gJointMove.target[i];
                JointAxis[i].finishFlag = true;
            }
            else
            {
                gJointMove.stepLast[i] += gJointMove.speed[i] * periodTime;
            }
        }
        else
        {
            gJointMove.speed[i] = 0;
        }
    }
}

/*********************************************************************************************************
** Function name:       MOVJ
** Descriptions:        MOVJ
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void MOVJ(float x, float y, float z, float r)
{
    if (!planFlag)
    {
        float v, T;

        PTPJointParams* jointParams = &gSysParams.ptp.params.jointParams;

        for (uint32_t i = 0; i < 4; i++)
        {
            JointAxis[i].a = jointParams->acceleration[i] * accRatio;
            JointAxis[i].v = jointParams->velocity[i] * velRatio;
        }

        if (ROBOT_MODE_HOME == gRobotPrfMode)
        {
            float homeAcc[] = { 30, 50, 50, 50 };
            float homeVel[] = { 30, 50, 50, 50 };

            for (uint32_t i = 0; i < 4; i++)
            {
                JointAxis[i].a = homeAcc[i];
                JointAxis[i].v = homeVel[i];
            }
        }

        if (gMotionType == MOVJ_XYZ || gMotionType == JUMP_XYZ || gMotionType == JUMP_ANGLE)
        {
            TCPosition position;
            TRobotTheta theta;
            int32_t calInvErr;

            position.x = x;
            position.y = y;
            position.z = z;
            position.r = r;

            calInvErr = InverseCal(&position, 0, &theta);

            if (0 != calInvErr)
            {
                AlarmSysSetBit(calInvErr, true);
                finishFlag = true;
                planFlag = false;
                return;
            }

            for (uint32_t i = 0; i < 4; i++)
            {
                gJointMove.target[i] = theta.theta[i];
            }
        }
        else if (gMotionType == MOVJ_ANGLE)
        {
            gJointMove.target[0] = x;
            gJointMove.target[1] = y;
            gJointMove.target[2] = z;
            gJointMove.target[3] = r;
        }
        else if (gMotionType == MOVJ_XYZ_INC)
        {
            TCPosition position;
            TRobotTheta theta;
            int32_t calInvErr;
            position.x = gSysParams.pose.x + x;
            position.y = gSysParams.pose.y + y;
            position.z = gSysParams.pose.z + z;
            position.r = gSysParams.pose.rHead + r;

            calInvErr = InverseCal(&position, 0, &theta);

            if (0 != calInvErr)
            {
                AlarmSysSetBit(calInvErr, true);
                finishFlag = true;
                planFlag = false;
                return;
            }

            for (uint32_t i = 0; i < 4; i++)
            {
                gJointMove.target[i] = theta.theta[i];
            }
        }
        else
        {
            float coordinate[] = { x, y, z, r };

            for (uint32_t i = 0; i < 4; i++)
            {
                gJointMove.target[i] = gSysParams.pose.jointAngle[i] + coordinate[i];
            }
        }

        for (uint32_t i = 0; i < 4; i++)
        {
            gJointMove.stepLast[i] = gSysParams.pose.jointAngle[i];
        }

        for (int i = 0; i < ROBOT_AXIS; i++)
        {
            JointAxis[i].s = gJointMove.target[i] - gJointMove.stepLast[i];
            JointAxis[i].dir = 1;

            if (JointAxis[i].s < 0)
            {
                JointAxis[i].s = -JointAxis[i].s;
                JointAxis[i].dir = -1;
            }

            JointAxis[i].finishFlag = JointAxis[i].s < ZERO;
            x = JointAxis[i].v * JointAxis[i].v / JointAxis[i].a;

            if (x < JointAxis[i].s)
            {
                JointAxis[i].t1 = JointAxis[i].v / JointAxis[i].a;
                JointAxis[i].t2 = (JointAxis[i].s - x) / JointAxis[i].v;
                JointAxis[i].T = 2 * JointAxis[i].t1 + JointAxis[i].t2;
            }
            else
            {
                v = sqrt(JointAxis[i].a * JointAxis[i].s);
                JointAxis[i].t1 = v / JointAxis[i].a;
                JointAxis[i].t2 = 0;
                JointAxis[i].T = 2 * JointAxis[i].t1;
            }

            if (i == 0)
            {
                T = JointAxis[0].T;
            }
            else
            {
                if (JointAxis[i].T > T)
                {
                    T = JointAxis[i].T;
                }
            }
        }

        for (auto & JointAxi : JointAxis)
        {
            if (2 * JointAxi.s / T <= JointAxi.v)
            {
                JointAxi.t1 = T / 2;
                JointAxi.t2 = 0;
                JointAxi.a = JointAxi.s / JointAxi.t1 / JointAxi.t1;
            }
            else
            {
                JointAxi.t1 = T - JointAxi.s / JointAxi.v;
                JointAxi.t2 = T - 2 * JointAxi.t1;
                JointAxi.a = JointAxi.v / JointAxi.t1;
            }

            JointAxi.v = JointAxi.a * JointAxi.t1;
        }

        AllAxis.T = T;

        planFlag = true;
        periodCount = 0;
    }

    if (!(JointAxis[0].finishFlag && JointAxis[1].finishFlag && JointAxis[2].finishFlag && JointAxis[3].finishFlag))
    {
        GetSpeedJoint();
    }
    else
    {
        finishFlag = true;
        planFlag = false;
        periodCount = 0;
        ClearSpeed();
    }
}

float GetMOVJTime(float x, float y, float z, float r)
{
    float T = -1;
    float v;
    PTPJointParams* jointParams = &gSysParams.ptp.params.jointParams;

    for (uint32_t i = 0; i < 4; i++)
    {
        JointAxis[i].a = jointParams->acceleration[i] * accRatio;
        JointAxis[i].v = jointParams->velocity[i] * velRatio;
    }

    if (ROBOT_MODE_HOME == gRobotPrfMode)
    {
        float homeAcc[] = { 30, 50, 50, 50 };
        float homeVel[] = { 30, 50, 50, 50 };

        for (uint32_t i = 0; i < 4; i++)
        {
            JointAxis[i].a = homeAcc[i];
            JointAxis[i].v = homeVel[i];
        }
    }

    if (gMotionType == MOVJ_XYZ || gMotionType == JUMP_XYZ || gMotionType == JUMP_ANGLE)
    {
        TCPosition position;
        TRobotTheta theta;
        int32_t calInvErr;

        position.x = x;
        position.y = y;
        position.z = z;
        position.r = r;

        calInvErr = InverseCal(&position, 0, &theta);

        if (0 != calInvErr)
        {
            return -1;
        }

        for (uint32_t i = 0; i < 4; i++)
        {
            gJointMove.target[i] = theta.theta[i];
        }
    }
    else if (gMotionType == MOVJ_ANGLE)
    {
        gJointMove.target[0] = x;
        gJointMove.target[1] = y;
        gJointMove.target[2] = z;
        gJointMove.target[3] = r;
    }
    else if (gMotionType == MOVJ_XYZ_INC)
    {
        TCPosition position;
        TRobotTheta theta;
        int32_t calInvErr;
        position.x = gSysParams.pose.x + x;
        position.y = gSysParams.pose.y + y;
        position.z = gSysParams.pose.z + z;
        position.r = gSysParams.pose.rHead + r;

        calInvErr = InverseCal(&position, 0, &theta);

        if (0 != calInvErr)
        {
            return -1;
        }

        for (uint32_t i = 0; i < 4; i++)
        {
            gJointMove.target[i] = theta.theta[i];
        }
    }
    else
    {
        float coordinate[] = { x, y, z, r };

        for (uint32_t i = 0; i < 4; i++)
        {
            gJointMove.target[i] = gSysParams.pose.jointAngle[i] + coordinate[i];
        }
    }

    for (uint32_t i = 0; i < 4; i++)
    {
        gJointMove.stepLast[i] = gSysParams.pose.jointAngle[i];
    }

    for (int i = 0; i < ROBOT_AXIS; i++)
    {
        JointAxis[i].s = gJointMove.target[i] - gJointMove.stepLast[i];
        JointAxis[i].dir = 1;

        if (JointAxis[i].s < 0)
        {
            JointAxis[i].s = -JointAxis[i].s;
            JointAxis[i].dir = -1;
        }

        JointAxis[i].finishFlag = JointAxis[i].s < ZERO;
        x = JointAxis[i].v * JointAxis[i].v / JointAxis[i].a;

        if (x < JointAxis[i].s)
        {
            JointAxis[i].t1 = JointAxis[i].v / JointAxis[i].a;
            JointAxis[i].t2 = (JointAxis[i].s - x) / JointAxis[i].v;
            JointAxis[i].T = 2 * JointAxis[i].t1 + JointAxis[i].t2;
        }
        else
        {
            v = sqrt(JointAxis[i].a * JointAxis[i].s);
            JointAxis[i].t1 = v / JointAxis[i].a;
            JointAxis[i].t2 = 0;
            JointAxis[i].T = 2 * JointAxis[i].t1;
        }

        if (i == 0)
        {
            T = JointAxis[0].T;
        }
        else
        {
            if (JointAxis[i].T > T)
            {
                T = JointAxis[i].T;
            }
        }
    }
    return T;
}

/*********************************************************************************************************
** Function name:       MOVJ_Time
** Descriptions:        MOVJ_Time
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void MOVJ_Time(float x, float y, float z, float r, float time)
{
    if (!planFlag)
    {
        float T = velRatio;

        PTPJointParams* jointParams = &gSysParams.ptp.params.jointParams;

        for (uint32_t i = 0; i < 4; i++)
        {
            JointAxis[i].a = jointParams->acceleration[i] * accRatio;
            JointAxis[i].v = jointParams->velocity[i] * velRatio;
        }

        if (gMotionType == MOVJ_XYZ || gMotionType == JUMP_XYZ || gMotionType == JUMP_ANGLE)
        {
            TCPosition position;
            TRobotTheta theta;
            int32_t calInvErr;

            position.x = x;
            position.y = y;
            position.z = z;
            position.r = r;

            calInvErr = InverseCal(&position, 0, &theta);

            if (0 != calInvErr)
            {
                AlarmSysSetBit(calInvErr, true);
                finishFlag = true;
                planFlag = false;
                return;
            }

            for (uint32_t i = 0; i < 4; i++)
            {
                gJointMove.target[i] = theta.theta[i];
            }
        }
        else if (gMotionType == MOVJ_ANGLE)
        {
            gJointMove.target[0] = x;
            gJointMove.target[1] = y;
            gJointMove.target[2] = z;
            gJointMove.target[3] = r;
        }
        else
        {
            float coordinate[] = { x, y, z, r };

            for (uint32_t i = 0; i < 4; i++)
            {
                gJointMove.target[i] = gSysParams.pose.jointAngle[i] + coordinate[i];
            }
        }

        for (uint32_t i = 0; i < 4; i++)
        {
            gJointMove.stepLast[i] = gSysParams.pose.jointAngle[i];
        }

        for (int i = 0; i < ROBOT_AXIS; i++)
        {
            JointAxis[i].s = gJointMove.target[i] - gJointMove.stepLast[i];
            JointAxis[i].dir = 1;

            if (JointAxis[i].s < 0)
            {
                JointAxis[i].s = -JointAxis[i].s;
                JointAxis[i].dir = -1;
            }

            JointAxis[i].finishFlag = JointAxis[i].s < ZERO;
        }

        for (auto & JointAxi : JointAxis)
        {
            if (2 * JointAxi.s / T <= JointAxi.v)
            {
                JointAxi.t1 = T / 2;
                JointAxi.t2 = 0;
                JointAxi.a = JointAxi.s / JointAxi.t1 / JointAxi.t1;
            }
            else
            {
                JointAxi.t1 = T - JointAxi.s / JointAxi.v;
                JointAxi.t2 = T - 2 * JointAxi.t1;
                JointAxi.a = JointAxi.v / JointAxi.t1;
            }

            JointAxi.v = JointAxi.a * JointAxi.t1;
        }

        AllAxis.T = T;

        planFlag = true;
        periodCount = 0;
    }

    if (!(JointAxis[0].finishFlag && JointAxis[1].finishFlag && JointAxis[2].finishFlag && JointAxis[3].finishFlag))
    {
        GetSpeedJoint();
    }
    else
    {
        finishFlag = true;
        planFlag = false;
        periodCount = 0;
        ClearSpeed();
    }
}

/*********************************************************************************************************
** Function name:       MOVL
** Descriptions:        MOVL
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void MOVL(float x, float y, float z, float r)
{
    if (!planFlag)
    {
        float s, v, T;

        if (gMotionType == MOVL_XYZ || gMotionType == JUMP_XYZ || gMotionType == JUMP_ANGLE)
        {
            gDistPos.x = x;
            gDistPos.y = y;
            gDistPos.z = z;
            gDistPos.r = r;
        }
        else if (gMotionType == MOVL_ANGLE)
        {
            TRobotTheta theta;
            theta.theta[0] = x;
            theta.theta[1] = y;
            theta.theta[2] = z;
            theta.theta[3] = r;
            ForwardCal(&theta, &gDistPos);
        }
        else
        {
            gDistPos.x = gSysParams.pose.x + x;
            gDistPos.y = gSysParams.pose.y + y;
            gDistPos.z = gSysParams.pose.z + z;
            gDistPos.r = gSysParams.pose.rHead + r;
        }

        float xDelta = gDistPos.x - gSysParams.pose.x;
        float yDelta = gDistPos.y - gSysParams.pose.y;
        float zDelta = gDistPos.z - gSysParams.pose.z;

        Line.s = sqrt(xDelta * xDelta + yDelta * yDelta + zDelta * zDelta);
        Gesture.s = gDistPos.r - gSysParams.pose.rHead;

        if (fabs(Line.s) < ZERO && fabs(Gesture.s) < ZERO)
        {
            finishFlag = true;
            planFlag = false;
            return;
        }

        Line.finishFlag = Line.s < ZERO;
        if (!Line.finishFlag)
        {
            gLineMove.dir[0] = xDelta / Line.s;
            gLineMove.dir[1] = yDelta / Line.s;
            gLineMove.dir[2] = zDelta / Line.s;
        }

        gOrigPos.x = gSysParams.pose.x;
        gOrigPos.y = gSysParams.pose.y;
        gOrigPos.z = gSysParams.pose.z;
        gOrigPos.r = gSysParams.pose.rHead;

        Line.v = gSysParams.ptp.params.coordinateParams.xyzVelocity * velRatio;
        Line.a = gSysParams.ptp.params.coordinateParams.xyzAcceleration * accRatio;
        s = Line.v * Line.v / Line.a;

        if (s < Line.s)
        {
            Line.t1 = Line.v / Line.a;
            Line.t2 = (Line.s - s) / Line.v;
            Line.T = 2 * Line.t1 + Line.t2;
        }
        else
        {
            v = sqrt(Line.a * Line.s);
            Line.t1 = v / Line.a;
            Line.t2 = 0;
            Line.T = 2 * Line.t1;
            Line.v = v;
        }

        Gesture.finishFlag = fabs(Gesture.s) < ZERO;
        gLineMove.dirR = 1;

        if (Gesture.s < 0)
        {
            Gesture.s = -Gesture.s;
            gLineMove.dirR = -1;
        }

        Gesture.v = gSysParams.ptp.params.coordinateParams.rVelocity * velRatio;
        Gesture.a = gSysParams.ptp.params.coordinateParams.rAcceleration * accRatio;

        s = Gesture.v * Gesture.v / Gesture.a;

        if (s < Gesture.s)
        {
            Gesture.t1 = Gesture.v / Gesture.a;
            Gesture.t2 = (Gesture.s - s) / Gesture.v;
            Gesture.T = 2 * Gesture.t1 + Gesture.t2;
        }
        else
        {
            v = sqrt(Gesture.a * Gesture.s);
            Gesture.t1 = v / Gesture.a;
            Gesture.t2 = 0;
            Gesture.T = 2 * Gesture.t1;
            Gesture.v = v;
        }

        if (Line.T > Gesture.T)
        {
            T = Line.T;
        }
        else
        {
            T = Gesture.T;
        }

        AllAxis.T = T;
        //        AllAxis.t1 = Line.t1;
        //        AllAxis.t2 = Line.t2;

        if (2 * Line.s / T <= Line.v)
        {
            Line.t1 = T / 2;
            Line.t2 = 0;
            Line.a = Line.s / Line.t1 / Line.t1;
        }
        else
        {
            Line.t1 = T - Line.s / Line.v;
            Line.t2 = T - 2 * Line.t1;
            Line.a = Line.v / Line.t1;
        }

        Line.v = Line.a * Line.t1;

        if (2 * Gesture.s / T <= Gesture.v)
        {
            Gesture.t1 = T / 2;
            Gesture.t2 = 0;
            Gesture.a = Gesture.s / Gesture.t1 / Gesture.t1;
        }
        else
        {
            Gesture.t1 = T - Gesture.s / Gesture.v;
            Gesture.t2 = T - 2 * Gesture.t1;
            Gesture.a = Gesture.v / Gesture.t1;
        }

        Gesture.v = Gesture.a * Gesture.t1;

        // Line.finishFlag = false;
        // Gesture.finishFlag = false;
        planFlag = true;
        gLineMove.stepLastL = 0;
        gLineMove.stepLastR = 0;
        periodCount = 0;
    }

    if (!Line.finishFlag || !Gesture.finishFlag)
    {
        GetSpeedLine();
    }
    else
    {
        finishFlag = true;
        planFlag = false;
        gLineMove.stepLastL = 0;
        gLineMove.stepLastR = 0;
        periodCount = 0;
        ClearSpeed();
        // 2016 0713 JUMP模式下R轴舵机不动问题
        // gJointMove.speed[3] = (gDistPos.r-gSysParams.pose.jointAngle[0] - gSysParams.pose.jointAngle[3])/periodTime;
    }
}

float GetMOVLTime(float x, float y, float z, float r)
{
    float T;
    float s, v;

    if (gMotionType == MOVL_XYZ || gMotionType == JUMP_XYZ || gMotionType == JUMP_ANGLE)
    {
        gDistPos.x = x;
        gDistPos.y = y;
        gDistPos.z = z;
        gDistPos.r = r;
    }
    else if (gMotionType == MOVL_ANGLE)
    {
        TRobotTheta theta;
        theta.theta[0] = x;
        theta.theta[1] = y;
        theta.theta[2] = z;
        theta.theta[3] = r;
        ForwardCal(&theta, &gDistPos);
    }
    else
    {
        gDistPos.x = gSysParams.pose.x + x;
        gDistPos.y = gSysParams.pose.y + y;
        gDistPos.z = gSysParams.pose.z + z;
        gDistPos.r = gSysParams.pose.rHead + r;
    }

    float xDelta = gDistPos.x - gSysParams.pose.x;
    float yDelta = gDistPos.y - gSysParams.pose.y;
    float zDelta = gDistPos.z - gSysParams.pose.z;

    Line.s = sqrt(xDelta * xDelta + yDelta * yDelta + zDelta * zDelta);
    Gesture.s = gDistPos.r - gSysParams.pose.rHead;

    if (fabs(Line.s) < ZERO && fabs(Gesture.s) < ZERO)
    {
        return -1;
    }

    Line.finishFlag = Line.s < ZERO;
    gLineMove.dir[0] = xDelta / Line.s;
    gLineMove.dir[1] = yDelta / Line.s;
    gLineMove.dir[2] = zDelta / Line.s;

    gOrigPos.x = gSysParams.pose.x;
    gOrigPos.y = gSysParams.pose.y;
    gOrigPos.z = gSysParams.pose.z;
    gOrigPos.r = gSysParams.pose.rHead;

    Line.v = gSysParams.ptp.params.coordinateParams.xyzVelocity * velRatio;
    Line.a = gSysParams.ptp.params.coordinateParams.xyzAcceleration * accRatio;
    s = Line.v * Line.v / Line.a;

    if (s < Line.s)
    {
        Line.t1 = Line.v / Line.a;
        Line.t2 = (Line.s - s) / Line.v;
        Line.T = 2 * Line.t1 + Line.t2;
    }
    else
    {
        v = sqrt(Line.a * Line.s);
        Line.t1 = v / Line.a;
        Line.t2 = 0;
        Line.T = 2 * Line.t1;
        Line.v = v;
    }

    Gesture.finishFlag = Gesture.s < ZERO;
    gLineMove.dirR = 1;

    if (Gesture.s < 0)
    {
        Gesture.s = -Gesture.s;
        gLineMove.dirR = -1;
    }

    Gesture.v = gSysParams.ptp.params.coordinateParams.rVelocity * velRatio;
    Gesture.a = gSysParams.ptp.params.coordinateParams.rAcceleration * accRatio;

    s = Gesture.v * Gesture.v / Gesture.a;

    if (s < Gesture.s)
    {
        Gesture.t1 = Gesture.v / Gesture.a;
        Gesture.t2 = (Gesture.s - s) / Gesture.v;
        Gesture.T = 2 * Gesture.t1 + Gesture.t2;
    }
    else
    {
        v = sqrt(Gesture.a * Gesture.s);
        Gesture.t1 = v / Gesture.a;
        Gesture.t2 = 0;
        Gesture.T = 2 * Gesture.t1;
        Gesture.v = v;
    }

    if (Line.T > Gesture.T)
    {
        T = Line.T;
    }
    else
    {
        T = Gesture.T;
    }
    return T;
}

/*********************************************************************************************************
** Function name:       JUMP
** Descriptions:        JUMP
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void JUMP(float x, float y, float z, float r)
{
    if (!jumPlanFlag)
    {
        transFlag = false;
        downFlag = false;

        if (gMotionType == JUMP_XYZ)
        {
            gDistPosJump.x = x;
            gDistPosJump.y = y;
            gDistPosJump.z = z;
            gDistPosJump.r = r;
        }
        else if (gMotionType == JUMP_ANGLE)
        {
            TRobotTheta theta;
            theta.theta[0] = x;
            theta.theta[1] = y;
            theta.theta[2] = z;
            theta.theta[3] = r;
            ForwardCal(&theta, &gDistPosJump);
        }

        gOrigPosJump.x = gSysParams.pose.x;
        gOrigPosJump.y = gSysParams.pose.y;
        gOrigPosJump.z = gSysParams.pose.z;
        gOrigPosJump.r = gSysParams.pose.rHead;

        if (gOrigPosJump.z > gSysParams.ptp.params.jumpParams.maxJumpHeight ||
            gDistPosJump.z > gSysParams.ptp.params.jumpParams.maxJumpHeight)
        {
            if (gOrigPosJump.z > gDistPosJump.z)
            {
                heightMiddle = gOrigPosJump.z;
            }
            else
            {
                heightMiddle = gDistPosJump.z;
            }
        }
        else
        {
            if (gOrigPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight >=
                gDistPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight)
            {
                heightMiddle = gOrigPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight;
            }
            else
            {
                heightMiddle = gDistPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight;
            }

            if ((gOrigPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight >
                 gSysParams.ptp.params.jumpParams.maxJumpHeight) ||
                (gDistPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight >
                 gSysParams.ptp.params.jumpParams.maxJumpHeight))
            {
                heightMiddle = gSysParams.ptp.params.jumpParams.maxJumpHeight;
            }
        }

        TCPosition position;
        TRobotTheta theta;
        int32_t calInvErr;

        calInvErr = InverseCal(&gDistPosJump, 0, &theta);

        if (0 != calInvErr)
        {
            AlarmSysSetBit(calInvErr, true);
            finishFlag = true;
            planFlag = false;
            jumPlanFlag = false;
            return;
        }

        position.x = gOrigPosJump.x;
        position.y = gOrigPosJump.y;
        position.z = heightMiddle;
        position.r = gOrigPosJump.r;

        calInvErr = InverseCal(&position, 0, &theta);

        if (0 != calInvErr)
        {
            AlarmSysSetBit(calInvErr, true);
            finishFlag = true;
            jumPlanFlag = false;
            planFlag = false;
            return;
        }

        position.x = gDistPosJump.x;
        position.y = gDistPosJump.y;
        position.z = heightMiddle;
        position.r = gDistPosJump.r;

        calInvErr = InverseCal(&position, 0, &theta);

        if (0 != calInvErr)
        {
            AlarmSysSetBit(calInvErr, true);
            finishFlag = true;
            planFlag = false;
            jumPlanFlag = false;
            return;
        }

        jumPlanFlag = true;
    }

    if (!finishFlag && !transFlag)
    {
        if (sqrt(gOrigPosJump.x * gOrigPosJump.x + gOrigPosJump.y * gOrigPosJump.y) > gSoftLimit.disMOVJ)
        {
            MOVL(gOrigPosJump.x, gOrigPosJump.y, heightMiddle, gOrigPosJump.r);
        }
        else
        {
            MOVJ(gOrigPosJump.x, gOrigPosJump.y, heightMiddle, gOrigPosJump.r);
        }
    }

    if (finishFlag && !transFlag)
    {
        transFlag = true;
        finishFlag = false;
    }

    if (!finishFlag && transFlag && !downFlag)
    {
        if (JumpLineFlag == 0)
        {
            MOVJ(gDistPosJump.x, gDistPosJump.y, heightMiddle, gDistPosJump.r);
        }
        else
        {
            MOVL(gDistPosJump.x, gDistPosJump.y, heightMiddle, gDistPosJump.r);
        }
    }

    if (finishFlag && transFlag)
    {
        downFlag = true;
        finishFlag = false;
    }

    if (!finishFlag && downFlag)
    {
        if (sqrt(gDistPosJump.x * gDistPosJump.x + gDistPosJump.y * gDistPosJump.y) > gSoftLimit.disMOVJ)
        {
            MOVL(gDistPosJump.x, gDistPosJump.y, gDistPosJump.z, gDistPosJump.r);
        }
        else
        {
            MOVJ(gDistPosJump.x, gDistPosJump.y, gDistPosJump.z, gDistPosJump.r);
        }
    }

    if (finishFlag)
    {
        jumPlanFlag = false;
        transFlag = false;
        downFlag = false;
    }
}

float GetJUMPTime(float x, float y, float z, float r)
{
    float T;
    float upT;
    float downT;
    float transT;

    if (gMotionType == JUMP_XYZ)
    {
        gDistPosJump.x = x;
        gDistPosJump.y = y;
        gDistPosJump.z = z;
        gDistPosJump.r = r;
    }
    else if (gMotionType == JUMP_ANGLE)
    {
        TRobotTheta theta;
        theta.theta[0] = x;
        theta.theta[1] = y;
        theta.theta[2] = z;
        theta.theta[3] = r;
        ForwardCal(&theta, &gDistPosJump);
    }

    gOrigPosJump.x = gSysParams.pose.x;
    gOrigPosJump.y = gSysParams.pose.y;
    gOrigPosJump.z = gSysParams.pose.z;
    gOrigPosJump.r = gSysParams.pose.rHead;

    if (gOrigPosJump.z > gSysParams.ptp.params.jumpParams.maxJumpHeight ||
        gDistPosJump.z > gSysParams.ptp.params.jumpParams.maxJumpHeight)
    {
        if (gOrigPosJump.z > gDistPosJump.z)
        {
            heightMiddle = gOrigPosJump.z;
        }
        else
        {
            heightMiddle = gDistPosJump.z;
        }
    }
    else
    {
        if (gOrigPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight >=
            gDistPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight)
        {
            heightMiddle = gOrigPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight;
        }
        else
        {
            heightMiddle = gDistPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight;
        }

        if ((gOrigPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight >
             gSysParams.ptp.params.jumpParams.maxJumpHeight) ||
            (gDistPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight >
             gSysParams.ptp.params.jumpParams.maxJumpHeight))
        {
            heightMiddle = gSysParams.ptp.params.jumpParams.maxJumpHeight;
        }
    }

    TCPosition position;
    TRobotTheta theta;
    int32_t calInvErr;

    calInvErr = InverseCal(&gDistPosJump, 0, &theta);

    if (0 != calInvErr)
    {
        return -1;
    }

    position.x = gOrigPosJump.x;
    position.y = gOrigPosJump.y;
    position.z = heightMiddle;
    position.r = gOrigPosJump.r;

    calInvErr = InverseCal(&position, 0, &theta);

    if (0 != calInvErr)
    {
        return -1;
    }

    position.x = gDistPosJump.x;
    position.y = gDistPosJump.y;
    position.z = heightMiddle;
    position.r = gDistPosJump.r;

    calInvErr = InverseCal(&position, 0, &theta);

    if (0 != calInvErr)
    {
        return -1;
    }
    if (sqrt(gOrigPosJump.x * gOrigPosJump.x + gOrigPosJump.y * gOrigPosJump.y) > gSoftLimit.disMOVJ)
    {
        upT = GetMOVLTime(gOrigPosJump.x, gOrigPosJump.y, heightMiddle, gOrigPosJump.r);
    }
    else
    {
        upT = GetMOVJTime(gOrigPosJump.x, gOrigPosJump.y, heightMiddle, gOrigPosJump.r);
    }
    if (upT < 0)
    {
        return upT;
    }
    if (JumpLineFlag == 0)
    {
        transT = GetMOVJTime(gDistPosJump.x, gDistPosJump.y, heightMiddle, gDistPosJump.r);
    }
    else
    {
        transT = GetMOVLTime(gDistPosJump.x, gDistPosJump.y, heightMiddle, gDistPosJump.r);
    }
    if (transT < 0)
    {
        return transT;
    }
    if (sqrt(gDistPosJump.x * gDistPosJump.x + gDistPosJump.y * gDistPosJump.y) > gSoftLimit.disMOVJ)
    {
        downT = GetMOVLTime(gDistPosJump.x, gDistPosJump.y, gDistPosJump.z, gDistPosJump.r);
    }
    else
    {
        downT = GetMOVJTime(gDistPosJump.x, gDistPosJump.y, gDistPosJump.z, gDistPosJump.r);
    }
    if (downT < 0)
    {
        return downT;
    }

    T = upT + transT + downT;
    return T;
}

/*********************************************************************************************************
** Function name:       JUMP_Time
** Descriptions:        JUMP_Time
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void JUMP_Time(float x, float y, float z, float r, float time)
{
    if (!jumPlanFlag)
    {
        transFlag = false;
        downFlag = false;

        if (gMotionType == JUMP_XYZ)
        {
            gDistPosJump.x = x;
            gDistPosJump.y = y;
            gDistPosJump.z = z;
            gDistPosJump.r = r;
        }
        else if (gMotionType == JUMP_ANGLE)
        {
            TRobotTheta theta;
            theta.theta[0] = x;
            theta.theta[1] = y;
            theta.theta[2] = z;
            theta.theta[3] = r;
            ForwardCal(&theta, &gDistPosJump);
        }

        gOrigPosJump.x = gSysParams.pose.x;
        gOrigPosJump.y = gSysParams.pose.y;
        gOrigPosJump.z = gSysParams.pose.z;
        gOrigPosJump.r = gSysParams.pose.rHead;

        if (gOrigPosJump.z > gSysParams.ptp.params.jumpParams.maxJumpHeight ||
            gDistPosJump.z > gSysParams.ptp.params.jumpParams.maxJumpHeight)
        {
            AlarmSysSetBit(ERR_PLAN_JUMP_PARAM, true);
            finishFlag = true;
            planFlag = false;
            return;
        }

        if (gOrigPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight >=
            gDistPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight)
        {
            heightMiddle = gOrigPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight;
        }
        else
        {
            heightMiddle = gDistPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight;
        }

        if ((gOrigPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight >
             gSysParams.ptp.params.jumpParams.maxJumpHeight) ||
            (gDistPosJump.z + gSysParams.ptp.params.jumpParams.jumpHeight >
             gSysParams.ptp.params.jumpParams.maxJumpHeight))
        {
            heightMiddle = gSysParams.ptp.params.jumpParams.maxJumpHeight;
        }

        TCPosition position;
        TRobotTheta theta;
        int32_t calInvErr;

        calInvErr = InverseCal(&gDistPosJump, 0, &theta);

        if (0 != calInvErr)
        {
            AlarmSysSetBit(calInvErr, true);
            finishFlag = true;
            planFlag = false;
            return;
        }

        position.x = gOrigPosJump.x;
        position.y = gOrigPosJump.y;
        position.z = heightMiddle;
        position.r = gOrigPosJump.r;

        calInvErr = InverseCal(&position, 0, &theta);

        if (0 != calInvErr)
        {
            AlarmSysSetBit(calInvErr, true);
            finishFlag = true;
            planFlag = false;
            return;
        }

        position.x = gDistPosJump.x;
        position.y = gDistPosJump.y;
        position.z = heightMiddle;
        position.r = gDistPosJump.r;

        calInvErr = InverseCal(&position, 0, &theta);

        if (0 != calInvErr)
        {
            AlarmSysSetBit(calInvErr, true);
            finishFlag = true;
            planFlag = false;
            return;
        }

        jumPlanFlag = true;
    }

    if (!finishFlag && !transFlag)
    {
        if (sqrt(gOrigPosJump.x * gOrigPosJump.x + gOrigPosJump.y * gOrigPosJump.y) > gSoftLimit.disMOVJ)
        {
            MOVL(gOrigPosJump.x, gOrigPosJump.y, heightMiddle, gOrigPosJump.r);
        }
        else
        {
            MOVJ(gOrigPosJump.x, gOrigPosJump.y, heightMiddle, gOrigPosJump.r);
        }
    }

    if (finishFlag && !transFlag)
    {
        transFlag = true;
        finishFlag = false;
    }

    if (!finishFlag && transFlag && !downFlag)
    {
        MOVJ_Time(gDistPosJump.x, gDistPosJump.y, heightMiddle, gDistPosJump.r, time);
    }

    if (finishFlag && transFlag)
    {
        downFlag = true;
        finishFlag = false;
    }

    if (!finishFlag && downFlag)
    {
        if (sqrt(gDistPosJump.x * gDistPosJump.x + gDistPosJump.y * gDistPosJump.y) > gSoftLimit.disMOVJ)
        {
            MOVL(gDistPosJump.x, gDistPosJump.y, gDistPosJump.z, gDistPosJump.r);
        }
        else
        {
            MOVJ(gDistPosJump.x, gDistPosJump.y, gDistPosJump.z, gDistPosJump.r);
        }
    }

    if (finishFlag)
    {
        jumPlanFlag = false;
        transFlag = false;
        downFlag = false;
    }
}

/*********************************************************************************************************
** Function name:       Playback
** Descriptions:        Playback
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void Playback(float x, float y, float z, float r, int type)
{
    if (finishFlag)
    {
        return;
    }

    gMotionType = type;

    if (type == JUMP_XYZ || type == JUMP_ANGLE)
    {
        JUMP(x, y, z, r);
    }
    else if (type == MOVJ_XYZ || type == MOVJ_ANGLE || type == MOVJ_INC || type == MOVJ_XYZ_INC)
    {
        MOVJ(x, y, z, r);
    }
    else if (type == MOVL_XYZ || type == MOVL_ANGLE || type == MOVL_INC)
    {
        MOVL(x, y, z, r);
    }

    if (finishFlag && stopFinishFlag)
    {
        gRobotPrfMode = ROBOT_MODE_NONE;
        stopFinishFlag = false;
    }
}

float GetPlaybackTime(float x, float y, float z, float r, int type)
{
    float time = 0.0f;
    gMotionType = type;

    switch (type)
    {
        case JUMP_XYZ:
        case JUMP_ANGLE:
            time = GetJUMPTime(x, y, z, r);
            break;

        case MOVJ_XYZ:
        case MOVJ_ANGLE:
        case MOVJ_INC:
        case MOVJ_XYZ_INC:
            time = GetMOVJTime(x, y, z, r);
            break;

        case MOVL_XYZ:
        case MOVL_ANGLE:
        case MOVL_INC:
            time = GetMOVLTime(x, y, z, r);
            break;

        default:
            assert(0);
            break;
    }

    return time;
}
