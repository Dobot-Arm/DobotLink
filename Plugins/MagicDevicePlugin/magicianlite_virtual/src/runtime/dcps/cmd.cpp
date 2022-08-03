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

#include "dcps.h"
#include "algorithm/rtKinematicCalc.h"

#include <log/log.h>
#include <algorithm/type.h>
#include <algorithm/alarms.h>
#include <algorithm/variable.h>
#include "cmd_id.h"
#include "mm/ParaComm.h"
#include "algorithm/alarms.h"
#include "algorithm/playBack.h"

#include <QDateTime>
#include <math.h>

const uint32_t QUEUE_SIZE = 50;
bool gQueuedCmdIsFinished[QueuedCmdTypeNum] = { true, true, true, true };

uint32_t QueuedCmdTypeCalc(uint32_t protocolID)
{
    uint32_t queuedCmdType;
    switch(protocolID) {
        case CmdID::CMD_ID_HOME:
        case CmdID::CMD_ID_GET_JOG_CMD:
        case CmdID::CMD_ID_PTP:
        case 91:
        case 92:
        case 95:
        case 101:
            queuedCmdType = QueuedCmdMotionType;
            break;
        case CMD_ID_SET_WAIT_CMD:
            queuedCmdType = QueuedCmdWaitType;
            break;
        case 120:
            queuedCmdType = QueuedCmdTrigType;
            break;
        default:
            queuedCmdType = QueuedCmdIOType;
            break;
    }
    return queuedCmdType;
}

bool QueuedCmdIsFinished(SysParams *sysParams)
{
    return gQueuedCmdIsFinished[sysParams->queuedCmdType];
}

void QueuedCmdFinishFlagClear(SysParams *sysParams)
{
    gQueuedCmdIsFinished[sysParams->queuedCmdType] = false;
}

void QueuedCmdFinishFlagRefresh(SysParams *sysParams)
{
    switch(sysParams->queuedCmdType) {
        case QueuedCmdMotionType:
            // finishFlag is refreshed by algorithm
            gQueuedCmdIsFinished[sysParams->queuedCmdType] = finishFlag;
            break;
        case QueuedCmdWaitType:
            // Refresh the timeout flag
            {
                quint32 nowTime = QDateTime::currentMSecsSinceEpoch()&0x7FFFFFFF; //可以支撑24天
                if(nowTime - sysParams->wait.initialTick > sysParams->wait.cmd.timeout) {
                    gQueuedCmdIsFinished[sysParams->queuedCmdType] = true;
                }
            }
            break;
        case QueuedCmdIOType:
            gQueuedCmdIsFinished[sysParams->queuedCmdType] = true;
            break;
        case QueuedCmdTrigType:
//        if (sysParams->trig.cmd.mode == TRIGInputIOMode) {
//            uint8_t IOValue =  GetGPIODIValue(EioAddress[sysParams->trig.cmd.address]);
//            if (((sysParams->trig.cmd.condition == TRIGInputIOEqual) && (IOValue == sysParams->trig.cmd.threshold))
//                ||((sysParams->trig.cmd.condition == TRIGInputIONotEqual) && (IOValue != sysParams->trig.cmd.threshold))) {
//                gQueuedCmdIsFinished[sysParams->queuedCmdType] = true;
//            }
//        } else if (sysParams->trig.cmd.mode == TRIGADCMode) {
//            uint16_t AdcValue = GetAdcValue(EioAddress[sysParams->trig.cmd.address]);
//            if (((sysParams->trig.cmd.condition == TRIGADCLT) && (AdcValue < sysParams->trig.cmd.threshold))
//                ||((sysParams->trig.cmd.condition == TRIGADCLE) && (AdcValue <= sysParams->trig.cmd.threshold))
//                ||((sysParams->trig.cmd.condition == TRIGADCGE) && (AdcValue > sysParams->trig.cmd.threshold))
//                ||((sysParams->trig.cmd.condition == TRIGADCGT) && (AdcValue >= sysParams->trig.cmd.threshold))) {
//                gQueuedCmdIsFinished[sysParams->queuedCmdType] = true;
//            }
//        }
            break;
        default:
            break;
    }
}


class GetProductName : public IDobotCmd
{
public:
    explicit GetProductName() noexcept : IDobotCmd(CMD_ID_GET_PRODUCT_NAME)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        const char* product_name = "MagicianLite";

        DOBOT_DEBUG("GetProductName : %s", product_name);
        response->setPayload(product_name, (uint16_t)strlen(product_name) + 1);
        return true;
    }
} cmd_get_product_name;

class GetPoseCmd : public IDobotCmd
{
public:
    explicit GetPoseCmd() noexcept : IDobotCmd(CMD_ID_GET_POSE)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("get pose");
        response->setPayload(&gSysParams.pose, sizeof(gSysParams.pose));
        return true;
    }
} cmd_get_pose;

class CheckPoseLimitCmd : public IDobotCmd
{
public:
    explicit CheckPoseLimitCmd() noexcept : IDobotCmd(CMD_ID_POS_CHECK_LIMIT)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        uint32_t error = 0;

        DOBOT_DEBUG("CheckPoseLimitCmd");

        if (request->isWrite() && request->length() == 17)
        {
            uint8_t type = *(uint8_t*)request->payload();
            if (type == 0)
            { /* 坐标点位限制 */
                TCPosition pose;
                TRobotTheta theta;
                memcpy((uint8_t*)&pose, (uint8_t*)request->payload() + 1, sizeof(TCPosition));
                uint8_t result = InverseCal(&pose, 0, &theta);
                response->setPayload(&result, sizeof(result));

                DOBOT_DEBUG("coordinate limit check %d", result);
            }
            else if (type == 1)
            { /* 关节点位限制 */
                TRobotTheta theta;
                memcpy((uint8_t*)&theta, (uint8_t*)request->payload() + 1, sizeof(TRobotTheta));
                uint8_t result = GetJointLimit(&theta);
                response->setPayload(&result, sizeof(result));
                DOBOT_DEBUG("joint limit check %d", result);
            }
        }

        return true;
    }
} cmd_check_pose_limit;

class GetAlarmStatusCmd : public IDobotCmd
{
public:
    explicit GetAlarmStatusCmd() noexcept : IDobotCmd(CMD_ID_ALARM_STATUS)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if  (request->isWrite())
        {
            DOBOT_DEBUG("ClearAlarmStatusCmd");
            AlarmSysClearAll();
        }
        else
        {
            DOBOT_DEBUG("GetAlarmStatusCmd");

            uint32_t len = 0;
            uint8_t* alarmsState;
            AlarmSysGetAll(&alarmsState, &len);
            response->setPayload(alarmsState, len);
        }
        return true;
    }
} cmd_get_alarm_status;

class SetHomeParamCmd : public IDobotCmd
{
public:
    explicit SetHomeParamCmd() noexcept : IDobotCmd(CMD_ID_HOME_PARAM)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("SetHomeParamCmd");

        TRobotTheta gHomeTheta;
        TCPosition  gHomePos;
        if  (request->isWrite())
        {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            memcpy(&gHomePos, protocolParams, sizeof(gHomePos));
            int CalErr = InverseCal(&gHomePos, 0, &gHomeTheta);
    //        if(CalErr == 0)
            memcpy(&gHomePoint, &gHomeTheta, sizeof(gHomeTheta));
        }
        else
        {
            memcpy(&gHomeTheta, &gHomePoint, sizeof(gHomeTheta));
            ForwardCal(&gHomeTheta, &gHomePos);
            response->setPayload(&gHomePos, sizeof(gHomePos));
        }
        return true;
    }
} cmd_set_home_param;

class HomeCmd : public IDobotCmd
{
public:
    explicit HomeCmd() noexcept : IDobotCmd(CMD_ID_HOME)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("HomeCmd");
        gRobotPrfMode = ROBOT_MODE_HOME;
        gHome[0].homeState = SEEK_HOME;
        finishFlag = false;
        uint64_t current_index = request->msg_id;//slave_->getQueueIndex() - 1;
        response->setPayload(&current_index, sizeof(current_index));
        DOBOT_DEBUG("home cmd index : %ld", current_index);

        return true;
    }
} cmd_home;

class GetEndEffectorParams : public IDobotCmd
{
public:
    explicit GetEndEffectorParams() noexcept : IDobotCmd(CMD_ID_END_EFFECTOR_PARAM)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("GetEndEffectorType");

        SysParams *sysParams = &gSysParams;
        if(request->isWrite())
        {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());

            memcpy(&sysParams->endEffectorParams, &protocolParams[0], sizeof(EndEffectorParams));
            gRobotPrfMode = ROBOT_MODE_NONE;
            gDobotMechPara.linkBlockH = sysParams->endEffectorParams.xBias;
            gDobotMechPara.linkBlockY = sysParams->endEffectorParams.yBias;
            gDobotMechPara.linkBlockV = sysParams->endEffectorParams.zBias;
        }
        else
        {
            response->setPayload(&sysParams->endEffectorParams, sizeof(EndEffectorParams));
        }
        return true;
    }
} cmd_get_end_effector_params;

