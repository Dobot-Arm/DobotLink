/****************************************Copyright(c)*****************************************************
**                            Shenzhen Yuejiang Technology Co., LTD.
**
**                                 http://www.dobot.cc
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           command.cpp
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
#include "variable.h"

DOBOTSTRUCT gDobotMechPara;
SoftLimit gSoftLimit;

TCPosition gOrigPos;
TCPosition gDistPos;

TCPosition gOrigPosJump;
TCPosition gDistPosJump;

int gRobotPrfMode;
int gSingleMode;
int gDobotError;
int gMotionType;

SysParams gSysParams;
ParamStorage gParamStorage;
InformationStorage gInformationStorage;

uint8_t HoareFlag;
uint8_t DecoderMove;
uint8_t HoldRHeadFlag;
uint8_t CP_TrackFlag;
uint8_t JumpLineFlag;

Pulse gPulse;
Stepper gStepper;
JointMove gJointMove;
LineMove gLineMove;
CircleMove gCircleMove;
SingleMove gSingleMove;
CPParams gLookAheadParams;
typ_interp_segment seg;
typ_marlin_interp_segment segMarlin;

bool finishFlag; /* 指令结束标志位 */
bool planFlag;   /* 数据规划标志位 */
bool stopPlanFlag;
bool stopFinishFlag;

/* 再现运动速度比例和加速度比例 */
float velRatio;
float accRatio;

float heightMiddle;

/* 单轴速度比例  */
float singleVelRatio;

float periodTime; /* 插补周期 */

Home gHome[ROBOT_AXIS];
TRobotTheta gHomePoint;

Axis JointAxis[ROBOT_AXIS];
Axis Line; /* 直线规划结果 */
Axis AllAxis;
Axis Gesture;

/* JUMP函数  */
bool transFlag;   /* 门型提升标志位 */
bool jumPlanFlag; /* 门型平移标志位 */
bool downFlag;    /* 门型下降标志位 */

/* GetSpeedJoint */
int periodCount; /* 周期计数  */

volatile int flashEnd;

/* Home标志，判断启动后是否执行过 */
uint8_t gHomeExecuted;
uint8_t gOfflineHomeExecuted; /* 离线时使用 */

/* LED 灯颜色指示标志 */
uint8_t gLedOffline;
uint8_t gLedHome = 0;

float vbg;

/* 舵机最大旋转角度标志 0--300° 1-- 270° */
uint8_t gServoFlag;

uint8_t LaserGravingFlag;

/*末端配件类型*/
uint8_t gEffectorType = 0;
uint8_t gEffectorEnale[2] = {0};
uint8_t gEffectorStatus[2] = {0};
/*末端长度参数*/
float  gSuctioncup_LinkBlockH = 93;
float  gGripper_LinkBlockH = 93;
float  gPen_LinkBlockH = 78.54;


float gVelocityLimit[4] = {300,300,300,300};
float gAccelrationLimit[4] = {400,400,400,400};
float gxyzVelocityLimit = 300;
float gxyzAccelerationLimit = 400;
float grVelocityLimit = 200;
float grAccelerationLimit = 300;
/*CP*/
float planAccLimit = 100;
float juncitionVelLimit = 100;
/*Jog*/
float gJogVelocityLimit[4] = {100,100,100,100};
float gJogAccelrationLimit[4] = {200,200,200,200};
/*ARC*/
float gArcxyzVelocityLimit = 300;
float gArcxyzAccelerationLimit = 400;
float gArcrVelocityLimit = 200;
float gArcrAccelerationLimit = 300;
float gAngleOffSet[ROBOT_AXIS] = {0};
