#include "playBack.h"
#include "rtKinematicCalc.h"

#include <cmath>
#include <cstring>

#include "alarms.h"

void TP_CalcArcPtForScara(TCPosition beginPos, MATRIX3D planeRotM, VECTOR3D arcCenter, VECTOR3D rotAxis,
                          float arcRadius, float angle_i, float theta_i, TCPosition* nextPos)
{
    float R, cosAnglei, sinAnglei, tmpX, tmpY;
    VECTOR3D Center, tmpPt;
    MATRIX3D Mat;

    R = arcRadius;
    memcpy(&Mat, &planeRotM, sizeof(MATRIX3D));
    memcpy(&Center, &arcCenter, sizeof(VECTOR3D));

    cosAnglei = (float)cos((double)angle_i);
    sinAnglei = (float)sin((double)angle_i);
    tmpX = R * cosAnglei;
    tmpY = R * sinAnglei;
    tmpPt.item01 = Mat.item11 * tmpX + Mat.item12 * tmpY + Center.item01;
    tmpPt.item02 = Mat.item21 * tmpX + Mat.item22 * tmpY + Center.item02;
    tmpPt.item03 = Mat.item31 * tmpX + Mat.item32 * tmpY + Center.item03;

    nextPos->x = tmpPt.item01;
    nextPos->y = tmpPt.item02;
    nextPos->z = tmpPt.item03;
    nextPos->r = beginPos.r + rotAxis.item03 * theta_i;
}