class GetEndEffectorType : public IDobotCmd
{
public:
    explicit GetEndEffectorType() noexcept : IDobotCmd(CMD_ID_END_EFFECTOR_TYPE)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("GetEndEffectorType");
        if(request->isWrite())
        {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());

            gEffectorType = protocolParams[0];
            if(gEffectorType == SUCTIONCUP){
                gDobotMechPara.linkBlockH = gSuctioncup_LinkBlockH;
            }else if(gEffectorType == GRIPPER){
                gDobotMechPara.linkBlockH = gGripper_LinkBlockH;
            }else if(gEffectorType == PEN){
                gDobotMechPara.linkBlockH = gPen_LinkBlockH;
            }
        }
        else
        {
            uint8_t data[] = {gEffectorType};
            response->setPayload(data, sizeof(data)/sizeof(data[0]));
        }
        return true;
    }
} cmd_get_end_effector_type;

class SetRobotVelRatio : public IDobotCmd
{
private:
    enum Type
    {
        JOG = 0,
        OTHERS
    };

    struct Param
    {
        uint8_t type;
        uint8_t value;

        Param() : type(0), value(0)
        {
        }
    };

#define MAX_VAL(a, b) ((a) > (b) ? (a) : (b))
#define MIN_VAL(a, b) ((a) < (b) ? (a) : (b))

public:
    explicit SetRobotVelRatio() noexcept : IDobotCmd(CMD_ID_ROBOT_VEL_RATIO)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        static uint8_t jogRatio=100,otherRotio=100;
        static uint8_t opMode = 0;
        PTPJointParams *ptpjointParams = &gSysParams.ptp.params.jointParams;
        PTPCoordinateParams *ptpcoordinateParams = &gSysParams.ptp.params.coordinateParams;
        JOGJointParams *jogParams = &gSysParams.jog.params.jointParams;
        JOGCoordinateParams *jogcoordinateParams = &gSysParams.jog.params.coordinateParams;
        CPParams *params = &gSysParams.cp.params;
        ARCParams *arcParams = &gSysParams.arc.params;
        if(request->isWrite()) {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            opMode = protocolParams[0];
            if(protocolParams[1] == 0){
                AlarmSysSetBit(ERR_OTHER_INVALIDPARAM, true);
                return false;
            }else{
                AlarmSysSetBit(ERR_OTHER_INVALIDPARAM, false);
                if(opMode == 0){
                    jogRatio = protocolParams[1];
                    if(jogRatio>100){
                        gJogRobotRatio = 1.0;
                    } else {
                        gJogRobotRatio = jogRatio / 100.0f;
                    }
                }else{
                    otherRotio = protocolParams[1];
                    if(otherRotio>100){
                        gRobotRatio = 1.0;
                    } else {
                        gRobotRatio = otherRotio / 100.0f;
                    }
                }
                for(uint8_t i = 0; i < AXIS_NUM; i++) {
                    if(opMode == 0){             /*Jog*/
                        jogParams->velocity[i] = gJogVelocityLimit[i] * gJogRobotRatio;
                        jogParams->acceleration[i] = gJogAccelrationLimit[i] * gJogRobotRatio;
                        jogcoordinateParams->velocity[i] = gJogVelocityLimit[i] * gJogRobotRatio;
                        jogcoordinateParams->acceleration[i] = gJogAccelrationLimit[i] * gJogRobotRatio;
                    } else if(opMode == 1){
                        /*PTP*/
                        ptpjointParams->velocity[i] = gVelocityLimit[i] * gRobotRatio;
                        ptpjointParams->acceleration[i] = gAccelrationLimit[i] * gRobotRatio;
                        ptpcoordinateParams->xyzVelocity = gxyzVelocityLimit * gRobotRatio;
                        ptpcoordinateParams->xyzAcceleration = gxyzAccelerationLimit * gRobotRatio;
                        ptpcoordinateParams->rVelocity = grVelocityLimit * gRobotRatio;
                        ptpcoordinateParams->rAcceleration = grAccelerationLimit * gRobotRatio;
                        /*CP*/
                        params->planAcc = planAccLimit * gRobotRatio;
                        params->juncitionVel = juncitionVelLimit * gRobotRatio;
                        params->acc = planAccLimit * gRobotRatio;
                         /*ARC*/
                        arcParams->xyzAcceleration = gArcxyzAccelerationLimit * gRobotRatio;
                        arcParams->rAcceleration = gArcrAccelerationLimit * gRobotRatio;
                        arcParams->xyzVelocity = gArcxyzVelocityLimit * gRobotRatio;
                        arcParams->rVelocity  = gArcrVelocityLimit  * gRobotRatio;
                    }
                }
            }
        } else {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            opMode = protocolParams[0];

            uint8_t value[2];
            value[0] = opMode;
            if(opMode == 0){
                value[1] = jogRatio;
            }else{
                value[1] = otherRotio;
            }
            response->setPayload(value, 2);
        }

        return true;
    }
} cmd_set_robot_vel_ratio;

class GetCfgAndStatus : public IDobotCmd
{
public:
    explicit GetCfgAndStatus() noexcept : IDobotCmd(CMD_ID_ROBOT_CFG_AND_STATUS)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if (request->isWrite())
        {
        }
        else
        {
            uint8_t result[3]={gEffectorType,0,0};
            int length = 0;
            if(gEffectorType == 1){    /*吸盘*/
                result[1] = gEffectorEnale[0];
                result[2] = gEffectorStatus[0];
                length = 3;
            } else if(gEffectorType == 2){    /*手爪*/
                result[1] = gEffectorEnale[1];
                result[2] = gEffectorStatus[1];
                length = 3;
            }else{
                length = 1;
            }
            response->setPayload(result, length);
        }
        return true;
    }
} cmd_get_cfg_and_status;

class GetRobotRTStatus : public IDobotCmd
{
public:
    explicit GetRobotRTStatus() noexcept : IDobotCmd(CMD_ID_ROBOT_RT_STATUS)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if (request->isWrite())
        {
        }
        else
        {
            uint8_t *alarmsState;
            uint32_t len = 0;
            uint64_t currentIndex = slave_->getQueueIndex();;
            AlarmSysGetAll(&alarmsState, &len);
            if(QueuedCmdIsFinished(&gSysParams) == false) {
                currentIndex -= 1;
            }

            uint8_t protocolParams[256];
            memcpy(protocolParams, (void *)&gSysParams.pose, sizeof(Pose));
            memcpy(protocolParams+32, (void *)alarmsState, len);
            memcpy(protocolParams+48, &currentIndex, sizeof(uint64_t));
            uint32_t length = sizeof(Pose)+ len + sizeof(uint64_t);

            response->setPayload(protocolParams, length);
        }
        return true;
    }
} cmd_get_robot_rt_status;

class GetJogJointParams : public IDobotCmd
{
public:
    explicit GetJogJointParams() noexcept : IDobotCmd(CMD_ID_GET_JOG_JOINT_PARAMS)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        JOGJointParams *jointParams = &gSysParams.jog.params.jointParams;
        if(request->isWrite()) {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            memcpy(jointParams, &protocolParams[0], sizeof(JOGJointParams));
            for(uint8_t i = 0; i < AXIS_NUM; i++) {
                if(jointParams->velocity[i] > gVelocityLimit[i]) {
                    jointParams->velocity[i] = gVelocityLimit[i];
                }
                if(jointParams->acceleration[i] > gAccelrationLimit[i]) {
                    jointParams->acceleration[i] = gAccelrationLimit[i];
                }
            }
        } else {
            response->setPayload(jointParams, sizeof(JOGJointParams));
        }

        return true;
    }
} cmd_get_joint_param;

class GetJogCoordinateParams : public IDobotCmd
{
public:
    explicit GetJogCoordinateParams() noexcept : IDobotCmd(CMD_ID_GET_JOG_COORDINATE_PARAMS)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        JOGCoordinateParams *coordinateParams = &gSysParams.jog.params.coordinateParams;
        if(request->isWrite()) {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            memcpy(coordinateParams, &protocolParams[0], sizeof(JOGCoordinateParams));
            for(uint8_t i = 0; i < AXIS_NUM; i++) {
                if(coordinateParams->velocity[i] > gVelocityLimit[i]) {
                    coordinateParams->velocity[i] = gVelocityLimit[i];
                }
                if(coordinateParams->acceleration[i] > gAccelrationLimit[i]) {
                    coordinateParams->acceleration[i] = gAccelrationLimit[i];
                }
            }
        } else {
            response->setPayload(coordinateParams, sizeof(JOGCoordinateParams));
        }

        return true;
    }
} cmd_get_jog_coordinate_param;

