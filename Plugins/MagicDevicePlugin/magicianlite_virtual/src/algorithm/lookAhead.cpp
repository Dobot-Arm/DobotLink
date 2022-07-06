/****************************************Copyright(c)*****************************************************
**                            Shenzhen Yuejiang Technology Co., LTD.
**
**                                 http://www.dobot.cc
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           lookahead.cpp
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
#include <math.h>
#include "playBack.h"
#include "rtKinematicCalc.h"
//#include "command.h"
//#include "../signals/signals.h"
#include "alarms.h"

/*********************************************************************************************************
** Variables
*********************************************************************************************************/
volatile float le;
volatile float li;
volatile float vst, vbt, vet;
volatile float sa = 0;
volatile float su = 0;
volatile float sde = 0;
volatile float sum = 0;
volatile float lrem = 0;
volatile int add = 0;
volatile int unfm = 0;
volatile int dec = 0;
volatile float vb = 0;
volatile float AT = 0;
int testcnt = 0;

//static float vbg = 0;

/*********************************************************************************************************
** Function name:       line_interp_plan
** Descriptions:
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void line_interp_plan(typ_interp_segment * interpl_segment)
{
    if (interpl_segment->vb>interpl_segment->interp_time.vel) {
        interpl_segment->vb = interpl_segment->interp_time.vel;
    }

    if (interpl_segment->ve>interpl_segment->interp_time.vel) {
        interpl_segment->ve = interpl_segment->interp_time.vel;
    }

    vst = (float)(interpl_segment->interp_time.vel*interpl_segment->interp_time.vel);
    vbt = (float)(interpl_segment->vb*interpl_segment->vb);
    vet = (float)(interpl_segment->ve*interpl_segment->ve);

    if (interpl_segment->dist > LINE_MIN_DIST) {
        le = (vst - 0.5f*vbt - 0.5f*vet) / interpl_segment->accelat;

        if (le <= interpl_segment->dist) {
            interpl_segment->vm = interpl_segment->interp_time.vel;
            interpl_segment->interp_time.term_unfm = ((interpl_segment->dist - le) / (interpl_segment->vm*periodTime)) + 0.5f;
            interpl_segment->interp_time.term_add = ((interpl_segment->vm - interpl_segment->vb) / interpl_segment->interp_time.AT) + 0.5f;
            interpl_segment->interp_time.term_dec = ((interpl_segment->vm - interpl_segment->ve) / interpl_segment->interp_time.AT) + 0.5f;
        } else {
            li = 0.5f*(vet - vbt) / interpl_segment->accelat;

            if (interpl_segment->dist <= fabs(li)) {
                if (li >= 0) {
                    interpl_segment->vm = sqrt(2 * interpl_segment->accelat*interpl_segment->dist + vbt);
                    interpl_segment->ve = interpl_segment->vm;
                    interpl_segment->interp_time.term_unfm = 0;
                    interpl_segment->interp_time.term_add = ((interpl_segment->ve - interpl_segment->vb) / interpl_segment->interp_time.AT) + 0.5f;
                    interpl_segment->interp_time.term_dec = 0;
                } else {
                    interpl_segment->vm = sqrt(2 * interpl_segment->accelat*interpl_segment->dist + vet);
                    interpl_segment->vb = interpl_segment->vm;
                    interpl_segment->interp_time.term_unfm = 0;
                    interpl_segment->interp_time.term_add = 0;
                    interpl_segment->interp_time.term_dec = ((interpl_segment->vb - interpl_segment->ve) / interpl_segment->interp_time.AT) + 0.5f;
                }
            } else {
                if (interpl_segment->vb<interpl_segment->interp_time.vel*LDEGREE&& \
                    interpl_segment->ve<interpl_segment->interp_time.vel*LDEGREE) {
                    interpl_segment->vm = sqrt(interpl_segment->accelat*interpl_segment->dist + 0.5f*vbt + 0.5f*vet);
                    interpl_segment->interp_time.term_add = ((interpl_segment->vm - interpl_segment->vb) / interpl_segment->interp_time.AT) + 0.5f;
                    interpl_segment->interp_time.term_unfm = 0;
                    interpl_segment->interp_time.term_dec = ((interpl_segment->vm - interpl_segment->ve) / interpl_segment->interp_time.AT) + 0.5f;
                } else {
                    if (li >= 0) {
                        interpl_segment->vm = interpl_segment->ve;
                        interpl_segment->interp_time.term_add = (int)(((interpl_segment->ve - interpl_segment->vb) / interpl_segment->interp_time.AT) + 0.5f);
                        interpl_segment->interp_time.term_dec = 0;
                    } else { //L ???
                        interpl_segment->vm = interpl_segment->vb;
                        interpl_segment->interp_time.term_add = 0;
                        interpl_segment->interp_time.term_dec = (int)(((interpl_segment->vb - interpl_segment->ve) / interpl_segment->interp_time.AT) + 0.5f);
                    }
                }
            }
        }

        add = interpl_segment->interp_time.term_add;
        dec = interpl_segment->interp_time.term_dec;
        AT = interpl_segment->interp_time.AT;
        vb = interpl_segment->vb;

        sa = add*(2 * vb + (add - 1)*AT)*periodTime*0.5f;

        if (add>0) {
            sde = dec*(2 * vb + 2 * (add - 1)*AT - (dec - 1)*AT)*periodTime*0.5f;
        } else {
            sde = dec*(2 * vb - (dec + 1)*AT)*periodTime*0.5f;
        }

        interpl_segment->interp_time.term_unfm = INTERP_RECI*(interpl_segment->dist - sa - sde) / (vb + add*AT);    //????????
        unfm = interpl_segment->interp_time.term_unfm;

        if ((interpl_segment->interp_time.term_unfm + interpl_segment->interp_time.term_add + interpl_segment->interp_time.term_dec) == 0) {
            interpl_segment->interp_time.term_unfm = 1;
        }

        unfm = interpl_segment->interp_time.term_unfm;
        su = unfm*(vb + add*AT)*periodTime;
        sum = sa + su + sde;
        lrem = interpl_segment->dist - sum;
        interpl_segment->interp_time.term_rem = interpl_segment->interp_time.term_add + interpl_segment->interp_time.term_unfm \
                                                + interpl_segment->interp_time.term_dec;
        interpl_segment->interp_time.vDelta = INTERP_RECI*lrem / interpl_segment->interp_time.term_rem;

        if (abs(interpl_segment->interp_time.vDelta)>AT) {
            interpl_segment->interp_time.vDelta = AT;
        }

        if (add>0 && dec>0) {
            interpl_segment->ve = vb + add*AT - dec*AT + interpl_segment->interp_time.vDelta;
        } else if (add == 0 && dec == 0) {
            interpl_segment->ve = vb + interpl_segment->interp_time.vDelta;
        } else if (dec <= 0) {
            if (unfm <= 0) {
                interpl_segment->ve = vb + (add - 1)*AT + interpl_segment->interp_time.vDelta;
            } else {
                interpl_segment->ve = vb + (add)*AT + interpl_segment->interp_time.vDelta;
            }
        } else if (add <= 0) {
            interpl_segment->ve = vb - (dec)*AT + interpl_segment->interp_time.vDelta;
        }

        interpl_segment->interp_time.term_vel = interpl_segment->vb  + interpl_segment->interp_time.vDelta;
//         if(interpl_segment->ve <0 || interpl_segment->vb <0)
//        {
//            int a;
//            a = 1;
//        }
      //  printf("CP %1.3f %1.3f %d %d %d\r\n", interpl_segment->vb, interpl_segment->ve,interpl_segment->interp_time.term_add ,interpl_segment->interp_time.term_unfm, interpl_segment->interp_time.term_dec);
    } else {
        interpl_segment->interp_time.term_cmd = 0;
        interpl_segment->interp_time.term_add = 0;
        interpl_segment->interp_time.term_rem = 0;
        interpl_segment->interp_time.term_unfm = 0;
        interpl_segment->interp_time.term_dec = 0;
        interpl_segment->interp_time.motion_stat = INP_MOTION_END;
        interpl_segment->interp_time.motion_stat_E = INP_MOTION_END;
    }
}

/*********************************************************************************************************
** Function name:       linear_vel_ctrl
** Descriptions:
** Input parameters:    None
** Output parameters:   None
** Returned value:      0
*********************************************************************************************************/
int linear_vel_ctrl(typ_interp_time * interp_time)
{
    if (interp_time->term_add>0) {
        interp_time->term_length = interp_time->term_vel*periodTime;
        interp_time->term_vel += interp_time->AT;
        interp_time->term_add--;
    } else if (interp_time->term_unfm>0) {
        interp_time->term_length = interp_time->term_vel*periodTime;
        interp_time->term_unfm--;
    } else if (interp_time->term_dec>0) {
        interp_time->term_vel -= interp_time->AT;
        interp_time->term_length = interp_time->term_vel*periodTime;
        interp_time->term_dec--;
    }

    if ((interp_time->term_add + interp_time->term_unfm + interp_time->term_dec) == 0) {
        return 1;
    }

    return 0;
}  //end of sub velocity_ctrl()

