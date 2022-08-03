#pragma once
#include "symbol.h"
#include <cstdint>

//-----------------------------------------------------------------------------
// 基本数据类型定义
//-----------------------------------------------------------------------------
#ifndef NULL
#define NULL                           ((void*)0)
#endif

#ifndef TRUE
#define TRUE                           (1)
#endif

#ifndef FALSE
#define FALSE                          (0)
#endif

#define POSITIVE                       (1)
#define NEGATIVE                       (-1)

#define PI                              (3.14159265358979f)
#define ZERO                            (0.0001)

typedef union {
    uint8_t  data8[2];
    int16_t  data16;
} TData16;

typedef union {
    uint16_t data16[2];
    int32_t  data32;
} TData32;

typedef union {
    uint16_t data16[4];
    uint32_t data32[2];
    int64_t  data64;
} TData64;

//-----------------------------------------------------------------------------
// TDi
//-----------------------------------------------------------------------------
typedef struct {
    float state;    //状态
    float Axise;    //轴运动指令
    float X; //相对位置
    float Y; //相对位置
    float Z; //相对位置
    float RHead;
    float isGrab;
    float StartVel;//起始速度
    float EndVel;//结束速度
    float MaxVel;    //本段最大速度
} PackageStruct;

typedef struct {
    float X;
    float Y;
    float Z;
    float RHead;//舵机角度
    float isGrab;//手抓吸盘状态
    float baseAngle;//底座角度
    float longArmAngle;//大臂角度
    float shortArmAngle;//小臂角度
    float pawAngle;//手抓角度
    float gripperAngle;
} CurrentCoord;

typedef struct {
    float s;//位移
    float v;//速度
    int dir;
    float t1;//加速时间
    float t2;//匀速时间
    float T;//总时间
    float a;//加速度
    bool finishFlag;//运动完成标志位

} Axis; //各轴直线加减速规划结果

typedef struct {
    int lastSendPulseSum[ROBOT_AXIS];
    float sendPulseSum[ROBOT_AXIS];
    float sendSpeedSum[ROBOT_AXIS];//速度
    int   pulseFPGA[ROBOT_AXIS];
    float angleFPGA[ROBOT_AXIS] ;
    int stepSum[ROBOT_AXIS];
} Pulse;

typedef struct {
    float stepAngle;//步距角
    float subDiv[ROBOT_AXIS];//细分数
    float gearRatio[ROBOT_AXIS];//减速比
    int stepDir[ROBOT_AXIS];
    float stepPerDegree[ROBOT_AXIS];
} Stepper;

typedef struct {
    float target[ROBOT_AXIS];//要到达的绝对位置
    float stepLast[ROBOT_AXIS];//已到达的各轴位置
    float speed[ROBOT_AXIS];//各轴每周期规划的速度
} JointMove;

typedef struct {
    float dir[ROBOT_AXIS];
    float stepLastL;
    float speedLine;
    float speedRot;
    float dirR;
    float stepLastR;
} LineMove;

typedef struct {
    float singleVel;//单轴运动速度
    float singleVelMax;//单轴运动最大速度
    float singleAcc;//单轴运动加速度
    int lastState;//点动状态
} SingleMove;

typedef struct {
    float theta[ROBOT_AXIS];
} TRobotTheta;

typedef struct {
    float x;                                                // 位置X,mm
    float y;                                                // 位置Y,mm
    float z;                                                // 位置Z,mm
    float r;                                                // 姿态A,deg
} TCPosition;

// pos & pose
typedef struct {
    double x;
    double y;
    double z;
    double a;
    double b;
    double c;
} WCSPosition;

typedef struct VECTOR3D_STR
{
    float item01;
    float item02;
    float item03;
}VECTOR3D;

//6*1 vector
typedef struct VECTOR6D_STR
{
    float item01;
    float item02;
    float item03;
    float item04;
    float item05;
    float item06;
}VECTOR6D;


//3*3 matrix
typedef struct MATRIX3D_STR
{
    float item11;
    float item12;
    float item13;
    float item21;
    float item22;
    float item23;
    float item31;
    float item32;
    float item33;
}MATRIX3D;

//4*4 matrix
typedef struct MATRIX4D_STR
{
    float item11;
    float item12;
    float item13;
    float item14;
    float item21;
    float item22;
    float item23;
    float item24;
    float item31;
    float item32;
    float item33;
    float item34;
    float item41;
    float item42;
    float item43;
    float item44;
}MATRIX4D;

typedef struct
{
        MATRIX3D planeRotM;
        VECTOR3D arcCenter;
        VECTOR3D rotAxis;
        float arcRadius;
        float arcAng;
        float rotAng;
}CirlPlanPara;