class SetPTPJointParam : public IDobotCmd
{
public:
    explicit SetPTPJointParam() noexcept : IDobotCmd(CMD_ID_PTP_JOINT_PARAMS)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("SetPTPCoordinateParam");

        PTPJointParams* jointParams = &gSysParams.ptp.params.jointParams;
        if(request->isWrite()) {
            memcpy(jointParams, request->payload(), sizeof(PTPJointParams));
            for(uint8_t i = 0; i < AXIS_NUM; i++) {
                if(jointParams->velocity[i] > gVelocityLimit[i]) {
                    jointParams->velocity[i] = gVelocityLimit[i];
                }
                if(jointParams->acceleration[i] > gAccelrationLimit[i]) {
                    jointParams->acceleration[i] = gAccelrationLimit[i];
                }
            }
        } else {
            response->setPayload(jointParams, sizeof(PTPJointParams));
        }
        return true;
    }
} cmd_set_ptp_joint_param;

class SetPTPCoordinateParam : public IDobotCmd
{
public:
    explicit SetPTPCoordinateParam() noexcept : IDobotCmd(CMD_ID_PTP_COORDINATE_PARAM)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("SetPTPCoordinateParam");

        PTPCoordinateParams* coordinateParams = &gSysParams.ptp.params.coordinateParams;
        if(request->isWrite()) {
            memcpy(coordinateParams, request->payload(), sizeof(PTPCoordinateParams));
            //vel
            if(coordinateParams->xyzVelocity > gxyzVelocityLimit) {
                coordinateParams->xyzVelocity = gxyzVelocityLimit;
            }
            if(coordinateParams->xyzAcceleration > gxyzAccelerationLimit) {
                coordinateParams->xyzAcceleration = gxyzAccelerationLimit;
            }
            if(coordinateParams->rVelocity > grVelocityLimit) {
                coordinateParams->rVelocity = grVelocityLimit;
            }
            if(coordinateParams->rAcceleration > grAccelerationLimit) {
                coordinateParams->rAcceleration = grAccelerationLimit;
            }
        } else {
            response->setPayload(coordinateParams, sizeof(PTPCoordinateParams));
        }
        return true;
    }
} cmd_set_ptp_coordinate_param;

class SetPTPJumpParams : public IDobotCmd
{
public:
    explicit SetPTPJumpParams() noexcept : IDobotCmd(CMD_ID_PTP_JUMP_PARAMS)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("SetPTPJumpParams");

        PTPJumpParams* jumpParams = &gSysParams.ptp.params.jumpParams;
        PTPJump2Params *jump2Params = &gSysParams.ptp.params.jump2Params;
        if(request->isWrite()) {
            memcpy(jumpParams, request->payload(), sizeof(PTPJumpParams));
            jump2Params->startJumpHeight = jumpParams->jumpHeight;
            jump2Params->endJumpHeight = jumpParams->jumpHeight;
            jump2Params->zLimit = jumpParams->maxJumpHeight;//jumpParams->zLimit;
        } else {
            response->setPayload(jumpParams, sizeof(PTPJumpParams));
        }

        return true;
    }
} cmd_set_ptp_jump_params;

class SetPTPCommonParams : public IDobotCmd
{
public:
    explicit SetPTPCommonParams() noexcept : IDobotCmd(CMD_ID_PTP_COMMON_PARAMS)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("SetPTPCommonParams");

        PTPCommonParams* commonParams = &gSysParams.ptp.params.commonParams;
        if(request->isWrite()) {
            memcpy(commonParams, request->payload(), sizeof(PTPCommonParams));
            if(commonParams->velocityRatio > 100) {
                velRatio = 1.0f;
            } else {
                velRatio = commonParams->velocityRatio / 100.0f;
            }
            if(commonParams->accelerationRatio > 100) {
                accRatio = 1.0f;
            } else {
                accRatio = commonParams->accelerationRatio / 100.0f;
            }
        } else {
            response->setPayload(commonParams, sizeof(PTPCommonParams));
        }
        return true;
    }
} cmd_set_ptp_common_params;

class StartPTPCmd : public IDobotCmd
{
public:
    explicit StartPTPCmd() noexcept : IDobotCmd(CMD_ID_PTP)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("StartPTPCmd");

        PTPCmd* cmd = &gSysParams.ptp.cmd;
        memcpy(cmd, request->payload(), sizeof(PTPCmd));
        DOBOT_DEBUG("PTP mode : %d, %0.3f, %0.3f, %0.3f, %0.3f", cmd->ptpMode, cmd->x, cmd->y, cmd->z, cmd->rHead);
        gRobotPrfMode = ROBOT_MODE_PLAYBACK;
        finishFlag = false;
        return true;
    }
} cmd_start_ptp_cmd;

class JOGCommonParamsCmd : public IDobotCmd
{
public:
    explicit JOGCommonParamsCmd() noexcept : IDobotCmd(CMD_ID_GET_JOG_COMMON_PARAMS)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if (response->isWrite())
        {
            DOBOT_DEBUG("SetJOGCommonParamsCmd");
            gSysParams.jog.params.commonParams = *reinterpret_cast<JOGCommonParams*>(request->payload());
            singleVelRatio = gSysParams.jog.params.commonParams.velocityRatio / 100.0f;
        }
        else
        {
            DOBOT_DEBUG("GetJOGCommonParamsCmd");
            response->setPayload(&gSysParams.jog.params.commonParams, sizeof(gSysParams.jog.params.commonParams));
        }
        return true;
    }
} cmd_get_jog_common_params;

class JOGCmdSet : public IDobotCmd
{
public:
    explicit JOGCmdSet() noexcept : IDobotCmd(CMD_ID_GET_JOG_CMD)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("JOGCmdSet");
        auto cmd = reinterpret_cast<JOGCmd*>(request->payload());
        gSysParams.jog.cmd = *cmd;
        gRobotPrfMode = cmd->isJoint ? ROBOT_MODE_SINGLE : ROBOT_MODE_SINGLEXYZ;
        gSingleMode = cmd->cmd;
        DOBOT_DEBUG("jog type : %d, value : %d", cmd->isJoint, cmd->cmd);
        return true;
    }
} cmd_jog_set;

class CollisionCheckCmd : public IDobotCmd
{
public:
    explicit CollisionCheckCmd() noexcept : IDobotCmd(CMD_ID_COLLISION_CHECK)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("CollisionCheckCmd");
        if(request->isWrite()){
            uint8_t *protocolParams = reinterpret_cast<uint8_t *>(request->payload());
            gCollisionDetectFlag = protocolParams[0];
            memcpy(&gSysParams.CollisionCheckValue, (void*)&protocolParams[1], sizeof(gSysParams.CollisionCheckValue));

            if(gSysParams.CollisionCheckValue < 8.0){
                gSysParams.CollisionCheckValue = 8.0;
            }
        }else{
            uint8_t param[5];
            param[0] = gCollisionDetectFlag;
            memcpy(param+1, &gSysParams.CollisionCheckValue, 4);
            response->setPayload(param, 5);
        }
        return true;
    }
} cmd_collision_check;

class StartQueueCmd : public IDobotCmd
{
public:
    explicit StartQueueCmd() noexcept : IDobotCmd(CMD_ID_START_QUEUE)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("StartQueueCmd");
        gSysParams.runQueuedCmd = true;
        finishFlag = true;
        planFlag = false;
        jumPlanFlag = false;
        return true;
    }
} cmd_start_queue;

class StopQueueCmd : public IDobotCmd
{
public:
    explicit StopQueueCmd() noexcept : IDobotCmd(CMD_ID_QUEUE_STOP)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        gSysParams.runQueuedCmd = false;
        if (gSysParams.queuedCmdType == QueuedCmdMotionType)
            stopFinishFlag = true;
        else
            gQueuedCmdIsFinished[gSysParams.queuedCmdType] = true;
        return true;
    }
} cmd_stop_queue;

class ForceStopQueueCmd : public IDobotCmd
{
public:
    explicit ForceStopQueueCmd() noexcept : IDobotCmd(CMD_ID_QUEUE_FORCE_STOP)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        gSysParams.runQueuedCmd = false;

        finishFlag = true;
        planFlag = false;
        jumPlanFlag = false;
        stopPlanFlag = false;
        gRobotPrfMode = ROBOT_MODE_STOP;

        for (bool& is_finished : gQueuedCmdIsFinished)
        {
            is_finished = true;
        }

        return true;
    }
} cmd_force_stop_queue;

class StartDownloadQueueCmd : public IDobotCmd
{
public:
    explicit StartDownloadQueueCmd() noexcept : IDobotCmd(CMD_ID_QUEUE_START_DOWNLOAD)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if (!gSysParams.runOfflineQueuedCmd)
        {
            gSysParams.queuedCmdOfflineLoopLine.isDownloading = true;
        }