int16_t TP_ArcPlanForScara(TCPosition beginPos, TCPosition midPos, TCPosition endPos, CirlPlanPara* ArcPlanParam)
{
    float Plane_A, Plane_B, Plane_C, Plane_D;
    float a1, b1, c1, d1, a2, b2, c2, d2;
    float det1, det2, det3, det4, tmpR, Angle, tmpAngle, flag1, tmpTheta;

    VECTOR3D p1, p2, p3, tmpCenter, tmpNorm, OA, OC, crossAC;
    VECTOR3D Norm_x, Norm_y, Norm_z, tmpAxis;
    MATRIX3D Mat1;

    p1.item01 = beginPos.x;
    p1.item02 = beginPos.y;
    p1.item03 = beginPos.z;

    p2.item01 = midPos.x;
    p2.item02 = midPos.y;
    p2.item03 = midPos.z;

    p3.item01 = endPos.x;
    p3.item02 = endPos.y;
    p3.item03 = endPos.z;

    Plane_A = p2.item02 * p3.item03 - p2.item03 * p3.item02 - p1.item02 * p3.item03 + p1.item03 * p3.item02 +
              p1.item02 * p2.item03 - p1.item03 * p2.item02;
    Plane_B = -(p2.item01 * p3.item03 - p2.item03 * p3.item01 - p1.item01 * p3.item03 + p1.item03 * p3.item01 +
                p1.item01 * p2.item03 - p1.item03 * p2.item01);
    Plane_C = p2.item01 * p3.item02 - p2.item02 * p3.item01 - p1.item01 * p3.item02 + p1.item02 * p3.item01 +
              p1.item01 * p2.item02 - p1.item02 * p2.item01;
    Plane_D = (p2.item01 * p3.item02 - p2.item02 * p3.item01) * p1.item03 -
              (p1.item01 * p3.item02 - p1.item02 * p3.item01) * p2.item03 +
              (p1.item01 * p2.item02 - p1.item02 * p2.item01) * p3.item03;

    a1 = p1.item01 - p2.item01;
    b1 = p1.item02 - p2.item02;
    c1 = p1.item03 - p2.item03;
    d1 = (float)(p1.item01 * p1.item01 + p1.item02 * p1.item02 + p1.item03 * p1.item03 - p2.item01 * p2.item01 -
                 p2.item02 * p2.item02 - p2.item03 * p2.item03) *
         0.5f;
    a2 = p1.item01 - p3.item01;
    b2 = p1.item02 - p3.item02;
    c2 = p1.item03 - p3.item03;
    d2 = (float)(p1.item01 * p1.item01 + p1.item02 * p1.item02 + p1.item03 * p1.item03 - p3.item01 * p3.item01 -
                 p3.item02 * p3.item02 - p3.item03 * p3.item03) *
         0.5f;

    det1 = a1 * (b2 * Plane_C - c2 * Plane_B) - a2 * (b1 * Plane_C - c1 * Plane_B) + Plane_A * (b1 * c2 - c1 * b2);
    if (fabs(det1) < ZERO)
    {
        // return ERR_CIRDEGRAD;
        // AlarmSysSetBit(ERR_PLAN_JUMP_PARAM,true);
        return 1;
    }
    det2 = d1 * (b2 * Plane_C - c2 * Plane_B) - d2 * (b1 * Plane_C - c1 * Plane_B) + Plane_D * (b1 * c2 - c1 * b2);
    det3 = a1 * (d2 * Plane_C - c2 * Plane_D) - a2 * (d1 * Plane_C - c1 * Plane_D) + Plane_A * (d1 * c2 - c1 * d2);
    det4 = a1 * (b2 * Plane_D - d2 * Plane_B) - a2 * (b1 * Plane_D - d1 * Plane_B) + Plane_A * (b1 * d2 - d1 * b2);

    tmpCenter.item01 = det2 / det1;
    tmpCenter.item02 = det3 / det1;
    tmpCenter.item03 = det4 / det1;

    tmpR = (float)sqrt((tmpCenter.item01 - p1.item01) * (tmpCenter.item01 - p1.item01) +
                       (tmpCenter.item02 - p1.item02) * (tmpCenter.item02 - p1.item02) +
                       (tmpCenter.item03 - p1.item03) * (tmpCenter.item03 - p1.item03));

    //??????
    OA.item01 = (p1.item01 - tmpCenter.item01);
    OA.item02 = (p1.item02 - tmpCenter.item02);
    OA.item03 = (p1.item03 - tmpCenter.item03);
    OC.item01 = (p3.item01 - tmpCenter.item01);
    OC.item02 = (p3.item02 - tmpCenter.item02);
    OC.item03 = (p3.item03 - tmpCenter.item03);

    crossAC.item01 = OA.item02 * OC.item03 - OA.item03 * OC.item02;
    crossAC.item02 = OA.item03 * OC.item01 - OA.item01 * OC.item03;
    crossAC.item03 = OA.item01 * OC.item02 - OA.item02 * OC.item01;

    tmpNorm.item01 = b1 * c2 - c1 * b2;
    tmpNorm.item02 = c1 * a2 - a1 * c2;
    tmpNorm.item03 = a1 * b2 - a2 * b1;
    flag1 = (float)sqrt(tmpNorm.item01 * tmpNorm.item01 + tmpNorm.item02 * tmpNorm.item02 +
                        tmpNorm.item03 * tmpNorm.item03);
    Norm_z.item01 = tmpNorm.item01 / flag1;
    Norm_z.item02 = tmpNorm.item02 / flag1;
    Norm_z.item03 = tmpNorm.item03 / flag1;

    tmpAngle = (float)acos((OA.item01 * OC.item01 + OA.item02 * OC.item02 + OA.item03 * OC.item03) / tmpR / tmpR);
    flag1 = crossAC.item01 * Norm_z.item01 + crossAC.item02 * Norm_z.item02 + crossAC.item03 * Norm_z.item03;
    if (flag1 >= 0)
    {
        Angle = tmpAngle;
    }
    else
    {
        Angle = PI * 2 - tmpAngle;
    }

    // Angle = 10*PI;

    Norm_x.item01 = OA.item01 / tmpR;
    Norm_x.item02 = OA.item02 / tmpR;
    Norm_x.item03 = OA.item03 / tmpR;
    Norm_y.item01 = Norm_z.item02 * Norm_x.item03 - Norm_z.item03 * Norm_x.item02;
    Norm_y.item02 = Norm_z.item03 * Norm_x.item01 - Norm_z.item01 * Norm_x.item03;
    Norm_y.item03 = Norm_z.item01 * Norm_x.item02 - Norm_z.item02 * Norm_x.item01;

    Mat1.item11 = Norm_x.item01;
    Mat1.item21 = Norm_x.item02;
    Mat1.item31 = Norm_x.item03;
    Mat1.item12 = Norm_y.item01;
    Mat1.item22 = Norm_y.item02;
    Mat1.item32 = Norm_y.item03;
    Mat1.item13 = Norm_z.item01;
    Mat1.item23 = Norm_z.item02;
    Mat1.item33 = Norm_z.item03;

    tmpTheta = endPos.r - beginPos.r;
    tmpAxis.item01 = 0.0;
    tmpAxis.item02 = 0.0;
    if (tmpTheta > 0)
    {
        tmpAxis.item03 = 1;
    }
    else
    {
        tmpAxis.item03 = -1;
    }

    ArcPlanParam->arcRadius = tmpR;
    ArcPlanParam->arcAng = Angle;
    ArcPlanParam->rotAng = (float)fabs(tmpTheta);
    memcpy(&ArcPlanParam->planeRotM, &Mat1, sizeof(MATRIX3D));
    memcpy(&ArcPlanParam->arcCenter, &tmpCenter, sizeof(VECTOR3D));
    memcpy(&ArcPlanParam->rotAxis, &tmpAxis, sizeof(VECTOR3D));

    return 0;
}