/*********************************************************************************************************
** Function name:       line_interp_deal
** Descriptions:
** Input parameters:    None
** Output parameters:   None
** Returned value:      isSegDone
*********************************************************************************************************/
int line_interp_deal(typ_interp_segment * interpl_segment)
{
    int isSegDone = linear_vel_ctrl((typ_interp_time *)&interpl_segment->interp_time);
    interpl_segment->dist_out[0] = interpl_segment->interp_time.term_length*interpl_segment->gCode.X / interpl_segment->dist;
    interpl_segment->axis_seg[0] += interpl_segment->dist_out[0];

    interpl_segment->dist_out[1] = interpl_segment->interp_time.term_length*interpl_segment->gCode.Y / interpl_segment->dist;
    interpl_segment->axis_seg[1] += interpl_segment->dist_out[1];

    interpl_segment->dist_out[2] = interpl_segment->interp_time.term_length*interpl_segment->gCode.Z / interpl_segment->dist;
    interpl_segment->axis_seg[2] += interpl_segment->dist_out[2];

    gDistPos.x = gOrigPos.x+interpl_segment->axis_seg[0];
    gDistPos.y = gOrigPos.y+interpl_segment->axis_seg[1];
    gDistPos.z = gOrigPos.z+interpl_segment->axis_seg[2];
    gDistPos.r = 0;

    if (isSegDone) {
        if(LaserGravingFlag == 1){
            gDistPos.x = gOrigPos.x+interpl_segment->gCode.X;
            gDistPos.y = gOrigPos.y+interpl_segment->gCode.Y;
            gDistPos.z = gOrigPos.z+interpl_segment->gCode.Z;
            gDistPos.r = 0;
            LaserGravingFlag = 0;
        }

        printf("CP %1.3f \r\n", interpl_segment->axis_seg[1]);

        finishFlag = true;
        planFlag = false;
        vbg = seg.ve;
    }

    if (isSegDone && stopFinishFlag) {
        gRobotPrfMode = ROBOT_MODE_NONE;
        stopFinishFlag = false;
        vbg = 0;

        if (RingBufferGetCount(&gQueuedCmdCPPPRBHandler) > 1) {
            CPCmd *cmdNext = (CPCmd *)*(uint32_t *)RingBufferDataAt(&gQueuedCmdCPPPRBHandler, 1);
            cmdNext->lookAhead.vc = 0;
            cmdNext->lookAhead.ve = 0;

        }

        ConfirmLaserOff();
    }

    return isSegDone;
}