        memcpy(&gSysParams.queuedCmdOfflineLoopLine.totalLoop, request->payload(), sizeof(uint32_t));
        memcpy(&gSysParams.queuedCmdOfflineLoopLine.linePerLoop, (char*)request->payload() + 4, sizeof(uint32_t));
        gSysParams.queuedCmdOfflineLoopLine.writeFlag = true;
        gSysParams.queuedCmdOfflineLoopLine.readFlag = true;
        return true;
    }
} cmd_start_download_queue;

class StopDownloadQueueCmd : public IDobotCmd
{
public:
    explicit StopDownloadQueueCmd() noexcept : IDobotCmd(CMD_ID_QUEUE_STOP_DOWNLOAD)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        gSysParams.queuedCmdOfflineLoopLine.isDownloading = false;
        return true;
    }
} cmd_stop_download_queue;

class ClearQueueCmd : public IDobotCmd
{
public:
    explicit ClearQueueCmd() noexcept : IDobotCmd(CMD_ID_QUEUE_CLEAR)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        finishFlag = true;
        slave_->clearQueue();
        return true;
    }
} cmd_clear_queue;

class GetQueueIndexCmd : public IDobotCmd
{
public:
    explicit GetQueueIndexCmd() noexcept : IDobotCmd(CMD_ID_QUEUE_GET_INDEX)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        uint64_t index = slave_->getQueueIndex();
        response->setPayload(&index, sizeof(index));
        DOBOT_DEBUG("current index : %ld", index);
        return true;
    }
} cmd_get_queue_index;

class GetQueueLeftSpaceCmd : public IDobotCmd
{
public:
    explicit GetQueueLeftSpaceCmd() noexcept : IDobotCmd(CMD_ID_QUEUE_GET_LEFT_SPACE)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        uint64_t left = slave_->getLeftSpace();
        response->setPayload(&left, sizeof(left));
        return true;
    }
} cmd_get_queue_left_space;

class SetEndEffectorSuctionCupCmd : public IDobotCmd
{
private:

public:
    explicit SetEndEffectorSuctionCupCmd() noexcept : IDobotCmd(CMD_ID_END_EFFECTOR_SUCTION_CUP)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("SetEndEffectorSuctionCupCmd");

        static uint8_t enableCtrl;
        static uint8_t suck;
        if (request->isWrite())
        {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());

            gEffectorEnale[0] = 0;
            gEffectorStatus[0] = 0;
            if((protocolParams[0]!=0) && (protocolParams[0]!=1)){
                return true;
            }
            enableCtrl = protocolParams[0];
            suck = protocolParams[1];

            gEffectorEnale[0] = enableCtrl;
            gEffectorStatus[0] = suck;
        }
        else
        {
            uint8_t data[] = {enableCtrl,suck};
            response->setPayload(data, sizeof(data)/sizeof(data[0]));
        }
        return true;
    }
} cmd_set_end_effector_suction_cup;

class SetEndEffectorGripperCmd : public IDobotCmd
{
private:

public:
    explicit SetEndEffectorGripperCmd() noexcept : IDobotCmd(CMD_ID_END_EFFECTOR_GRIPPER)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("SetEndEffectorGripperCmd");

        static uint8_t enableCtrl;
        static uint8_t suck;
        if (request->isWrite())
        {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());

            if((protocolParams[0]!=0) && (protocolParams[0]!=1)){
                return 0;
            }
            gEffectorEnale[1] = 0;
            gEffectorStatus[1] = 0;
            enableCtrl = protocolParams[0];
            suck = protocolParams[1];

            gEffectorEnale[1] = enableCtrl;
            gEffectorStatus[1] = suck;
        }
        else
        {
            uint8_t data[] = {enableCtrl,suck};
            response->setPayload(data, sizeof(data)/sizeof(data[0]));
        }
        return true;
    }
} cmd_set_end_effector_gripper;

class SetWAITCmdCmd : public IDobotCmd
{
private:

public:
    explicit SetWAITCmdCmd() noexcept : IDobotCmd(CMD_ID_SET_WAIT_CMD)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("SetWAITCmdCmd");

        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());

        quint32 nowTime = QDateTime::currentMSecsSinceEpoch()&0x7FFFFFFF; //可以支撑24天
        gSysParams.wait.initialTick = nowTime;
        memcpy(&gSysParams.wait.cmd, &protocolParams[0], sizeof(WAITCmd));
        return true;
    }
} cmd_set_wait_cmd;

class SetLostStepValueCmd : public IDobotCmd
{
private:

public:
    explicit SetLostStepValueCmd() noexcept : IDobotCmd(CMD_ID_SET_LOST_STEP_VALUE)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("SetLostStepValueCmd");

        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
        if(request->isWrite())
        {
            memcpy(&gSysParams.LostStepValue, protocolParams, sizeof(gSysParams.LostStepValue));
        }
        else
        {
            response->setPayload(&gSysParams.LostStepValue, sizeof(gSysParams.LostStepValue));
        }

        return true;
    }
} cmd_set_lost_step_value;

class SetLostStepCmdCmd : public IDobotCmd
{
private:

public:
    explicit SetLostStepCmdCmd() noexcept : IDobotCmd(CMD_ID_SET_LOST_STEP_CMD)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("SetLostStepCmdCmd");

        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());

        /*
         * 无实际硬件位置，不予处理，模拟数据都是理论值，不存在误差，也就不存在丢步现象
         * */
        return true;
    }
} cmd_set_lost_step_cmd;

class ProtocolResetPoseCmd : public IDobotCmd
{
public:
    explicit ProtocolResetPoseCmd() noexcept : IDobotCmd(ProtocolResetPose)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
        uint8_t manual = protocolParams[0];
        float rearArmAngle, frontArmAngle;
        memcpy(&rearArmAngle, &protocolParams[1], sizeof(float));
        memcpy(&frontArmAngle, &protocolParams[1 + sizeof(float)], sizeof(float));
        if(manual) {
            gSysParams.pose.jointAngle[0] = 0;
            gSysParams.pose.jointAngle[1] = rearArmAngle;
            gSysParams.pose.jointAngle[2] = frontArmAngle;
    //        printf("Manually reset pose!\r\n");
        } else {
            //MC_PresentPoseUpdate();
    //        printf("Auto reset pose!\r\n");
        }
        return true;
    }
} ProtocolResetPose_cmd;

class ProtocolGetKinematicsCmd : public IDobotCmd
{
public:
    explicit ProtocolGetKinematicsCmd() noexcept : IDobotCmd(ProtocolGetKinematics)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        response->setPayload(&gSysParams.kinematics,sizeof(Kinematics));
        return true;
    }
} ProtocolGetKinematics_cmd;

class ProtocolUpdatePoseCmd : public IDobotCmd
{
public:
    explicit ProtocolUpdatePoseCmd() noexcept : IDobotCmd(ProtocolUpdatePose)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
            //MC_PresentPoseUpdate();
        } else {

        }
        return true;
    }
} ProtocolUpdatePose_cmd;

class ProtocolAutoLevelingCmd : public IDobotCmd
{
public:
    explicit ProtocolAutoLevelingCmd() noexcept : IDobotCmd(ProtocolAutoLeveling)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            CalibrationMode = protocolParams[0];
            if(CalibrationMode == 3) { //底座水平度校准
                //if(BaseLeveling_GetCalibrationStatus() == 1)
                {
                    //BaseLeveling_CalibrationStart();
                    gRobotPrfMode = ROBOT_MODE_LEVELING;
                }
            }
        } else {
            //todo:
        }
        finishFlag = false;
        return true;
    }
}ProtocolAutoLeveling_cmd;

class ProtocolArmOrientationCmd : public IDobotCmd
{
public:
    explicit ProtocolArmOrientationCmd() noexcept : IDobotCmd(ProtocolArmOrientation)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            gSysParams.armOrientation = (ArmOrientation)protocolParams[0];
        } else {
            response->setPayload(&gSysParams.armOrientation,1);
        }
        return true;
    }
}ProtocolArmOrientation_cmd;

class ProtocolStepCtrlModeCmd : public IDobotCmd
{
public:
    explicit ProtocolStepCtrlModeCmd() noexcept : IDobotCmd(ProtocolStepCtrlMode)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            gStepCtrlMode = protocolParams[0];
            if(gStepCtrlMode == 0) {     //开环模式
                //HAL_ModuleInit();
                //MC_GetInitialAngle();    //当前关节角度
            } else {
                //HAL_ModuleInit_Calibrate();
            }
        } else {
            response->setPayload(&gStepCtrlMode,1);
        }
        return true;
    }
}ProtocolStepCtrlMode_cmd;

class ProtocolCPParamsCmd : public IDobotCmd
{
public:
    explicit ProtocolCPParamsCmd() noexcept : IDobotCmd(ProtocolCPParams)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        CPParams *cpParams = &gSysParams.cp.params;
        if(request->isWrite()) {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            memcpy(cpParams, &protocolParams[0], sizeof(CPParams));
            gLookAheadParams = *cpParams;
            CP_TrackFlag = cpParams->realTimeTrack;
        } else {
            cpParams->realTimeTrack = CP_TrackFlag;
            response->setPayload(cpParams,sizeof(CPParams));
        }
        return true;
    }
}ProtocolCPParams_cmd;

