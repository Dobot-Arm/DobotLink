#pragma once

#define ROBOT_AXIS                          (4)                         // 机器人关节数


//-----------------------------------------------------------------------------
// Robot Profile mode
//-----------------------------------------------------------------------------
#define ROBOT_MODE_NONE                     (0)
#define ROBOT_MODE_SINGLE                   (1)
#define ROBOT_MODE_HOME                     (4)
#define ROBOT_MODE_PLAYBACK                 (6)
#define ROBOT_MODE_SINGLEXYZ                (2)
#define ROBOT_MODE_CONTINUOUS_PATH          (16)
#define ROBOT_MODE_STOP                     (3)
#define ROBOT_MODE_ARC                      (5)
#define ROBOT_MODE_LEVELING                 (10)

//#define ERROR_INV_CALCU                      (1)
//#define ERROR_INV_LIMIT                      (2)

#define LINE_MIN_DIST           0.0001f

#define LDEGREE                 0.1f
#define INP_MOTION_END          0x50
#define AXIS_SUM                5
//#define INTERP_RECI             (float)50
#define INTERP_RECI             (1/periodTime)
//点动枚举状态state
enum {
    IDEL,
    AP_DOWN,
    AN_DOWN,
    BP_DOWN,
    BN_DOWN,
    CP_DOWN,
    CN_DOWN,
    DP_DOWN,
    DN_DOWN
};

//两点间运动模式type
enum {
    JUMP_XYZ,//门型运动
    MOVJ_XYZ,//关节运动
    MOVL_XYZ,//直线运动
    JUMP_ANGLE,
    MOVJ_ANGLE,
    MOVL_ANGLE,
    MOVJ_INC,
    MOVL_INC,
    MOVJ_XYZ_INC,
    JUMP_MOVL_XYZ,
};

enum {
    PRESEEK_HOME,
    SEEK_HOME,
    BACK_HOME,
    SEEK_Z,
    FINISH_HOME,
    SOFT_HOME
};