typedef struct
{
   CirlPlanPara circlePlanPara;
   
   TCPosition midPos;
   TCPosition endPos;
   TCPosition beginPos;
    
}CircleMove;

typedef struct DOBOTSTRUCT_STR {
    float joint1ArmLen;
    float joint2ArmLen;
    float baseH;
    float baseV;
    float linkBlockH;
    float linkBlockV;
    float linkBlockY;
} DOBOTSTRUCT;

typedef struct {
    float jointPos[ROBOT_AXIS];
    float jointNeg[ROBOT_AXIS];
    float parallelJoint12Pos;
    float parallelJoint12Neg;
    float crdPos[ROBOT_AXIS];
    float crdNeg[ROBOT_AXIS];
    float disMOVJ;

} SoftLimit;

typedef struct {
    int homeState;
    float seekHomeSpd;
    float backHomeSpd;
    float findZSpd;
    float homePos;
    int homeDir;

} Home;

typedef struct {
    int motion_stat;
    int motion_stat_E;

    long term_cmd;
    long term_run;
    long term_rem;

    int term_add;
    int term_unfm;
    int term_dec;

    float AT;
    float time_delta;
    float term_length;
    float term_vel;
    float vel;
    float vDelta;
} typ_interp_time;

typedef struct {
    PackageStruct gCode;
    float dist;
    float accelat;
    float axis_vel[3];
    float dist_out[3];

    typ_interp_time interp_time;
    float axis_seg[3];

    float vb;
    float vm;
    float ve;
    float vc;
    float vd;
    float va;
} typ_interp_segment;

typedef struct {
    float interp_length;
    float interp_vel;
    float accelerate_until;                    // The index of the step event on which to stop acceleration
    float decelerate_after;                    // The index of the step event on which to start decelerating
    float nominal_speed;                               // The nominal speed for this block in mm/sec
    float entry_speed;                                 // Entry speed at previous-current junction in mm/sec
    float exit_speed;                             // Maximum allowable junction entry speed in mm/sec
    float millimeters;                                 // The total travel of this block in mm
    float acceleration;                                // acceleration mm/sec^2
    float axis_seg[3];
    float axis_gcode[3];
    float dist_out[3];
} typ_marlin_interp_segment;
/*********************************************************************************************************
** 数据结构等定义
*********************************************************************************************************/

/*********************************************************************************************************
** 公共部分
*********************************************************************************************************/
#pragma pack(push)
#pragma pack(1)

/*
 * Pose
 */
typedef struct tagPose {
    float x;
    float y;
    float z;
    float rHead;
    float jointAngle[ROBOT_AXIS];
} Pose;

typedef struct tagDHParams {
    float theta_min[5];
    float theta_max[5];
    float DH[4];
    float Z_Limit[2];
} DHParams;

/*
 * Kinematics
 */
typedef struct tagKinematics {
    float velocity;
    float acceleration;
} Kinematics;

typedef struct tagHOMEParams {
    uint32_t temp;
} HOMEParams;

typedef struct tagHOMECmd {
    uint32_t temp;
} HOMECmd;

/*
 * HHT
 */
typedef enum tagHHTTrigMode {
    TriggeredOnKeyReleased,
    TriggeredOnPeriodicInterval
} HHTTrigMode;

/*
 * End effector
 */
typedef struct tagEndEffectorParams {
    float xBias;
    float yBias;
    float zBias;
} EndEffectorParams;

/*********************************************************************************************************
** Arm orientation
*********************************************************************************************************/
typedef enum tagArmOrientation {
    LeftyArmOrientation,
    RightyArmOrientation,
} ArmOrientation;

/*********************************************************************************************************
** 点动示教部分
*********************************************************************************************************/
/*
 * 单关节点动示教参数
 */
typedef struct tagJOGJointParams {
    float velocity[4];
    float acceleration[4];
} JOGJointParams;

/*
 * 单坐标轴点动示教参数
 */
typedef struct tagJOGCoordinateParams {
    float velocity[4];
    float acceleration[4];
} JOGCoordinateParams;

/*
 * 点动示教公共参数
 */
typedef struct tagJOGCommonParams {
    float velocityRatio;
    float accelerationRatio;
} JOGCommonParams;

/*
 * 点动示教所有参数
 */
typedef struct tagJOGParams {
    JOGJointParams jointParams;
    JOGCoordinateParams coordinateParams;
    JOGCommonParams commonParams;
} JOGParams;

/*
 * Jog Cmd
 */