class ProtocolCPCmdCmd : public IDobotCmd
{
public:
    explicit ProtocolCPCmdCmd() noexcept : IDobotCmd(0)//ProtocolCPCmd
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
        }
        return true;
    }
}ProtocolCPCmd_cmd;

class ProtocolCPLECmdCmd : public IDobotCmd
{
public:
    explicit ProtocolCPLECmdCmd() noexcept : IDobotCmd(0)//ProtocolCPLECmd
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
        }
        return true;
    }
}ProtocolCPLECmd_cmd;

class ProtocolCPRHeadHoldEnableCmd : public IDobotCmd
{
public:
    explicit ProtocolCPRHeadHoldEnableCmd() noexcept : IDobotCmd(ProtocolCPRHeadHoldEnable)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            gSysParams.cp.rHeadHoldEnable = protocolParams[0];
            gSysParams.cp.rHeadValue = gSysParams.pose.rHead;
        } else {
            response->setPayload(&(gSysParams.cp.rHeadHoldEnable),1);
        }
        return true;
    }
}ProtocolCPRHeadHoldEnable_cmd;

class ProtocolCPCommonParamsCmd : public IDobotCmd
{
public:
    explicit ProtocolCPCommonParamsCmd() noexcept : IDobotCmd(ProtocolCPCommonParams)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        float ratio[2];
        CPParams *params = &gSysParams.cp.params;
        if(request->isWrite()) {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            memcpy(ratio, &protocolParams[0], 2 * sizeof(float));
            if(ratio[1] > 100) {
                params->planAcc = params->planAcc * 1.0;
            } else {
                params->planAcc =  params->planAcc * ratio[1] / 100;
            }
            if(ratio[0] > 100) {
                params->juncitionVel = params->juncitionVel * 1.0;
            } else {
                params->juncitionVel = params->juncitionVel * ratio[0] / 100;
            }
        } else {
            response->setPayload(params,sizeof(CPParams));
        }
        return true;
    }
}ProtocolCPCommonParams_cmd;

class ProtocolCP2CmdCmd : public IDobotCmd
{
public:
    explicit ProtocolCP2CmdCmd() noexcept : IDobotCmd(0)//ProtocolCP2Cmd
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
        }
        return true;
    }
}ProtocolCP2Cmd_cmd;

class ProtocolARCParamsCmd : public IDobotCmd
{
public:
    explicit ProtocolARCParamsCmd() noexcept : IDobotCmd(ProtocolARCParams)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        ARCParams *arcParams = &gSysParams.arc.params;
        if(request->isWrite()) {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            memcpy(arcParams, &protocolParams[0], sizeof(ARCParams));
        } else {
            response->setPayload(arcParams,sizeof(ARCParams));
        }
        return true;
    }
}ProtocolARCParams_cmd;

class ProtocolARCCmdCmd : public IDobotCmd
{
public:
    explicit ProtocolARCCmdCmd() noexcept : IDobotCmd(ProtocolARCCmd)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        auto sysParams = &gSysParams;
        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
        memcpy(&sysParams->arc.cmd, &protocolParams[0], sizeof(ARCCmd));
        gRobotPrfMode = ROBOT_MODE_ARC;
        gCircleMove.midPos.x = sysParams->arc.cmd.cirPoint.x;
        gCircleMove.midPos.y = sysParams->arc.cmd.cirPoint.y;
        gCircleMove.midPos.z = sysParams->arc.cmd.cirPoint.z;
        gCircleMove.midPos.r = sysParams->arc.cmd.cirPoint.rHead;
        gCircleMove.endPos.x = sysParams->arc.cmd.toPoint.x;
        gCircleMove.endPos.y = sysParams->arc.cmd.toPoint.y;
        gCircleMove.endPos.z = sysParams->arc.cmd.toPoint.z;
        gCircleMove.endPos.r = sysParams->arc.cmd.toPoint.rHead;
        finishFlag = false;
        return true;
    }
}ProtocolARCCmd_cmd;

class ProtocolCircleCmdCmd : public IDobotCmd
{
public:
    explicit ProtocolCircleCmdCmd() noexcept : IDobotCmd(ProtocolCircleCmd)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        auto sysParams = &gSysParams;
        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());

        gWholeCircleFlag = 1;
        memcpy(&sysParams->circleCmd, &protocolParams[0], sizeof(CircleCmd));
        gRobotPrfMode = ROBOT_MODE_ARC;
        gCircleMove.midPos.x = sysParams->circleCmd.cirPoint.x;
        gCircleMove.midPos.y = sysParams->circleCmd.cirPoint.y;
        gCircleMove.midPos.z = sysParams->circleCmd.cirPoint.z;
        gCircleMove.midPos.r = sysParams->circleCmd.cirPoint.rHead;
        gCircleMove.endPos.x = sysParams->circleCmd.toPoint.x;
        gCircleMove.endPos.y = sysParams->circleCmd.toPoint.y;
        gCircleMove.endPos.z = sysParams->circleCmd.toPoint.z;
        gCircleMove.endPos.r = sysParams->circleCmd.toPoint.rHead;
        gCircleCount = sysParams->circleCmd.count;
        finishFlag = false;
        return true;
    }
}ProtocolCircleCmd_cmd;

class ProtocolARCCommonParamsCmd : public IDobotCmd
{
public:
    explicit ProtocolARCCommonParamsCmd() noexcept : IDobotCmd(ProtocolARCCommonParams)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        float ratio[2];
        auto sysParams = &gSysParams;
        ARCParams *arcParams = &sysParams->arc.params;

        if(request->isWrite()) {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            memcpy(ratio, &protocolParams[0], 2 * sizeof(float));
            if(ratio[1] > 100) {
                arcParams->xyzAcceleration = arcParams->xyzAcceleration * 1.0;
                arcParams->rAcceleration = arcParams->rAcceleration * 1.0;
            } else {
                arcParams->xyzAcceleration = arcParams->xyzAcceleration * ratio[1] / 100.0;
                arcParams->rAcceleration = arcParams->rAcceleration * ratio[1] / 100.0;
            }
            if(ratio[0] > 100) {
                arcParams->xyzVelocity = arcParams->xyzVelocity * 1.0;
                arcParams->rVelocity  = arcParams->rVelocity  * 1.0;
            } else {
                arcParams->xyzVelocity  = arcParams->xyzVelocity  * ratio[1] / 100.0;
                arcParams->rVelocity  = arcParams->rVelocity  * ratio[1] / 100.0;
            }
        } else {
            response->setPayload(arcParams,sizeof(ARCParams));
        }
        return true;
    }
}ProtocolARCCommonParams_cmd;

class ProtocolTRIGCmdCmd : public IDobotCmd
{
public:
    explicit ProtocolTRIGCmdCmd() noexcept : IDobotCmd(ProtocolTRIGCmd)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        auto sysParams = &gSysParams;
        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());

        sysParams->trig.cmd.address = protocolParams[0];
        sysParams->trig.cmd.mode = protocolParams[1];
        sysParams->trig.cmd.condition = protocolParams[2];
        memcpy(&sysParams->trig.cmd.threshold, &protocolParams[3], sizeof(uint16_t));
        if(sysParams->trig.cmd.mode == TRIGInputIOMode) {
            //ConfigGPIOFunction(EioAddress[sysParams->trig.cmd.address], IOFunctionDI);
        } else if(sysParams->trig.cmd.mode == TRIGADCMode) {
            //ConfigGPIOFunction(EioAddress[sysParams->trig.cmd.address], IOFunctionADC);
        }
        return true;
    }
}ProtocolTRIGCmd_cmd;

class ProtocolIOMultiplexingCmd : public IDobotCmd
{
public:
    explicit ProtocolIOMultiplexingCmd() noexcept : IDobotCmd(0)//ProtocolIOMultiplexing
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
        }
        return true;
    }
}ProtocolIOMultiplexing_cmd;

class ProtocolIODOCmd : public IDobotCmd
{
public:
    explicit ProtocolIODOCmd() noexcept : IDobotCmd(ProtocolIODO)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        static uint8_t DOIndex;
        static bool DOlevel[10];

        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
        if(request->isWrite()) {
            DOIndex = protocolParams[0];
            DOlevel[DOIndex] = (bool) protocolParams[1];
            //SetGPIODOValue(EioAddress[DOIndex], DOlevel[DOIndex]);
        } else {
            DOIndex = protocolParams[0];
            uint8_t tmp[] = {DOIndex, (uint8_t) DOlevel[DOIndex]};
            response->setPayload(tmp,2);
        }
        return true;
    }
}ProtocolIODO_cmd;