/*********************************************************************************************************
** Function name:       NewVector3D
** Descriptions:
** Input parameters:    None
** Output parameters:   None
** Returned value:      vector_3d
*********************************************************************************************************/
typedef struct {
    float x, y, z;
    float length;
} Vector3D ;

Vector3D NewVector3D(float x, float y, float z)
{
    Vector3D vector_3d;
    vector_3d.x = x;
    vector_3d.y = y;
    vector_3d.z = z;
    vector_3d.length = sqrt(x*x+y*y+z*z);
    return vector_3d;
}

/*********************************************************************************************************
** Function name:       VectorDot
** Descriptions:
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
float VectorDot(Vector3D *vnow, Vector3D *vlast)
{
    float dot;
    dot = vnow->x*vlast->x + vnow->y*vlast->y + vnow->z*vlast->z;
    return dot;
}

/*********************************************************************************************************
** Function name:       CalaSinAngle
** Descriptions:
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void CalaSinAngle()
{
    Vector3D vlast = NewVector3D(0, 0, 0);
    Vector3D vnow =  NewVector3D(0, 0, 0);
    double angleCos;

    for (int i = 0; i < RingBufferGetCount(&gQueuedCmdCPPPRBHandler); i++) {
        CPCmd *cmd = (CPCmd *)*(uint32_t *)RingBufferDataAt(&gQueuedCmdCPPPRBHandler,i);
        vnow = NewVector3D(cmd->x,cmd->y,cmd->z);

        if (vnow.length != 0) {
            if (vlast.length * vlast.length != 0) {
                angleCos = VectorDot(&vnow, &vlast) / (vnow.length * vlast.length);

                if (angleCos < 0) {
                    angleCos = 0;
                }
            } else {
                angleCos = 1;
            }

            cmd->lookAhead.juctionAngle = sqrt(1 - angleCos * angleCos);
        } else {
            cmd->lookAhead.juctionAngle = 1.0f;
        }

        vlast = vnow;
    }
}

/*********************************************************************************************************
** Function name:       CaAbsDelta
** Descriptions:
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void CaAbsDelta()
{

    for (int i = 0; i < RingBufferGetCount(&gQueuedCmdCPPPRBHandler); i++) {

        CPCmd *cmdnow = (CPCmd *)*(uint32_t *)RingBufferDataAt(&gQueuedCmdCPPPRBHandler,i);

        if (cmdnow->cpMode ==  CPAbsoluteMode) {
            if (i == 0) {
                cmdnow->x = cmdnow->abs_x - gSysParams.pose.x;
                cmdnow->y = cmdnow->abs_y - gSysParams.pose.y;
                cmdnow->z=  cmdnow->abs_z - gSysParams.pose.z;
            } else {
                CPCmd *cmdlast = (CPCmd *)*(uint32_t *)RingBufferDataAt(&gQueuedCmdCPPPRBHandler,i-1);
                cmdnow->x =  cmdnow->abs_x - cmdlast->abs_x;
                cmdnow->y =  cmdnow->abs_y - cmdlast->abs_y;
                cmdnow->z =  cmdnow->abs_z - cmdlast->abs_z;
            }

        } else {
            if (i == 0) {
                cmdnow->abs_x =  gSysParams.pose.x + cmdnow->x;
                cmdnow->abs_y =  gSysParams.pose.y + cmdnow->y;
                cmdnow->abs_z =  gSysParams.pose.z + cmdnow->z;
            } else {
                CPCmd *cmdlast = (CPCmd *)*(uint32_t *)RingBufferDataAt(&gQueuedCmdCPPPRBHandler,i-1);
                cmdnow->abs_x =  cmdlast->abs_x + cmdnow->x;
                cmdnow->abs_y =  cmdlast->abs_y + cmdnow->y;
                cmdnow->abs_z =  cmdlast->abs_z + cmdnow->z;
            }
        }
    }
}

/*********************************************************************************************************
** Function name:       CPLookAhead
** Descriptions:
** Input parameters:    None
** Output parameters:   None
** Returned value:      0
*********************************************************************************************************/
int CPLookAhead()
{
    float vs;
    float vm;

    CaAbsDelta();

    Vector3D vnow = NewVector3D(0, 0, 0);
    CPCmd *cmdTemp = (CPCmd *)*(uint32_t *)RingBufferDataAt(&gQueuedCmdCPPPRBHandler,0);
    vnow = NewVector3D(cmdTemp->x,cmdTemp->y,cmdTemp->z);

    if (vnow.length == 0) {
        return -1;
    }

    if (CP_TrackFlag == 0) {
        CalaSinAngle();
    }

    for (int i = RingBufferGetCount(&gQueuedCmdCPPPRBHandler) - 1; i > 0; i--) {
        CPCmd *cmd = (CPCmd *)*(uint32_t *)RingBufferDataAt(&gQueuedCmdCPPPRBHandler,i);
        cmd->lookAhead.AT = gLookAheadParams.planAcc *  periodTime;
        cmd->lookAhead.dist = sqrt(cmd->x*cmd->x + cmd->y*cmd->y + cmd->z*cmd->z);

        if (CP_TrackFlag == 0) {
            cmd->lookAhead.maxVel  = gSysParams.cp.params.juncitionVel;
            vs = cmd->lookAhead.maxVel;
        } else {
            vs = 1000*cmd->lookAhead.dist / gSysParams.cp.params.acc;

            if (vs > gSysParams.cp.params.juncitionVel) {
                vs  = gSysParams.cp.params.juncitionVel;
            }
        }

        vm = cmd->lookAhead.dist / periodTime;

        if (vs > vm) {
            vs = vm;
        }

        cmd->lookAhead.vd = sqrt(2 * gLookAheadParams.planAcc * cmd->lookAhead.dist + cmd->lookAhead.ve * cmd->lookAhead.ve);

        if (CP_TrackFlag == 0) {
            if (cmd->lookAhead.juctionAngle == 0) {
                cmd->lookAhead.va = vs;
            } else {
                double angle = asin(cmd->lookAhead.juctionAngle);
                double sResult = sin(angle / 2);
                cmd->lookAhead.va = (float)(0.5f * cmd->lookAhead.AT / sResult);
            }
        } else {
            cmd->lookAhead.va = 100000;
        }

        if (cmd->lookAhead.vd > cmd->lookAhead.va) {
            if (cmd->lookAhead.va > vs) {
                cmd->lookAhead.vc = vs;
            } else {
                cmd->lookAhead.vc = cmd->lookAhead.va;
            }
        } else {
            if (cmd->lookAhead.vd > vs) {
                cmd->lookAhead.vc = vs;
            } else {
                cmd->lookAhead.vc = cmd->lookAhead.vd;
            }
        }

        CPCmd *cmd1 = (CPCmd *)*(uint32_t *)RingBufferDataAt(&gQueuedCmdCPPPRBHandler, i - 1);
        cmd1->lookAhead.ve = cmd->lookAhead.vc;
    }

    CPCmd *cmdOut = (CPCmd *)*(uint32_t *)RingBufferDataAt(&gQueuedCmdCPPPRBHandler, 0);

    if (CP_TrackFlag == 0) {
        cmdOut->lookAhead.maxVel  = gSysParams.cp.params.juncitionVel;
    } else {
        cmdOut->lookAhead.maxVel = 1000*sqrt(cmdOut->x*cmdOut->x + cmdOut->y*cmdOut->y + cmdOut->z*cmdOut->z) / gSysParams.cp.params.acc;
    }

    cmdOut->lookAhead.startVel = cmdOut->lookAhead.vc;
    cmdOut->lookAhead.endVel = cmdOut->lookAhead.ve;
    return 0;
}

