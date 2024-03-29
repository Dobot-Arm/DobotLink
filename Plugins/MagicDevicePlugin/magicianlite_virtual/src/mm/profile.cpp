/**
 ***********************************************************************************************************************
 *
 * @author  ZhangRan
 * @version 1.0.0
 *
 * <h2><center>&copy; COPYRIGHT 2022 </center></h2>
 *
 ***********************************************************************************************************************
 */

#include "profile.h"

#include <QDateTime>
#include <algorithm/type.h>
#include <algorithm/teach.h>
#include <algorithm/variable.h>
#include <algorithm/playBack.h>
#include <algorithm/rtKinematicCalc.h>

#include "ParaComm.h"
#include "algorithm/alarms.h"
#include <QDateTime>

#include "log/log.h"

#include <list>
#include <dcps/cmd_id.h>
#include "dcps/dcps.h"


#define AXIS1_NEG_LIMIT   (-135.1)      //机械限位-140
#define AXIS1_POS_LIMIT   (135.1)       //机械限位140
#define AXIS2_NEG_LIMIT   (-8.1)        //机械限位-12度
#define AXIS2_POS_LIMIT   (80.1)        //机械限位85度
#define AXIS3_NEG_LIMIT   (-6.8)       //机械限位-15度
#define AXIS3_POS_LIMIT   (85.1)        //机械限位90度
#define AXIS4_NEG_LIMIT   (-145.1)
#define AXIS4_POS_LIMIT   (145.1)
#define AXIS23_NEG_LIMIT   (20)
#define AXIS23_POS_LIMIT   (145)

Profile::Profile(QObject* parent) : QObject(parent), prev_time_(0.0f)
{
    timer_ = new QTimer(this);
}

void Profile::init()
{
    InitRobot();
    InitInterpolation();
    connect(timer_, &QTimer::timeout, this, &Profile::elapse);
    prev_time_ = QDateTime::currentMSecsSinceEpoch();
    timer_->start(40);
}

void Profile::elapse()
{
    uint64_t current = QDateTime::currentMSecsSinceEpoch();
    periodTime = (float)(current - prev_time_) / 1000.0f;
    prev_time_ = current;

    KineticFdCal();
    ProfileCalculate();
    jointCal(periodTime);
    DobotProtocolSlave::instance()->doQueueCmd();
}

void Profile::KineticFdCal()
{
    TCPosition curCoord;
    TRobotTheta theta;

    theta.theta[0] = gSysParams.pose.jointAngle[0];
    theta.theta[1] = gSysParams.pose.jointAngle[1];
    theta.theta[2] = gSysParams.pose.jointAngle[2];
    theta.theta[3] = gSysParams.pose.jointAngle[3];

    ForwardCal(&theta, &curCoord);

    gSysParams.pose.x = curCoord.x;
    gSysParams.pose.y = curCoord.y;
    gSysParams.pose.z = curCoord.z;
    gSysParams.pose.rHead = curCoord.r;
}