class ProtocolIOPWMCmd : public IDobotCmd
{
public:
    explicit ProtocolIOPWMCmd() noexcept : IDobotCmd(ProtocolIOPWM)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        static uint8_t IOPWMIndex;
        static float IOPWMfreq[20];
        static float IOPWMduty[20];

        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
        if(request->isWrite()) {
            gFrontEnd_PWM_Flag = 1;
            IOPWMIndex = protocolParams[0];
            IOPWMfreq[IOPWMIndex] = *((float *)(&protocolParams[1]));
            IOPWMduty[IOPWMIndex] = *((float *)(&protocolParams[5]));
    //        memcpy(&IOPWMfreq[IOPWMIndex], &protocolParams[1], sizeof(float));
    //        memcpy(&IOPWMduty[IOPWMIndex], (&protocolParams[1] + sizeof(float)), sizeof(float));
            //SetGPIOPWM(EioAddress[IOPWMIndex], IOPWMfreq[IOPWMIndex], IOPWMduty[IOPWMIndex]);
        } else {
            IOPWMIndex = protocolParams[0];
            uint8_t data[9];
            data[0] = IOPWMIndex;
            memcpy(&data[1], &IOPWMfreq[IOPWMIndex], sizeof(float));
            memcpy((&data[1] + sizeof(float)), &IOPWMduty[IOPWMIndex], sizeof(float));
            response->setPayload(data, 9);
        }
        return true;
    }
}ProtocolIOPWM_cmd;

class ProtocolIODICmd : public IDobotCmd
{
public:
    explicit ProtocolIODICmd() noexcept : IDobotCmd(ProtocolIODI)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
        static uint8_t DIIndex;
        static uint8_t DIlevel[20];
        DIIndex = protocolParams[0];
        DIlevel[DIIndex] = 0;//GetGPIODIValue(EioAddress[DIIndex]);

        uint8_t data[2]={DIIndex,DIlevel[DIIndex]};
        response->setPayload(data, 2);
        return true;
    }
}ProtocolIODI_cmd;

class ProtocolIOADCCmd : public IDobotCmd
{
public:
    explicit ProtocolIOADCCmd() noexcept : IDobotCmd(ProtocolIOADC)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        static uint8_t IOAdcIndex;
        static uint16_t AdcValue[20];

        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
        if(request->isWrite()) {
        } else {
            IOAdcIndex = protocolParams[0];
            AdcValue[IOAdcIndex] =  0;//GetAdcValue(EioAddress[IOAdcIndex]);

            uint8_t data[3]={IOAdcIndex,0,0};
            memcpy(&data[1], &AdcValue[IOAdcIndex], sizeof(uint16_t));
            response->setPayload(data, 3);
        }
        return true;
    }
}ProtocolIOADC_cmd;

class ProtocolIICSensorCmd : public IDobotCmd
{
public:
    explicit ProtocolIICSensorCmd() noexcept : IDobotCmd(ProtocolIICSensor)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
        uint8_t FuncID = protocolParams[0];
        uint8_t IIC_ID = protocolParams[1];
        uint8_t SensorType = protocolParams[2];
        uint16_t SensorVaule = 0;

        if(request->isWrite()) {
            if(FuncID == 0) {
                gFrontEnd_PWM_Flag = 1;
                //ConfigIICInit(IIC_ID);
            }
        } else {
            if(FuncID == 1) {
                uint8_t data[5]={FuncID,IIC_ID,SensorType};
                SensorVaule = 0;//EIO_GetIICSensorValue(IIC_ID, SensorType);
                memcpy((void *)&data[3], (void *)&SensorVaule, sizeof(uint16_t));
                response->setPayload(data, 5);
            }
        }
        return true;
    }
}ProtocolIICSensor_cmd;

class ProtocolAngleSensorStaticErrorCmd : public IDobotCmd
{
public:
    explicit ProtocolAngleSensorStaticErrorCmd() noexcept : IDobotCmd(ProtocolAngleSensorStaticError)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
        }
        return true;
    }
}ProtocolAngleSensorStaticError_cmd;

class ProtocolAngleSensorCoefCmd : public IDobotCmd
{
public:
    explicit ProtocolAngleSensorCoefCmd() noexcept : IDobotCmd(ProtocolAngleSensorCoef)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
        }
        return true;
    }
}ProtocolAngleSensorCoef_cmd;

class ProtocolRobotUserCoordinateCalCmd : public IDobotCmd
{
    typedef struct {
        float X;
        float Y;
        float Z;
    } Position;
public:
    explicit ProtocolRobotUserCoordinateCalCmd() noexcept : IDobotCmd(ProtocolRobotUserCoordinateCal)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        Position  ZeroPosition, XDirPosition, YDirPosition;
        WCSPosition UserCoordinate;
        uint8_t ReturnInfo = 0;

        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
        if(request->isWrite()) {
            /*memcpy((void *)&ZeroPosition, (void *)&protocolParams[0], sizeof(Position));
            memcpy((void *)&XDirPosition, (void *)&protocolParams[12], sizeof(Position));
            memcpy((void *)&YDirPosition, (void *)&protocolParams[24], sizeof(Position));
            if((((int)ZeroPosition.X == 0) && ((int)ZeroPosition.Y == 0) && ((int)ZeroPosition.Z == 0))
                    && (((int)XDirPosition.X == 0) && ((int)XDirPosition.Y == 0) && ((int)XDirPosition.Z == 0))
                    && (((int)YDirPosition.X == 0) && ((int)YDirPosition.Y == 0) && ((int)YDirPosition.Z == 0))) {
                ReturnInfo = EraseRobotUserCoord();
            } else {
                calUserCoord(&ZeroPosition, &XDirPosition, &YDirPosition, &UserCoordinate);
                ReturnInfo = StoreRobotUserCoord(UserCoordinate);
                if(ReturnInfo == 0) {
                    memcpy((void *)&gLocalParams, (void *)&UserCoordinate, sizeof(WCSPosition));
                }
                gUserCoordSavedFlag = 1;
                StoreRobotUserCoordSavedFlag();
            }
            memcpy((void *)&protocolParams[0], (void *)&ReturnInfo, sizeof(uint8_t));*/
        } else {
        }
        return true;
    }
}ProtocolRobotUserCoordinateCal_cmd;

class ProtocolRobotCalibrationParamsCmd : public IDobotCmd
{
public:
    explicit ProtocolRobotCalibrationParamsCmd() noexcept : IDobotCmd(ProtocolRobotCalibrationParams)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());

        uint8_t Axis_ID;
        Axis_ID = protocolParams[0];
        float zeroAngleOffset;
        float rearLinkError = 0;

        auto sysParams = &gSysParams;

        if(request->isWrite()) {
            switch(Axis_ID) {
                case 0:
                    memcpy((void *)&zeroAngleOffset, (void *)&protocolParams[1], sizeof(float));
                    /*参数有效性判断*/
                    if(fabs(zeroAngleOffset) > ROT_MOTORANGLEERR_ZOOM) {
                        zeroAngleOffset = 0;
                    }
                    gSysParams.RotAngleError += zeroAngleOffset;
                    //StoreRotAngleError();
                    //gSysParams.RotAngleError = ReadRotAngleError();
                    if(fabs(gSysParams.RotAngleError) > ROT_MOTORANGLEERR_ZOOM) {
                        gSysParams.RotAngleError = 0;
                    }
                    //立即更新当前的底座关节角度（避免断电重启后再更新）
                    gSysParams.pose.jointAngle[0] += zeroAngleOffset;
                    break;
                case 1:
                    rearLinkError = (*(float *)&protocolParams[5]) - BIGARM_LENGTH;
                    memcpy((void *)&sysParams->rearLinkError, &rearLinkError, sizeof(float));
                    /*参数有效性判断*/
                    if(fabs(*((float *)&protocolParams[1])) > L_MOTORANGLEERR_ZOOM) {
                        *((float *)&protocolParams[1]) = 0;
                    }
                    if(fabs(rearLinkError) > BIGARM_LINKERR_ZOOM) {
                        gSysParams.rearLinkError = 0;
                    }
                    //StoreRearArmParams(*((float *)&protocolParams[1]) + gSysParams.rearArmAngleError, sysParams->rearLinkError);
                    gRearArmSavedFlag = 1;
                    //ReadRearArmParams();
                    //StoreRearArmParamsSavedFlag();
                    //立即更新当前的关节角度（避免断电重启后再更新）
                    gSysParams.pose.jointAngle[1] += *((float *)&protocolParams[1]);
                    gDobotMechPara.joint1ArmLen = BIGARM_LENGTH + gSysParams.rearLinkError;
                    break;
                case 2:
                    rearLinkError = (*(float *)&protocolParams[5]) - SMALLARM_LENGTH;
                    memcpy((void *)&sysParams->frontLinkError, &rearLinkError, sizeof(float));
                    /*参数有效性判断*/
                    if(fabs(*((float *)&protocolParams[1])) > R_MOTORANGLEERR_ZOOM) {
                        *((float *)&protocolParams[1]) = 0;
                    }
                    if(fabs(rearLinkError) > SMALLARM_LINKERR_ZOOM) {
                        gSysParams.frontLinkError = 0;
                    }
                    //StorefrontArmParams(*((float *)&protocolParams[1]) + gSysParams.frontArmAngleError, sysParams->frontLinkError);
                    gFrontArmSavedFlag = 1;
                    //ReadfrontArmParams();
                    //StoreFrontArmParamsSavedFlag();
                    //立即更新当前的关节角度（避免断电重启后再更新）
                    gSysParams.pose.jointAngle[2] += *((float *)&protocolParams[1]);
                    gDobotMechPara.joint2ArmLen = SMALLARM_LENGTH + gSysParams.frontLinkError;
                    break;
                default:
                    break;
            }
        } else {
            uint8_t data[9];
            int length = 0;
            switch(Axis_ID) {
                case 0:
                    data[0] = Axis_ID;
                    memcpy((void *)&data[1], (void *)&sysParams->RotAngleError, sizeof(float));
                    length = 5;
                    break;
                case 1:
                    data[0] = Axis_ID;
                    memcpy((void *)&data[1], (void *)&sysParams->rearArmAngleError, sizeof(float));
                    memcpy((void *)&data[5], (void *)&gDobotMechPara.joint1ArmLen, sizeof(float));
                    length = 9;
                    break;
                case 2:
                    data[0] = Axis_ID;
                    memcpy((void *)&data[1], (void *)&sysParams->frontArmAngleError, sizeof(float));
                    memcpy((void *)&data[5], (void *)&gDobotMechPara.joint2ArmLen, sizeof(float));
                    length = 9;
                    break;
                default:
                    break;
            }
            response->setPayload(data, length);
        }
        return true;
    }
}ProtocolRobotCalibrationParams_cmd;

