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

#include <QDateTime>

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
        uint32_t error = 0;
        static uint8_t jogRatio, otherRotio;
        static uint8_t opMode = 0;
        PTPJointParams* ptpjointParams = &gSysParams.ptp.params.jointParams;
        PTPCoordinateParams* ptpcoordinateParams = &gSysParams.ptp.params.coordinateParams;
        JOGJointParams* jogParams = &gSysParams.jog.params.jointParams;
        JOGCoordinateParams* jogcoordinateParams = &gSysParams.jog.params.coordinateParams;
        CPParams* params = &gSysParams.cp.params;
        ARCParams* arcParams = &gSysParams.arc.params;

        auto input_params = reinterpret_cast<Param*>(request->payload());
        if (request->isWrite())
        {
            opMode = input_params->type;
            {
                uint8_t val = input_params->value;
                DOBOT_DEBUG("Set speed val : %d", val);
                float ratio = MIN_VAL(MAX_VAL(1, val), 100) / 100.0f;

                DOBOT_DEBUG("Set speed radio : %0.3f", ratio);

                for (uint8_t i = 0; i < AXIS_NUM; i++)
                {
                    if (opMode == JOG)
                    { /*Jog*/
                        jogRatio = val;
                        jogParams->velocity[i] = gJogVelocityLimit[i] * ratio;
                        jogParams->acceleration[i] = gJogAccelrationLimit[i] * ratio;
                        jogcoordinateParams->velocity[i] = gJogVelocityLimit[i] * ratio;
                        jogcoordinateParams->acceleration[i] = gJogAccelrationLimit[i] * ratio;
                    }
                    else if (opMode == OTHERS)
                    {/*PTP*/
                        otherRotio = val;
                        ptpjointParams->velocity[i] = gVelocityLimit[i] * ratio;
                        ptpjointParams->acceleration[i] = gAccelrationLimit[i] * ratio;
                        ptpcoordinateParams->xyzVelocity = gxyzVelocityLimit * ratio;
                        ptpcoordinateParams->xyzAcceleration = gxyzAccelerationLimit * ratio;
                        ptpcoordinateParams->rVelocity = grVelocityLimit * ratio;
                        ptpcoordinateParams->rAcceleration = grAccelerationLimit * ratio;
                        /*CP*/
                        params->planAcc = planAccLimit * ratio;
                        params->juncitionVel = juncitionVelLimit * ratio;
                        params->acc = planAccLimit * ratio;
                        /*ARC*/
                        arcParams->xyzAcceleration = gArcxyzAccelerationLimit * ratio;
                        arcParams->rAcceleration = gArcrAccelerationLimit * ratio;
                        arcParams->xyzVelocity = gArcxyzVelocityLimit * ratio;
                        arcParams->rVelocity = gArcrVelocityLimit * ratio;
                    }
                }
            }
        }
        else
        {
            Param result;
            result.type = input_params->type;
            if (input_params->type == JOG)
                result.value = jogRatio;
            else
                result.value = otherRotio;
            response->setPayload(&result, sizeof(result));
        }
        return true;
    }
} cmd_set_robot_vel_ratio;

class SetPTPJointParam : public IDobotCmd
{
public:
    explicit SetPTPJointParam() noexcept : IDobotCmd(CMD_ID_PTP_JOINT_PARAMS)
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("SetPTPCoordinateParam");

        PTPJointParams* params = &gSysParams.ptp.params.jointParams;
        memcpy(params, request->payload(), sizeof(PTPJointParams));
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

        PTPCoordinateParams* params = &gSysParams.ptp.params.coordinateParams;
        memcpy(params, request->payload(), sizeof(PTPCoordinateParams));
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

        PTPJumpParams* params = &gSysParams.ptp.params.jumpParams;
        memcpy(params, request->payload(), sizeof(PTPJumpParams));
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

        PTPCommonParams* params = &gSysParams.ptp.params.commonParams;
        memcpy(params, request->payload(), sizeof(PTPCommonParams));
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
private:
#pragma pack(push)
#pragma pack(1)
    struct CollisionParam
    {
        uint8_t on;
        float angle;
    };
#pragma pack(pop)

    CollisionParam param_;

public:
    explicit CollisionCheckCmd() noexcept : IDobotCmd(CMD_ID_COLLISION_CHECK), param_({ 0, 0.0 })
    {
    }

    bool exec(const Packet* request, Packet* response) override
    {
        DOBOT_DEBUG("CollisionCheckCmd");
        if (request->isWrite())
        {
            auto param = reinterpret_cast<CollisionParam*>(request->payload());
            DOBOT_DEBUG("%s collision check, %f", param->on ? "Enable" : "Disable", param->angle);

            if (param->angle < 8.0f)
                param->angle = 8.0;
            param_ = *param;
        }
        else
        {
            DOBOT_DEBUG("Get collision check: %d, %f", param_.on, param_.angle);
            response->setPayload(&param_, sizeof(param_));
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