void Profile::InitRobot()
{
    gStepper.stepAngle = 1.8;

    for (uint32_t i = 0; i < ROBOT_AXIS; ++i)
    {
        gStepper.subDiv[i] = 16;
        gStepper.gearRatio[i] = 10;
        gStepper.stepPerDegree[i] = gStepper.subDiv[i] * gStepper.gearRatio[i] / gStepper.stepAngle;
        gPulse.angleFPGA[i] = 0;
    }

    /*Load device name from the filesystem, if not valid, then initialize!*/
    strcpy(gSysParams.deviceName, "Dobot 2.0");
    strcpy(gSysParams.deviceSN, "00000001");

    gStepper.stepDir[0] = -1;
    gStepper.stepDir[1] = 1;
    gStepper.stepDir[2] = -1;

    periodCount = 0;

    gSysParams.RotAngleError = 0;
    gSysParams.rearArmAngleError = 0;
    gSysParams.rearLinkError = 0;

    gDobotMechPara.joint1ArmLen = BIGARM_LENGTH + gSysParams.rearLinkError;
    gDobotMechPara.joint2ArmLen = SMALLARM_LENGTH + gSysParams.frontLinkError;
    gDobotMechPara.baseH = -3;//90;
    gDobotMechPara.baseV = 62;
    gDobotMechPara.linkBlockV = 67;//63.5;
    gDobotMechPara.linkBlockY = 0;

    /*gSoftLimit.jointNeg[0] = -135.1;
    gSoftLimit.jointPos[0] = 135.1;
    gSoftLimit.jointNeg[1] = -8.1;
    gSoftLimit.jointPos[1] = 80.1;
    gSoftLimit.jointNeg[2] = -6.8;
    gSoftLimit.jointPos[2] = 85.1;
    gSoftLimit.jointNeg[3] = -145.1;
    gSoftLimit.jointPos[3] = 145.1;
    gSoftLimit.parallelJoint12Neg = 20;
    gSoftLimit.parallelJoint12Pos = 145;*/

    gSoftLimit.jointNeg[0] = AXIS1_NEG_LIMIT;
    gSoftLimit.jointPos[0] = AXIS1_POS_LIMIT;
    gSoftLimit.jointNeg[1] = AXIS2_NEG_LIMIT;
    gSoftLimit.jointPos[1] = AXIS2_POS_LIMIT;
    gSoftLimit.jointNeg[2] = AXIS3_NEG_LIMIT;
    gSoftLimit.jointPos[2] = AXIS3_POS_LIMIT;
    gSoftLimit.jointNeg[3] = AXIS4_NEG_LIMIT;
    gSoftLimit.jointPos[3] = AXIS4_POS_LIMIT;
    gSoftLimit.parallelJoint12Neg = AXIS23_NEG_LIMIT;
    gSoftLimit.parallelJoint12Pos = AXIS23_POS_LIMIT;

    /*DH params*/
    gSysParams.dhParams.theta_min[0] = AXIS1_NEG_LIMIT;
    gSysParams.dhParams.theta_min[1] = AXIS2_NEG_LIMIT;
    gSysParams.dhParams.theta_min[2] = AXIS3_NEG_LIMIT;
    gSysParams.dhParams.theta_min[3] = AXIS4_NEG_LIMIT;
    gSysParams.dhParams.theta_min[4] = AXIS23_NEG_LIMIT;
    gSysParams.dhParams.theta_max[0] = AXIS1_POS_LIMIT;
    gSysParams.dhParams.theta_max[1] = AXIS2_POS_LIMIT;
    gSysParams.dhParams.theta_max[2] = AXIS3_POS_LIMIT;
    gSysParams.dhParams.theta_max[3] = AXIS4_POS_LIMIT;
    gSysParams.dhParams.theta_max[4] = AXIS23_POS_LIMIT;
    gSysParams.dhParams.DH[0] = BIGARM_LENGTH;
    gSysParams.dhParams.DH[1] = SMALLARM_LENGTH;
    gSysParams.dhParams.DH[2] = END_LENGTH;
    gSysParams.dhParams.DH[3] = 0;
    gSysParams.dhParams.Z_Limit[0] = -45;
    gSysParams.dhParams.Z_Limit[1] = 175;

    /*Optimize the access*/
    SysParams* sysParams = &gSysParams;

    Pose* pose = &sysParams->pose;

    pose->jointAngle[0] = 0;
    pose->jointAngle[3] = 0;
    pose->x = 0;
    pose->y = 0;
    pose->z = 0;
    pose->rHead = 0;

    sysParams->armOrientation = LeftyArmOrientation;
    HoldRHeadFlag = 0;
    CP_TrackFlag = 0;
    JumpLineFlag = 0;

    /*JOG*/
    JOGJointParams* jogJointParams = &sysParams->jog.params.jointParams;
    JOGCoordinateParams* jogCoordinateParams = &sysParams->jog.params.coordinateParams;
    JOGCommonParams* jogCommonParams = &sysParams->jog.params.commonParams;

    jogJointParams->velocity[0] = 15;
    jogJointParams->velocity[1] = 15;
    jogJointParams->velocity[2] = 15;
    jogJointParams->velocity[3] = 30;
    jogJointParams->acceleration[0] = 50;
    jogJointParams->acceleration[1] = 50;
    jogJointParams->acceleration[2] = 50;
    jogJointParams->acceleration[3] = 50;

    jogCoordinateParams->velocity[0] = 60;
    jogCoordinateParams->acceleration[0] = 60;
    jogCoordinateParams->velocity[1] = 60;
    jogCoordinateParams->acceleration[1] = 60;
    jogCoordinateParams->velocity[2] = 60;
    jogCoordinateParams->acceleration[2] = 60;
    jogCoordinateParams->velocity[3] = 60;
    jogCoordinateParams->acceleration[3] = 60;

    jogCommonParams->velocityRatio = 50;
    jogCommonParams->accelerationRatio = 50;

    /* PTP*/
    PTPJointParams* ptpJointParams = &sysParams->ptp.params.jointParams;
    PTPCoordinateParams* ptpCoordinateParams = &sysParams->ptp.params.coordinateParams;
    PTPJumpParams* ptpJumpParams = &sysParams->ptp.params.jumpParams;
    PTPJump2Params *ptpJump2Params = &sysParams->ptp.params.jump2Params;
    PTPCommonParams* ptpCommonParams = &sysParams->ptp.params.commonParams;

    /*ptpJointParams->velocity[0] = 200;
    ptpJointParams->velocity[1] = 200;
    ptpJointParams->velocity[2] = 200;
    ptpJointParams->velocity[3] = 200;
    ptpJointParams->acceleration[0] = 200;
    ptpJointParams->acceleration[1] = 200;
    ptpJointParams->acceleration[2] = 200;
    ptpJointParams->acceleration[3] = 200;*/
    ptpJointParams->velocity[0] = 300;
    ptpJointParams->velocity[1] = 300;
    ptpJointParams->velocity[2] = 300;
    ptpJointParams->velocity[3] = 200;
    ptpJointParams->acceleration[0] = 400;
    ptpJointParams->acceleration[1] = 400;
    ptpJointParams->acceleration[2] = 400;
    ptpJointParams->acceleration[3] = 200;

    /*ptpCoordinateParams->xyzVelocity = 200;
    ptpCoordinateParams->xyzAcceleration = 200;
    ptpCoordinateParams->rVelocity = 100;
    ptpCoordinateParams->rAcceleration = 100;*/
    ptpCoordinateParams->xyzVelocity = 300;
    ptpCoordinateParams->xyzAcceleration = 400;
    ptpCoordinateParams->rVelocity = 200;
    ptpCoordinateParams->rAcceleration = 200;

    ptpJumpParams->jumpHeight = 20;
    //ptpJumpParams->maxJumpHeight = 100;
    ptpJumpParams->maxJumpHeight = 150;

    ptpJump2Params->startJumpHeight = 20;
    ptpJump2Params->endJumpHeight = 20;
    ptpJump2Params->zLimit = 150;

    ptpCommonParams->velocityRatio = 50;
    ptpCommonParams->accelerationRatio = 50;

    /* CP*/
    CPParams* cpParams = &sysParams->cp.params;

    cpParams->planAcc = 100;
    cpParams->juncitionVel = 100;
    cpParams->acc = 100;
    vbg = 0;
    gLookAheadParams = *cpParams;

    heightMiddle = 70;

    //LostStep
    sysParams->LostStepValue = 5.0;
    sysParams->CollisionCheckValue = 10;

    sysParams->queuedCmdType = QueuedCmdMotionType;
    sysParams->runQueuedCmd = true;

    sysParams->runOfflineQueuedCmd = false;
    sysParams->runOfflineQueuedCmdState = RunOfflineQueuedCmdIdleState;

    sysParams->queuedCmdOfflineLoopLine.isDownloading = false;    //默认isDownloading =false  stop
    sysParams->queuedCmdOfflineLoopLine.readFlag = true;
    sysParams->queuedCmdOfflineLoopLine.writeFlag = false;
    sysParams->queuedCmdOfflineLoopLine.loopIndex = 0;
    sysParams->queuedCmdOfflineLoopLine.lineIndex = 0;

    /*Soft-Off*/
    sysParams->isSoftOffTriggered = false;

    for (auto& home : gHome)
    {
        home.homeState = 0;
        home.seekHomeSpd = 20;
        home.backHomeSpd = 1;
        home.findZSpd = 0.5;
    }

    gHome[0].homeDir = -1;
    gHome[1].homeDir = 1;
    gHome[2].homeDir = -1;

    gHome[0].homePos = 0;
    gHome[1].homePos = 90 - 68.4;
    gHome[2].homePos = 5.6;

    gHomePoint.theta[0] = 0;
    /*gHomePoint.theta[1] = 45;
    gHomePoint.theta[2] = 45;*/
    gHomePoint.theta[1] = 0;
    gHomePoint.theta[2] = 0;
    gHomePoint.theta[3] = 0;

    /*******************************************************************************/
    if((gEffectorType != SUCTIONCUP) && (gEffectorType != GRIPPER) && (gEffectorType != PEN)){  //新机器初次上电读到的默认值不一定为1、2、3
        gDobotMechPara.linkBlockH = gSuctioncup_LinkBlockH;
    }
    if(gEffectorType == SUCTIONCUP){
        gDobotMechPara.linkBlockH = gSuctioncup_LinkBlockH;
    }else if(gEffectorType == GRIPPER){
        gDobotMechPara.linkBlockH = gGripper_LinkBlockH;
    }else if(gEffectorType == PEN){
        gDobotMechPara.linkBlockH = gPen_LinkBlockH;
    }
    gSoftLimit.disMOVJ = gDobotMechPara.joint2ArmLen + gDobotMechPara.linkBlockH;
    /*******************************************************************************/
}