class ProtocolRobotDHParamsCmd : public IDobotCmd
{
public:
    explicit ProtocolRobotDHParamsCmd() noexcept : IDobotCmd(ProtocolRobotDHParams)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
        if(request->isWrite()) {
        } else {
            gSysParams.dhParams.DH[2] = END_LENGTH + gDobotMechPara.linkBlockH;
            response->setPayload(&gSysParams.dhParams, sizeof(gSysParams.dhParams));
        }
        return true;
    }
}ProtocolRobotDHParams_cmd;

class ProtocolUpdatePrepareCmd : public IDobotCmd
{
public:
    explicit ProtocolUpdatePrepareCmd() noexcept : IDobotCmd(0)//ProtocolUpdatePrepare
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
        }
        return true;
    }
}ProtocolUpdatePrepare_cmd;

class ProtocolMcuTemperatureCmd : public IDobotCmd
{
public:
    explicit ProtocolMcuTemperatureCmd() noexcept : IDobotCmd(ProtocolMcuTemperature)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        float McuTemp = 0.0;
        if(request->isWrite()) {
        } else {
            uint8_t protocolParams[16];
            McuTemp = 0;//GetMcuTemp();
            (*(float *)(protocolParams)) = McuTemp;
            (*(float *)(protocolParams + 4)) = 90;
            (*(float *)(protocolParams + 8)) = 60;
            (*(float *)(protocolParams + 12)) = 50;
            response->setPayload(protocolParams, 16);
        }
        return true;
    }
}ProtocolMcuTemperature_cmd;

class ProtocolPTPTimeCmd : public IDobotCmd
{
public:
    explicit ProtocolPTPTimeCmd() noexcept : IDobotCmd(ProtocolPTPTime)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());

        PTPCmd ptpCmd;
        memcpy(&ptpCmd, &protocolParams[0], sizeof(PTPCmd));
        float ptpTimeTemp = GetPlaybackTime(ptpCmd.x, ptpCmd.y, ptpCmd.z, ptpCmd.rHead, ptpCmd.ptpMode);
        if(ptpTimeTemp < 0) {
            ptpTimeTemp = 0;
        }
        uint32_t ptpTime = (uint32_t)(ptpTimeTemp * 1000);
        response->setPayload(&ptpTime, sizeof(uint32_t));

        if(request->isWrite()) {
        } else {
            response->setPayload(protocolParams, 16);
        }
        return true;
    }
}ProtocolPTPTime_cmd;

class ProtocolJointZeroCalCmd : public IDobotCmd
{
public:
    explicit ProtocolJointZeroCalCmd() noexcept : IDobotCmd(ProtocolJointZeroCal)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            //JointZeroCalFlag = protocolParams[0];
            //Func_JointZeroCal();
        } else {
            /*获取零点的编码器值*/
            uint8_t protocolParams[16];
            memcpy(&protocolParams[0], &gPosZero[0], sizeof(int32_t));
            memcpy(&protocolParams[4], &gPosZero[1], sizeof(int32_t));
            memcpy(&protocolParams[8], &gPosZero[2], sizeof(int32_t));
            memcpy(&protocolParams[12], &gPosZero[3], sizeof(int32_t));

            response->setPayload(protocolParams, 16);
        }
        return true;
    }
}ProtocolJointZeroCal_cmd;

class ProtocolMagneticCoderCalibStartCmd : public IDobotCmd
{
public:
    explicit ProtocolMagneticCoderCalibStartCmd() noexcept : IDobotCmd(ProtocolMagneticCoderCalibStart)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
        uint8_t Axis_ID = protocolParams[0];

        if(request->isWrite()) {
            if(Axis_ID > 3) {
                return true;
            }

            /**********************************************************************************************************************************/
            /**********************************************************************************************************************************/
            /*这4个变量是嵌入式的变量，这里为了让编译通过添加的了，意义不大*/
            uint8_t  gEncodeLinearityCalibration_StartFlag[4] = {0};
            uint8_t  gEncodeLinearityCalibrationTest_StartFlag[4] = {0};
            uint8_t  gBuildTable_StartFlag[3] = {0};
            uint8_t  gBuildTableTest_StartFlag[3] = {0};
            /**********************************************************************************************************************************/
            /**********************************************************************************************************************************/

            switch(Axis_ID) {
                case 0:
                    if(protocolParams[1] == 1) {                 //编码器线性度校准开启
                        gEncodeLinearityCalibration_StartFlag[0] = 1;
                        gStepCtrlModeTemp = 1;
                    } else if(protocolParams[1] == 2) {           //编码器线性度校准精度测试开启
                        gEncodeLinearityCalibrationTest_StartFlag[0] = 1;
                        gStepCtrlModeTemp = 2;
                    }
                    break;
                case 1:
                    if(protocolParams[1] == 1) {   //编码器线性度校准开启
                        gEncodeLinearityCalibration_StartFlag[1] = 1;
                        gStepCtrlModeTemp = 1;
                    } else if(protocolParams[1] == 2) {          //编码器线性度校准精度测试开启
                        gEncodeLinearityCalibrationTest_StartFlag[1] = 1;
                        gStepCtrlModeTemp = 2;
                    } else if(protocolParams[1] == 3) {          //整步校准---建表测试开始标志
                        gBuildTable_StartFlag[1] = 1;
                        gStepCtrlModeTemp = 3;
                    } else if(protocolParams[1] == 4) {          ///整步校准---建表开始标志
                        gBuildTableTest_StartFlag[1] = 1;
                        gStepCtrlModeTemp = 4;
                    }
                    break;
                case 2:
                    if(protocolParams[1] == 1) {   //编码器线性度校准开启
                        gEncodeLinearityCalibration_StartFlag[2] = 1;
                        gStepCtrlModeTemp = 1;
                    } else if(protocolParams[1] == 2) {          //编码器线性度校准精度测试开启
                        gEncodeLinearityCalibrationTest_StartFlag[2] = 1;
                        gStepCtrlModeTemp = 2;
                    } else if(protocolParams[1] == 3) {          //整步校准---建表测试开始标志
                        gBuildTable_StartFlag[2] = 1;
                        gStepCtrlModeTemp = 3;
                    } else if(protocolParams[1] == 4) {          ///整步校准---建表开始标志
                        gBuildTableTest_StartFlag[2] = 1;
                        gStepCtrlModeTemp = 4;
                    }
                    break;
                case 3:
                    if(protocolParams[1] == 1) {                //编码器线性度校准开启
                        gEncodeLinearityCalibration_StartFlag[3] = 1;
                        gStepCtrlModeTemp = 1;
                    } else if(protocolParams[1] == 2) {          //编码器线性度校准精度测试开启
                        gEncodeLinearityCalibrationTest_StartFlag[3] = 1;
                        gStepCtrlModeTemp = 2;
                    }else if(protocolParams[1] == 3) {          //整步校准---建表测试开始标志
                        gBuildTable_StartFlag[0] = 1;
                        gStepCtrlModeTemp = 3;
                    } else if(protocolParams[1] == 4) {          ///整步校准---建表开始标志
                        gBuildTableTest_StartFlag[0] = 1;
                        gStepCtrlModeTemp = 4;
                    }
                    break;
                default:
                    break;
            }
        } else {
        }
        return true;
    }
}ProtocolMagneticCoderCalibStart_cmd;