typedef struct tagJOGCmd {
    uint8_t isJoint;
    uint8_t cmd;
} JOGCmd;

/*********************************************************************************************************
** 再现运动部分
*********************************************************************************************************/
/*
 * 再现运动参数
 */
typedef struct tagPTPJointParams {
    float velocity[4];
    float acceleration[4];
} PTPJointParams;

typedef struct tagPTPCoordinateParams {
    float xyzVelocity;
    float rVelocity;
    float xyzAcceleration;
    float rAcceleration;
} PTPCoordinateParams;

typedef struct tagPTPJumpParams {
    float jumpHeight;
    float maxJumpHeight;
} PTPJumpParams;

typedef struct tagPTPJump2Params {
    float startJumpHeight;
    float endJumpHeight;
    float zLimit;
} PTPJump2Params;

typedef struct tagPTPCommonParams {
    float velocityRatio;
    float accelerationRatio;
} PTPCommonParams;

typedef struct tagPTPParams {
    PTPJointParams jointParams;
    PTPCoordinateParams coordinateParams;
    PTPJumpParams jumpParams;
    PTPJump2Params jump2Params;
    PTPCommonParams commonParams;
} PTPParams;

typedef struct tagPTPCmd {
    uint8_t ptpMode;
    float x;
    float y;
    float z;
    float rHead;
} PTPCmd;

/*********************************************************************************************************
** 连续轨迹：Continuous path
*********************************************************************************************************/
/*
 * CP参数
 */
typedef struct tagCPParams {
    float planAcc;
    float juncitionVel;
    union {
        float acc;
        float period;
    };
    uint8_t realTimeTrack;
} CPParams;

typedef enum tagCPMode {
    CPRelativeMode,
    CPAbsoluteMode
} CPMode;

typedef struct tagLookAhead {
    float AT;
    float vd;
    float ve;
    float va;
    float vc;
    float dist;
    float juctionAngle;

    float maxVel;
    float startVel;
    float endVel;
} LookAhead;

typedef struct tagMarlin {
    // Fields used by the motion planner to manage acceleration
    // float speed_x, speed_y, speed_z, speed_e;          // Nominal mm/sec for each axis
    float nominal_speed;                               // The nominal speed for this block in mm/sec
    float entry_speed;                                 // Entry speed at previous-current junction in mm/sec
    float max_entry_speed;                             // Maximum allowable junction entry speed in mm/sec
    float millimeters;                                 // The total travel of this block in mm
    float acceleration;                                // acceleration mm/sec^2
    unsigned char recalculate_flag;                    // Planner flag to recalculate trapezoids on entry junction
    unsigned char nominal_length_flag;                 // Planner flag for nominal speed always reached

} Marlin;

typedef struct tagCPCmd {
    uint8_t cpMode;
    float x;
    float y;
    float z;
    union {
        float velocity;
        float laserPower;
    };
    uint8_t isLaserEngraving;
    uint8_t laserStartControl;
    float abs_x;
    float abs_y;
    float abs_z;
    LookAhead lookAhead;
    Marlin marlin;
} CPCmd;

/*********************************************************************************************************
** 圆弧：ARC
*********************************************************************************************************/
typedef struct tagARCParams {
    float xyzVelocity;
    float rVelocity;
    float xyzAcceleration;
    float rAcceleration;
} ARCParams;

typedef struct tagARCCmd {
    struct {
        float x;
        float y;
        float z;
        float rHead;
    } cirPoint;
    struct {
        float x;
        float y;
        float z;
        float rHead;
    } toPoint;
} ARCCmd;

typedef struct tagCircleCmd {
    struct {
        float x;
        float y;
        float z;
        float rHead;
    } cirPoint;
    struct {
        float x;
        float y;
        float z;
        float rHead;
    } toPoint;
    uint32_t count;
} CircleCmd;

/*********************************************************************************************************
** WAIT cmd
*********************************************************************************************************/
typedef struct tagWAITCmd {
    uint32_t timeout;
} WAITCmd;

/*********************************************************************************************************
** TRIG cmd
*********************************************************************************************************/
typedef enum tagTRIGMode {
    TRIGInputIOMode,
    TRIGADCMode
} TRIGMode;

typedef enum tagTRIGInputIOCondition {
    TRIGInputIOEqual,
    TRIGInputIONotEqual
} TRIGInputIOCondition;

typedef enum tagTRIGADCCondition {
    TRIGADCLT,     //Lower than
    TRIGADCLE,     //Lower than or Equal
    TRIGADCGE,     //Greater than or Equal
    TRIGADCGT      //Greater than
} TRIGADCCondition;

