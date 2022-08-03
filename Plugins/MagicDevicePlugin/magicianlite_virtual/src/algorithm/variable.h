#pragma once

#include "type.h"
#include "symbol.h"

extern DOBOTSTRUCT gDobotMechPara;
extern SoftLimit gSoftLimit;

extern TCPosition gOrigPos;
extern TCPosition gDistPos;

extern TCPosition gOrigPosJump;
extern TCPosition gDistPosJump;

extern int gRobotPrfMode;
extern int gSingleMode;
extern int gDobotError;
extern int gMotionType;

extern SysParams gSysParams;
extern ParamStorage gParamStorage;
extern InformationStorage gInformationStorage;

extern   uint8_t HoareFlag;
extern   uint8_t DecoderMove;
extern   uint8_t HoldRHeadFlag;
extern   uint8_t CP_TrackFlag;
extern   uint8_t JumpLineFlag;

extern Pulse gPulse;
extern Stepper gStepper;
extern JointMove gJointMove;
extern LineMove gLineMove;
extern CircleMove gCircleMove;
extern SingleMove gSingleMove;
extern CPParams gLookAheadParams;
extern typ_interp_segment seg;
extern typ_marlin_interp_segment segMarlin;

extern bool finishFlag;         /* 指令结束标志位 */
extern bool planFlag;           /* 数据规划标志位 */
extern bool stopPlanFlag;
extern bool stopFinishFlag;

/* 再现运动速度比例和加速度比例 */
extern float velRatio;
extern float accRatio;

extern float heightMiddle;

/* 单轴速度比例 */
extern float singleVelRatio;

extern float periodTime;        /* 插补周期 */

extern Home gHome[ROBOT_AXIS];
extern TRobotTheta gHomePoint;
extern Axis JointAxis[ROBOT_AXIS];
extern Axis Line;               /* 直线规划结果 */
extern Axis Gesture;
extern Axis AllAxis;

/* JUMP函数 */
extern bool transFlag;          /* 门型提升标志位 */
extern bool jumPlanFlag;        /* 门型平移标志位 */
extern bool downFlag;           /* 门型下降标志位 */

/* 角度数据 */
extern float angle0, angle1, angle2;
extern int32_t decode_num;
extern int32_t DecoderOffset;
extern float Pitch1,Roll1,Yaw1;
extern float Pitch2,Roll2,Yaw2;
extern uint8_t AngleStableFlag;
extern void MpudmpGetData();
extern void ReSetInitialPose(float angle0, float angle1, float angle2);
extern void UpdatePresentPose();

/* GetSpeedJoint */
extern int periodCount;         /* 周期计数 */

extern volatile int flashEnd;

/* 判断是否执行过Home标志 */
extern uint8_t gHomeExecuted;
extern uint8_t gOfflineHomeExecuted;

/* LED 灯颜色指示标志 */
extern uint8_t gLedOffline;
extern uint8_t gLedHome;

extern float vbg;

/* 步进控制模式*/
extern uint8_t gStepCtrlMode;
extern uint8_t gStepCtrlModeTemp;

extern uint32_t gCircleCount;
extern uint8_t gWholeCircleFlag;
extern uint8_t CalibrationMode;

extern uint8_t gRearArmSavedFlag;
extern uint8_t gFrontArmSavedFlag;
extern uint8_t gUserCoordSavedFlag;

extern int32_t gPosZero[6];

extern uint8_t gFrontEnd_PWM_Flag;

/* 舵机最大旋转角度标志 0--300° 1-- 270° */
extern uint8_t gServoFlag;
extern uint8_t LaserGravingFlag;

/*机械臂速度比例*/
extern float gRobotRatio;
extern float gJogRobotRatio;
/*末端配件类型*/

/*末端配件类型*/
extern uint8_t gEffectorType;
extern uint8_t gEffectorEnale[2];
extern uint8_t gEffectorStatus[2];
/*末端长度参数*/
extern float  gSuctioncup_LinkBlockH;
extern float  gGripper_LinkBlockH;
extern float  gPen_LinkBlockH;
/*碰撞检测相关参数*/
extern uint8_t gCollisionDetectFlag;
extern uint8_t gKeyLostExeFlag;
/*机械臂各运行模式参数限制值*/

extern float gVelocityLimit[4];
extern float gAccelrationLimit[4];
extern float gxyzVelocityLimit;
extern float gxyzAccelerationLimit;
extern float grVelocityLimit;
extern float grAccelerationLimit;
/*CP*/
extern float planAccLimit;
extern float juncitionVelLimit;
/*Jog*/
extern float gJogVelocityLimit[4];
extern float gJogAccelrationLimit[4];
/*ARC*/
extern float gArcxyzVelocityLimit;
extern float gArcxyzAccelerationLimit;
extern float gArcrVelocityLimit;
extern float gArcrAccelerationLimit;
extern float gAngleOffSet[ROBOT_AXIS];