class ProtocolMagneticCoderCalibStateGetCmd : public IDobotCmd
{
public:
    explicit ProtocolMagneticCoderCalibStateGetCmd() noexcept : IDobotCmd(ProtocolMagneticCoderCalibStateGet)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            uint8_t Axis_ID = protocolParams[0];

            if(Axis_ID > 4) {
                return true;
            }

            /**********************************************************************************************************************************/
            /**********************************************************************************************************************************/
            /*这几个变量是嵌入式的变量，这里为了让编译通过添加的了，意义不大*/
            uint8_t gAutoLevelingCalFlag[4] = {Axis_ID,Axis_ID,Axis_ID,Axis_ID};
            /**********************************************************************************************************************************/
            /**********************************************************************************************************************************/

            uint8_t data = Axis_ID;
            switch(Axis_ID) {
                case 0:
                    break;
                case 1:
                    if(CalibrationMode == 1) {
                        data = gAutoLevelingCalFlag[0];
                    }
                    break;
                case 2:
                    if(CalibrationMode == 2) {
                        data = gAutoLevelingCalFlag[1];
                    }
                    break;
                case 3:
                    if(CalibrationMode == 3) {
                        data = gAutoLevelingCalFlag[2];
                    }
                    break;
                case 4:
                    if(CalibrationMode == 4) {
                        data = gAutoLevelingCalFlag[3];
                    }
                    break;
                default:
                    break;
            }
            response->setPayload(&data, 1);
        }
        return true;
    }
}ProtocolMagneticCoderCalibStateGet_cmd;

class ProtocolMagneticCoderRawDataGetCmd : public IDobotCmd
{
public:
    explicit ProtocolMagneticCoderRawDataGetCmd() noexcept : IDobotCmd(0)//ProtocolMagneticCoderRawDataGet
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
        }
        return true;
    }
}ProtocolMagneticCoderRawDataGet_cmd;

class ProtocolMakeFSCmd : public IDobotCmd
{
public:
    explicit ProtocolMakeFSCmd() noexcept : IDobotCmd(ProtocolMakeFS)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        uint8_t WrState = 0;
        if(request->isWrite()) {
            const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
            if(protocolParams[0] == 100) {
                WrState = 0;//MakeFileSys();
            }
            uint8_t data = WrState;
            response->setPayload(&data, 1);
        } else {
        }
        return true;
    }
}ProtocolMakeFS_cmd;

class ProtocolMagneticCoderParamsConfigCmd : public IDobotCmd
{
public:
    explicit ProtocolMagneticCoderParamsConfigCmd() noexcept : IDobotCmd(ProtocolMagneticCoderParamsConfig)
    {
    }

#define SET_ENC_DIR         1
#define SET_ENC_BCT         2
#define SET_ENC_ZERO        3
#define READ_ENC_VAULE      4
    bool exec(const Packet* request, Packet* response) override
    {
        const uint8_t* protocolParams = reinterpret_cast<const uint8_t*>(request->payload());
        uint8_t Func_ID = 0;
        uint8_t Axis_ID = 0;
        Axis_ID = protocolParams[0];
        Func_ID = protocolParams[1];
        uint32_t EncValue_InAxis0 = 0;
        uint16_t EncValue_OutAxis[3] = {0};

        if(request->isWrite()) {
            if(Func_ID == SET_ENC_DIR) {
                if(Axis_ID > 2) {
                    return true;
                }
                //MA730_DirSet(Axis_ID, protocolParams[2]);
            }
            if(Func_ID == SET_ENC_BCT) {
                if(Axis_ID > 2) {
                    return true;
                }
                //MA730_BctEtxEtySet(Axis_ID, (uint16_t)protocolParams[2], (uint16_t)protocolParams[3]);
            }
            if(Func_ID == SET_ENC_ZERO) {
                if(Axis_ID > 2) {
                    return true;
                }
                //MA730_ZeroSet(Axis_ID, (uint16_t)protocolParams[2], (uint16_t)protocolParams[3]);
            }
        } else {
            uint8_t data[256]={0};
            int length = 0;
            if(Func_ID == SET_ENC_DIR) {
                if(Axis_ID > 2) {
                    return true;
                }
                data[0] = Axis_ID;
                data[1] = Func_ID;
                data[2] = 0;//MA730_DirRead(Axis_ID);
                length = 3;
            }
            if(Func_ID == SET_ENC_BCT) {
                if(Axis_ID > 2) {
                    return true;
                }
                data[0] = Axis_ID;
                data[1] = Func_ID;
                data[2] = 0;//MA730_EtxEtyRead(Axis_ID);
                data[3] = 0;//MA730_BctRead(Axis_ID);
                length = 4;
            }
            if(Func_ID == SET_ENC_ZERO) {
                if(Axis_ID > 2) {
                    return true;
                }
                data[0] = Axis_ID;
                data[1] = Func_ID;
                data[2] = 0;//MA730_Z0Read(Axis_ID);
                data[3] = 0;//MA730_Z1Read(Axis_ID);
                length = 4;
            }
            if(Func_ID == READ_ENC_VAULE) {
                if(Axis_ID > 3) {
                    return true;
                }
                switch(Axis_ID) {
                    case 0:
                        data[0] = Axis_ID;
                        data[1] = Func_ID;
                        EncValue_OutAxis[0] = 0;//EncRead_OutAxis(0);
                        memcpy(&data[2], &EncValue_OutAxis[0], sizeof(uint32_t));
                        break;
                    case 1:
                        data[0] = Axis_ID;
                        data[1] = Func_ID;
                        EncValue_OutAxis[1] = 0;//EncRead_OutAxis(1);
                        memcpy(&data[2], &EncValue_OutAxis[1], sizeof(uint32_t));
                        break;
                    case 2:
                        data[0] = Axis_ID;
                        data[1] = Func_ID;
                        EncValue_OutAxis[2] = 0;//EncRead_OutAxis(2);
                        memcpy(&data[2], &EncValue_OutAxis[2], sizeof(uint32_t));
                        break;
                    case 3: //底座输入轴编码器的值
                        data[0] = Axis_ID;
                        data[1] = Func_ID;
                        EncValue_InAxis0 = 0;//(uint32_t)(EncRead_InAxis(0));
                        memcpy(&data[2], &EncValue_InAxis0, sizeof(uint32_t));
                        break;
                    default:
                        break;
                }
                length = 6;
            }
            response->setPayload(protocolParams, 16);
        }
        return true;
    }
}ProtocolMagneticCoderParamsConfig_cmd;

class ProtocolPcbaTest_AuxMcuToML_StartTestCmd : public IDobotCmd
{
public:
    explicit ProtocolPcbaTest_AuxMcuToML_StartTestCmd() noexcept : IDobotCmd(0)//ProtocolPcbaTest_AuxMcuToML_StartTest
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
        }
        return true;
    }
}ProtocolPcbaTest_AuxMcuToML_StartTest_cmd;

class ProtocolPcbaTest_AuxMcuToML_InquireResultCmd : public IDobotCmd
{
public:
    explicit ProtocolPcbaTest_AuxMcuToML_InquireResultCmd() noexcept : IDobotCmd(0)//ProtocolPcbaTest_AuxMcuToML_InquireResult
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
        }
        return true;
    }
}ProtocolPcbaTest_AuxMcuToML_InquireResult_cmd;

class ProtocolPcbaTest_PcToAuxMcu_StartTestCmd : public IDobotCmd
{
public:
    explicit ProtocolPcbaTest_PcToAuxMcu_StartTestCmd() noexcept : IDobotCmd(0)//ProtocolPcbaTest_PcToAuxMcu_StartTest
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
        }
        return true;
    }
}ProtocolPcbaTest_PcToAuxMcu_StartTest_cmd;

class ProtocolPcbaTest_PcToAuxMcu_InquireResultCmd : public IDobotCmd
{
public:
    explicit ProtocolPcbaTest_PcToAuxMcu_InquireResultCmd() noexcept : IDobotCmd(0)//ProtocolPcbaTest_PcToAuxMcu_InquireResult
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
        }
        return true;
    }
}ProtocolPcbaTest_PcToAuxMcu_InquireResult_cmd;

class ProtocolStepPlusDirCtrlCmd : public IDobotCmd
{
public:
    explicit ProtocolStepPlusDirCtrlCmd() noexcept : IDobotCmd(0)//ProtocolStepPlusDirCtrl
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
        }
        return true;
    }
}ProtocolStepPlusDirCtrl_cmd;

class ProtocolVariableDebugCmd : public IDobotCmd
{
public:
    explicit ProtocolVariableDebugCmd() noexcept : IDobotCmd(0)//ProtocolVariableDebug
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        if(request->isWrite()) {
        } else {
        }
        return true;
    }
}ProtocolVariableDebug_cmd;