void Profile::InitInterpolation()
{
    gRobotPrfMode = ROBOT_MODE_NONE;
    flashEnd = 1;
    finishFlag = true;
    planFlag = false;
    stopFinishFlag = false;

    gSingleMove.singleVel = 0;
    gSingleMove.singleVelMax = 20;
    gSingleMove.singleAcc = 20;

    gJointMove.speed[0] = 0;
    gJointMove.speed[1] = 0;
    gJointMove.speed[2] = 0;
    gJointMove.speed[3] = 0;

    gPulse.stepSum[0] = 0;
    gPulse.stepSum[1] = 0;
    gPulse.stepSum[2] = 0;
    gPulse.stepSum[3] = 0;

    gPulse.sendPulseSum[0] = 0;
    gPulse.sendPulseSum[1] = 0;
    gPulse.sendPulseSum[2] = 0;
    gPulse.sendPulseSum[3] = 0;

    gPulse.sendSpeedSum[0] = 0;
    gPulse.sendSpeedSum[1] = 0;
    gPulse.sendSpeedSum[2] = 0;
    gPulse.sendSpeedSum[3] = 0;

    transFlag = false;
    jumPlanFlag = false;
    downFlag = false;

    velRatio = 1.0f;//0.5;
    accRatio = 1.0f;//0.5;

    singleVelRatio = 1.0f;//0.5;
}

