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

#include <cstring>
#include <cmath>
#include <algorithm/variable.h>

#if 0

#include "CollisionCheckFunc.h"
extern bool gQueuedCmdIsFinished[QueuedCmdTypeNum];
 
/*********************************************************************************************************
** Function name:       CollisionCheck
** Descriptions:        碰撞检测
** Input parameters:    None
** Output parameters:   None
** Returned value:      None
*********************************************************************************************************/

void CollisionCheck()
{
    static uint8_t error = 0; 
    static uint32_t timeError = 0;
    static float lastAngleErr[3] = {0};
    static uint8_t stateMachine = 0;
    float JointAngle[3] = {0};
    float AngleErr[3] = {0};
    float AngleErrErr[3] = {0};
    if((gCollisionDetectFlag == 1) && (gKeyLostExeFlag == 1)){
        MC_JointPulseToAngle(JointAngle);
        if(((gAlarmSysState[12]&0x01)!= 0x01)&&((gAlarmSysState[12]&0x02) != 0x02)&&((gAlarmSysState[12]&0x04) != 0x04)){
            AngleErr[0] = fabs(JointAngle[0] - gSysParams.pose.jointAngle[0]);
            AngleErr[1] = fabs(JointAngle[1] - gSysParams.pose.jointAngle[1]);
            AngleErr[2] = fabs(JointAngle[2] - gSysParams.pose.jointAngle[2]);
            
            switch(stateMachine){
                case 0: 
                    if((AngleErr[0] >= 4) || (AngleErr[1] >= 1.5) || (AngleErr[2] >= 1.5)){ 
                        AngleErrErr[0]=  0;
                        AngleErrErr[1]=  0;
                        AngleErrErr[2]=  0;
                        stateMachine = 1;
                    }
                    break;
                
                case 1:
                    AngleErrErr[0]=  fabs(AngleErr[0] - lastAngleErr[0]);
                    AngleErrErr[1]=  fabs(AngleErr[1] - lastAngleErr[1]);
                    AngleErrErr[2]=  fabs(AngleErr[2] - lastAngleErr[2]);
                    break;
                
                default:
                    break;
            }
            lastAngleErr[0] = AngleErr[0];
            lastAngleErr[1] = AngleErr[1];
            lastAngleErr[2] = AngleErr[2];
    #if DEBUG_COMMAND
            if(error == 0){
//                printf("Test_AgnleErrErr:%1.3f %1.3f %1.3f %1.3f %1.3f %1.3f\r\n", AngleErrErr[0],AngleErrErr[1],AngleErrErr[2],AngleErr[0], AngleErr[1],AngleErr[2]);
//                printf("Test_Angle:%1.3f %1.3f %1.3f %1.3f %1.3f %1.3f\r\n", JointAngle[0], JointAngle[1],JointAngle[2],gSysParams.pose.jointAngle[0], gSysParams.pose.jointAngle[1],gSysParams.pose.jointAngle[2]);
            }
    #endif
            if((gAlarmSysState[8] == 0)&&(gAlarmSysState[9] == 0) &&(error == 0)){
                if(AngleErr[0] > gSysParams.CollisionCheckValue || AngleErrErr[0] >= 4.0) {
                    gSingleMove.singleVel = 0;
                    AlarmSysSetBit(ERR_LOSE_STEP_AXIS1, true);
                    error = 1;
                }
                if(AngleErr[1] > gSysParams.CollisionCheckValue || AngleErrErr[1] >= 1.5) {
                    gSingleMove.singleVel = 0;
                    AlarmSysSetBit(ERR_LOSE_STEP_AXIS2, true);
                    error = 1;
                }
                if(AngleErr[2] > gSysParams.CollisionCheckValue || AngleErrErr[2] >= 1.5) {
                    gSingleMove.singleVel = 0;
                    AlarmSysSetBit(ERR_LOSE_STEP_AXIS3, true);
                    error = 1;
                }
            }
            if(error == 1) {
                gSysParams.runQueuedCmd = false;
                finishFlag = true;
                planFlag = false;
                jumPlanFlag = false;
                stopPlanFlag = false;
                gRobotPrfMode = ROBOT_MODE_STOP;
                for(bool & i : gQueuedCmdIsFinished) {
                    i = true;
                }
                /* 清除脱机标记 */
                if(gSysParams.runOfflineQueuedCmd) {
                    gSysParams.runOfflineQueuedCmd = false;
                }
                Buzzer_SET(Buzzer,3,250);
                timeError = GetSysTickStart();
                error = 2;
            }
            if(error == 2){
                /* 保证坐标有效 */
                if(CheckSysTickTimeOut(1000,timeError) == true){
                    MC_PresentPoseUpdate();
                    stateMachine = 0;
                    error = 0;
                }
            }
        }
    }
}

#endif