typedef struct tagTRIGCmd {
    uint8_t address;
    uint8_t mode;
    uint8_t condition;
    uint16_t threshold;
} TRIGCmd;

/*********************************************************************************************************
** ZDF
*********************************************************************************************************/
typedef enum tagZDFCalibStatus {
    ZDFCalibNotFinished,
    ZDFCalibFinished,
} ZDFCalibStatus;

/*********************************************************************************************************
** 类型等定义
*********************************************************************************************************/
typedef enum tagQueuedCmdType {
    QueuedCmdMotionType,
    QueuedCmdWaitType,
    QueuedCmdIOType,
    QueuedCmdTrigType,
    QueuedCmdTypeNum
} QueuedCmdType;

typedef enum tagQueuedCmdExecMode {
    QueuedCmdExecOnlineMode,
    QueuedCmdExecOfflineMode
} QueuedCmdExecMode;

typedef enum tagRunOfflineQueuedCmdState {
    RunOfflineQueuedCmdIdleState,
    RunOfflineQueuedCmdRunState
} RunOfflineQueuedCmdState;

typedef struct tagOfflineLoopLine {
    bool isDownloading;
    uint32_t offlineLoop;
    uint32_t offlineLine;
    bool     offlineLoopLineWriteFlag;
    bool     offlineLoopLineReadFlag;
    uint32_t offlineLoopIndex;
    uint32_t offlineLineIndex;
} OfflineLoopLine;

#pragma pack(pop)

/*********************************************************************************************************
** System parameter
*********************************************************************************************************/
typedef struct tagSysParams {
    // Device information
    char deviceSN[24];
    char deviceName[65];
    char deviceVersion[3];

    // Pose
    Pose pose;
    Kinematics kinematics;
    DHParams dhParams;

    // Alarm

    // HOME
    struct {
        HOMEParams params;
        HOMECmd cmd;
    } home;

#ifdef STM32F10X_HD
    // HHT
    HHTTrigMode hhtTrigMode;
    bool hhtTrigOutputEnabled;
    bool hhtTrigOutput;
#endif

    // ARM
    ArmOrientation armOrientation;

    // End effector
    EndEffectorParams endEffectorParams;

    // JOG
    struct {
        JOGParams params;
        JOGCmd cmd;
    } jog;

    // PTP
    struct {
        PTPParams params;
        PTPCmd cmd;
    } ptp;

    // CP
    struct {
        CPParams params;
        CPCmd cmd;
        bool rHeadHoldEnable;
        float rHeadValue;
    } cp;

    // ARC
    struct {
        ARCParams params;
        ARCCmd cmd;
    } arc;
    CircleCmd circleCmd;

    // WAIT
    struct {
        WAITCmd cmd;
        uint32_t initialTick;
    } wait;

    // TRIG
    struct {
        TRIGCmd cmd;
    } trig;

    // EIO

    //LostStepValue
    float LostStepValue;
    float CollisionCheckValue;

    // CAL
    float RotAngleError;
    float rearArmAngleError;
    float frontArmAngleError;
    float rearLinkError;
    float frontLinkError;
    float AngleError[2];
    float AngleCoef[2];
    float RotErr[1];
    float HomeErr;
    
    // ZDF
    float zdfLaserCalibHeight;
    ZDFCalibStatus zdfCalibStatus;

    // Queued command control
    bool runQueuedCmd;
    // Total loop and line per loop used in offline downloading
    struct {
        bool isDownloading;
        uint32_t totalLoop;
        uint32_t linePerLoop;
        bool     writeFlag;
        bool     readFlag;
        uint32_t loopIndex;
        uint32_t lineIndex;
    } queuedCmdOfflineLoopLine;

    // #Internal use
    QueuedCmdType queuedCmdType;
    bool runOfflineQueuedCmd;
    RunOfflineQueuedCmdState runOfflineQueuedCmdState;

    // Soft
    bool isSoftOffTriggered;
    uint64_t softOffLastIndex;
} SysParams;

/*********************************************************************************************************
** System parameter Storage
*********************************************************************************************************/
typedef struct tagParamStorage {
    uint32_t storeLength;
    float    jogVelocityRatio;
    float    jogAccelerationRatio;
    float    ptpVelocityRatio;
    float    ptpAccelerationRatio;
    float    EndTypeXBias;
} ParamStorage;

/*********************************************************************************************************
** System Information Storage
*********************************************************************************************************/
typedef struct tagInfomationStorage {
    uint32_t storeLength;
    char deviceSN[24] ;
    char deviceName[65];
    uint8_t deviceVersion[3];
} InformationStorage;