/*********************************************************************************************************
** Function name:       CPMotion
** Descriptions:
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/
void CPMotion(float vi, float ve, float vm, float dx, float dy,float dz,float isLaserOn,float state, uint8_t cpMode)
{
    TRobotTheta theta;
    int32_t calInvErr;

    if (finishFlag) {
        return;
    }

    if (!planFlag) {

        gOrigPos.x = gSysParams.pose.x;
        gOrigPos.y = gSysParams.pose.y;
        gOrigPos.z = gSysParams.pose.z;

        if (CP_TrackFlag == 0) {
            seg.accelat = gLookAheadParams.planAcc;
            seg.interp_time.AT = gLookAheadParams.planAcc * periodTime;
        } else {
            seg.accelat = gLookAheadParams.planAcc;
            seg.interp_time.AT = gLookAheadParams.planAcc * periodTime;
        }

        seg.gCode.Z = dz;

        seg.gCode.X = dx;
        seg.gCode.Y = dy;

        seg.dist = (float)sqrt(dx*dx+dy*dy+dz*dz);

        seg.interp_time.vel = vm;
        seg.ve = ve;
        seg.vb = vbg;

        seg.axis_seg[0] = 0;
        seg.axis_seg[1] = 0;
        seg.axis_seg[2] = 0;

        line_interp_plan(&seg);
        planFlag = true;
    }

    line_interp_deal(&seg);
    calInvErr = InverseCal(&gDistPos,0, &theta);

    if (0 != calInvErr) {
        if (ERR_PLAN_INV_CALC == calInvErr) {
            AlarmSysSetBit(ERR_MOVE_INV_CALC,true);
        } else if (ERR_PLAN_INV_LIMIT == calInvErr) {
            AlarmSysSetBit(ERR_MOVE_INV_LIMIT,true);
        }

        finishFlag = true;
        planFlag = false;
        ClearSpeed();
        return;
    }

    gJointMove.speed[0] = (theta.theta[0] - gSysParams.pose.jointAngle[0])/periodTime;
    gJointMove.speed[1] = (theta.theta[1] - gSysParams.pose.jointAngle[1])/periodTime;
    gJointMove.speed[2] = (theta.theta[2] - gSysParams.pose.jointAngle[2])/periodTime;
    //gJointMove.speed[3] = (theta.theta[3] - gSysParams.pose.jointAngle[3])/periodTime;
    gJointMove.speed[3] = 0;
}