void GetSpeedCircle()
{
    float t0, T;
    TCPosition position;
    TRobotTheta theta;
    int32_t calInvErr;
    periodCount++;
    t0 = (float)periodCount * periodTime;
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

    TP_CalcArcPtForScara(gCircleMove.beginPos, gCircleMove.circlePlanPara.planeRotM,
                         gCircleMove.circlePlanPara.arcCenter, gCircleMove.circlePlanPara.rotAxis,
                         gCircleMove.circlePlanPara.arcRadius,
                         gLineMove.stepLastL / gCircleMove.circlePlanPara.arcRadius, gLineMove.stepLastR, &position);

    calInvErr = InverseCal(&position, 0, &theta);

    if (0 != calInvErr)
    {
        if (ERR_PLAN_INV_SINGULARITY == calInvErr)
        {
            AlarmSysSetBit(ERR_MOVE_INV_SINGULARITY, true);
        }
        else if (ERR_PLAN_INV_CALC == calInvErr)
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

void MovC()
{
    float vel, jerk[2];
    float arcLen;
    float s, v, T;
    int32_t calInvErr;

    if (finishFlag)
        return;
    if (!planFlag)
    {
        gCircleMove.beginPos.x = gSysParams.pose.x;
        gCircleMove.beginPos.y = gSysParams.pose.y;
        gCircleMove.beginPos.z = gSysParams.pose.z;
        gCircleMove.beginPos.r = gSysParams.pose.rHead;

        if (fabs(gCircleMove.midPos.x - gSysParams.pose.x) < ZERO &&
            fabs(gCircleMove.midPos.y - gSysParams.pose.y) < ZERO &&
            fabs(gCircleMove.midPos.z - gSysParams.pose.z) < ZERO &&
            fabs(gCircleMove.midPos.r - gSysParams.pose.rHead) < ZERO)
        {
            AlarmSysSetBit(ERR_PLAN_PUSH_DATA_REPEAT, true);
            finishFlag = true;
            planFlag = false;
            return;
        }
        if (fabs(gCircleMove.endPos.x - gSysParams.pose.x) < ZERO &&
            fabs(gCircleMove.endPos.y - gSysParams.pose.y) < ZERO &&
            fabs(gCircleMove.endPos.z - gSysParams.pose.z) < ZERO &&
            fabs(gCircleMove.endPos.r - gSysParams.pose.rHead) < ZERO)
        {
            AlarmSysSetBit(ERR_PLAN_PUSH_DATA_REPEAT, true);
            finishFlag = true;
            planFlag = false;
            return;
        }

        calInvErr = TP_ArcPlanForScara(gCircleMove.beginPos, gCircleMove.midPos, gCircleMove.endPos,
                                       &gCircleMove.circlePlanPara);
        if (calInvErr != 0)
        {
            AlarmSysSetBit(ERR_PLAN_ARC_INPUT_PARAM, true);
            finishFlag = true;
            planFlag = false;
            return;
        }

        arcLen = gCircleMove.circlePlanPara.arcAng * gCircleMove.circlePlanPara.arcRadius;

        Line.finishFlag = arcLen < 1e-6f;
        Gesture.finishFlag = gCircleMove.circlePlanPara.rotAng < 1e-6f;
        vel = gSysParams.ptp.params.coordinateParams.xyzVelocity * velRatio;
        jerk[0] = gSysParams.ptp.params.coordinateParams.xyzAcceleration * accRatio;

        Line.v = vel;
        Line.a = jerk[0];
        Line.s = arcLen;

        s = Line.v * Line.v / Line.a;

        if (s < Line.s)
        {
            Line.t1 = Line.v / Line.a;
            Line.t2 = (Line.s - s) / Line.v;
            Line.T = 2 * Line.t1 + Line.t2;
        }
        else
        {
            v = (float)sqrt(Line.a * Line.s);
            Line.t1 = v / Line.a;
            Line.t2 = 0;
            Line.T = 2 * Line.t1;
            Line.v = v;
        }

        vel = gSysParams.ptp.params.coordinateParams.rVelocity * velRatio;
        jerk[1] = gSysParams.ptp.params.coordinateParams.rAcceleration * accRatio;

        Gesture.v = vel;
        Gesture.a = jerk[1];
        Gesture.s = gCircleMove.circlePlanPara.rotAng;

        s = Gesture.v * Gesture.v / Gesture.a;

        if (s < Gesture.s)
        {
            Gesture.t1 = Gesture.v / Gesture.a;
            Gesture.t2 = (Gesture.s - s) / Gesture.v;
            Gesture.T = 2 * Gesture.t1 + Gesture.t2;
        }
        else
        {
            v = (float)sqrt(Gesture.a * Gesture.s);
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

        planFlag = true;
        gLineMove.stepLastL = 0;
        gLineMove.stepLastR = 0;
        periodCount = 0;
    }

    if (!Line.finishFlag || !Gesture.finishFlag)
    {
        GetSpeedCircle();
    }
    else
    {
        finishFlag = true;
        planFlag = false;
        gLineMove.stepLastL = 0;
        gLineMove.stepLastR = 0;
        periodCount = 0;
    }

    if (finishFlag && stopFinishFlag)
    {
        gRobotPrfMode = ROBOT_MODE_NONE;
        stopFinishFlag = false;
    }
}