void Profile::ProfileCalculate()
{
    if (finishFlag) {
        ClearSpeed();
    }
    switch (gRobotPrfMode)
    {
        case ROBOT_MODE_NONE:
            ClearSpeed();
            break;

        case ROBOT_MODE_SINGLE:
            Single(gSingleMode);
            break;

        case ROBOT_MODE_SINGLEXYZ:
            SingleXY(gSingleMode);
            break;

        case ROBOT_MODE_HOME:
            Playback(0.0, 0.0, 0.0, 0.0, MOVJ_ANGLE);
            break;

        case ROBOT_MODE_LEVELING:
            if (CalibrationMode == 1)
            {
                //BigArmParams_Calibration();
            }
            else if (CalibrationMode == 2)
            {
                //SmallArmParams_Calibration();
            }
            else if (CalibrationMode == 3)
            {
                //BaseLeveling_Calibration();
            }
            else if (CalibrationMode == 4)
            {
                // todo:
            }
            break;

        case ROBOT_MODE_PLAYBACK: {
            PTPCmd* cmd = &gSysParams.ptp.cmd;
            if (cmd->ptpMode == JUMP_MOVL_XYZ)
            {
                JumpLineFlag = 1;
                cmd->ptpMode = JUMP_XYZ;
            }
            else
            {
                JumpLineFlag = 0;
            }
            Playback(cmd->x, cmd->y, cmd->z, cmd->rHead, cmd->ptpMode);
        }
        break;

        case ROBOT_MODE_CONTINUOUS_PATH: {
            //            CPCmd* cmd = &gSysParams.cp.cmd;
            //                CPMotion(cmd->lookAhead.startVel, cmd->lookAhead.endVel, cmd->lookAhead.maxVel, cmd->x,
            //                cmd->y, cmd->z,
            //                         0, 0, cmd->cpMode);
        }
        break;

        case ROBOT_MODE_ARC:
            //            MOVC();
            break;

        case ROBOT_MODE_STOP:
            Stop();
            break;

        default:
            break;
    }
}
/*
void Profile::jointCal(float duration)
{
    float val;
    val = gSysParams.pose.jointAngle[0] + gJointMove.speed[0] * duration;
    gSysParams.pose.jointAngle[0] = getJoint(gSoftLimit.jointNeg[0], gSoftLimit.jointPos[0], val);
    val = gSysParams.pose.jointAngle[1] + gJointMove.speed[1] * duration;
    gSysParams.pose.jointAngle[1] = getJoint(gSoftLimit.jointNeg[1], gSoftLimit.jointPos[1], val);
    val = gSysParams.pose.jointAngle[2] + gJointMove.speed[2] * duration;
    gSysParams.pose.jointAngle[2] = getJoint(gSoftLimit.jointNeg[2], gSoftLimit.jointPos[2], val);
    val = gSysParams.pose.jointAngle[3] + gJointMove.speed[3] * duration;
    gSysParams.pose.jointAngle[3] = getJoint(gSoftLimit.jointNeg[3], gSoftLimit.jointPos[3], val);

    float* joints = gSysParams.pose.jointAngle;
    emit updateJoints(joints[0], joints[1], joints[2], joints[3]);
}
*/
float Profile::getJoint(float min_val, float max_val, float val)
{
    return std::min(std::max(val, min_val), max_val);
}

bool AdjustJoint(float& val,int idx, int axisNeg, int axisPos)
{
    bool bOutOfRange = false;
    if (val < gSoftLimit.jointNeg[idx])
    {
        val = gSoftLimit.jointNeg[idx];
        gSingleMove.singleVel = 0;
        AlarmSysSetBit(axisNeg,true);
        bOutOfRange = true;
    }
    else if (val > gSoftLimit.jointPos[idx])
    {
        val = gSoftLimit.jointPos[idx];
        gSingleMove.singleVel = 0;
        AlarmSysSetBit(axisPos,true);
        bOutOfRange = true;
    }
    else
    {
        AlarmSysSetBit(axisNeg,false);
        AlarmSysSetBit(axisPos,false);
    }
    return bOutOfRange;
}

void Profile::jointCal(float duration)
{
    if (ROBOT_MODE_HOME != gRobotPrfMode)
    {
        if (gSysParams.pose.jointAngle[0]<gSoftLimit.jointNeg[0]) {
            gSingleMove.singleVel = 0;
            AlarmSysSetBit(ERR_LIMIT_AXIS1_NEG,true);
        } else {
            AlarmSysSetBit(ERR_LIMIT_AXIS1_NEG,false);
        }

        if (gSysParams.pose.jointAngle[0]>gSoftLimit.jointPos[0]) {
            gSingleMove.singleVel = 0;
            AlarmSysSetBit(ERR_LIMIT_AXIS1_POS,true);
        } else {
            AlarmSysSetBit(ERR_LIMIT_AXIS1_POS,false);
        }

        if (gSysParams.pose.jointAngle[1]<gSoftLimit.jointNeg[1]) {
            gSingleMove.singleVel = 0;
            AlarmSysSetBit(ERR_LIMIT_AXIS2_NEG,true);
        } else {
            AlarmSysSetBit(ERR_LIMIT_AXIS2_NEG,false);
        }

        if (gSysParams.pose.jointAngle[1]>gSoftLimit.jointPos[1]) {
            gSingleMove.singleVel = 0;
            AlarmSysSetBit(ERR_LIMIT_AXIS2_POS,true);
        } else {
            AlarmSysSetBit(ERR_LIMIT_AXIS2_POS,false);
        }

        if (gSysParams.pose.jointAngle[2]<gSoftLimit.jointNeg[2]) {
            gSingleMove.singleVel = 0;
            AlarmSysSetBit(ERR_LIMIT_AXIS3_NEG,true);
        } else {
            AlarmSysSetBit(ERR_LIMIT_AXIS3_NEG,false);
        }

        if (gSysParams.pose.jointAngle[2]>gSoftLimit.jointPos[2]) {
            gSingleMove.singleVel = 0;
            AlarmSysSetBit(ERR_LIMIT_AXIS3_POS,true);
        } else {
            AlarmSysSetBit(ERR_LIMIT_AXIS3_POS,false);
        }

        if (gSysParams.pose.jointAngle[3]<gSoftLimit.jointNeg[3]) {
            gSingleMove.singleVel = 0;
            AlarmSysSetBit(ERR_LIMIT_AXIS4_NEG,true);
        } else {
            AlarmSysSetBit(ERR_LIMIT_AXIS4_NEG,false);
        }

        if (gSysParams.pose.jointAngle[3]>gSoftLimit.jointPos[3]) {
            gSingleMove.singleVel = 0;
            AlarmSysSetBit(ERR_LIMIT_AXIS4_POS,true);
        } else {
            AlarmSysSetBit(ERR_LIMIT_AXIS4_POS,false);
        }

        if (90+gSysParams.pose.jointAngle[1]-gSysParams.pose.jointAngle[2]<gSoftLimit.parallelJoint12Neg) {
            gSingleMove.singleVel = 0;
            AlarmSysSetBit(ERR_LIMIT_AXIS23_NEG,true);
        } else {
            AlarmSysSetBit(ERR_LIMIT_AXIS23_NEG,false);
        }

        if (90+gSysParams.pose.jointAngle[1]-gSysParams.pose.jointAngle[2]>gSoftLimit.parallelJoint12Pos) {
            gSingleMove.singleVel = 0;
            AlarmSysSetBit(ERR_LIMIT_AXIS23_POS,true);
        } else {
            AlarmSysSetBit(ERR_LIMIT_AXIS23_POS,false);
        }
    }

    if ((ROBOT_MODE_HOME != gRobotPrfMode)&&
         ((gSysParams.pose.jointAngle[0]<gSoftLimit.jointNeg[0] && gJointMove.speed[0]<0)
          || (gSysParams.pose.jointAngle[0]>gSoftLimit.jointPos[0] && gJointMove.speed[0]>0)
          ||(gSysParams.pose.jointAngle[1]<gSoftLimit.jointNeg[1] && gJointMove.speed[1]<0)
          ||(gSysParams.pose.jointAngle[1]>gSoftLimit.jointPos[1] && gJointMove.speed[1]>0)
          ||(gSysParams.pose.jointAngle[2]<gSoftLimit.jointNeg[2] && gJointMove.speed[2]<0)
          ||(gSysParams.pose.jointAngle[2]>gSoftLimit.jointPos[2] && gJointMove.speed[2]>0)
          ||(gSysParams.pose.jointAngle[3]<gSoftLimit.jointNeg[3] && gJointMove.speed[3]<0)
          ||(gSysParams.pose.jointAngle[3]>gSoftLimit.jointPos[3] && gJointMove.speed[3]>0)
          ||(90+gSysParams.pose.jointAngle[1]-gSysParams.pose.jointAngle[2]<gSoftLimit.parallelJoint12Neg && (gJointMove.speed[1]-gJointMove.speed[2]<0))
          ||(90+gSysParams.pose.jointAngle[1]-gSysParams.pose.jointAngle[2]>gSoftLimit.parallelJoint12Pos&& (gJointMove.speed[1]-gJointMove.speed[2]>0))
          )
       )
    {
        finishFlag = true;
        planFlag = false;
        return;
    } else {
        gSysParams.pose.jointAngle[0] += gJointMove.speed[0]*duration;
        gSysParams.pose.jointAngle[1] += gJointMove.speed[1]*duration;
        gSysParams.pose.jointAngle[2] += gJointMove.speed[2]*duration;
        gSysParams.pose.jointAngle[3] += gJointMove.speed[3]*duration;
    }
    float* joints = gSysParams.pose.jointAngle;
    emit updateJoints(joints[0], joints[1], joints[2], joints[3]);
}
