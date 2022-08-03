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

#ifndef SIMULATION_MAGICIANLITE_SRC_RUNTIME_DCPS_CMD_ID_H_
#define SIMULATION_MAGICIANLITE_SRC_RUNTIME_DCPS_CMD_ID_H_

enum CmdID
{
    // Device information
    CMD_ID_GET_PRODUCT_NAME = 7,

    CMD_ID_GET_POSE = 10,
    ProtocolResetPose = CMD_ID_GET_POSE + 1,
    ProtocolGetKinematics = CMD_ID_GET_POSE + 2,
    ProtocolUpdatePose =  CMD_ID_GET_POSE + 4,
    CMD_ID_POS_CHECK_LIMIT = 15,

    CMD_ID_ALARM_STATUS = 20,

    CMD_ID_HOME_PARAM = 30,
    CMD_ID_HOME,
    ProtocolAutoLeveling = CMD_ID_HOME_PARAM + 2,
    ProtocolHomeStatus = CMD_ID_HOME_PARAM + 3,
    ProtocolAutoLevelSwitchState = CMD_ID_HOME_PARAM + 4,

    // HHT
    ProtocolFunctionHHTBase = 40,
    ProtocolHHTTrigMode = ProtocolFunctionHHTBase + 0,
    ProtocolHHTTrigOutputEnabled = ProtocolFunctionHHTBase + 1,
    ProtocolHHTTrigOutput = ProtocolFunctionHHTBase + 2,

    // Function-Arm Orientation
    ProtocolFunctionArmOrientationBase = 50,
    ProtocolArmOrientation = ProtocolFunctionArmOrientationBase + 0,
    ProtocolStepCtrlMode = ProtocolFunctionArmOrientationBase + 1,

    // End effector
    CMD_ID_END_EFFECTOR_PARAM = 60,
    CMD_ID_END_EFFECTOR_LASER, //保留不实现，嵌入式那边也没有代码
    CMD_ID_END_EFFECTOR_SUCTION_CUP,
    CMD_ID_END_EFFECTOR_GRIPPER,
    CMD_ID_END_EFFECTOR_TYPE,

    CMD_ID_ROBOT_SERVO_ANGLE = 65, //保留不实现，嵌入式那边也没有代码
    CMD_ID_ROBOT_VEL_RATIO = 66,
    CMD_ID_ROBOT_LSPEED_RATIO = 67, //保留不实现，嵌入式那边也没有代码
    CMD_ID_ROBOT_CFG_AND_STATUS,
    CMD_ID_ROBOT_RT_STATUS,

    CMD_ID_GET_JOG_JOINT_PARAMS = 70,
    CMD_ID_GET_JOG_COORDINATE_PARAMS = 71,
    CMD_ID_GET_JOG_COMMON_PARAMS = 72,
    CMD_ID_GET_JOG_CMD,
    CMD_ID_GET_JOGL_CMD, //保留不实现，嵌入式那边也没有代码

    CMD_ID_PTP_JOINT_PARAMS = 80,
    CMD_ID_PTP_COORDINATE_PARAM,
    CMD_ID_PTP_JUMP_PARAMS,
    CMD_ID_PTP_COMMON_PARAMS,
    CMD_ID_PTP,

    // Function-CP
    ProtocolFunctionCPBase = 90,
    ProtocolCPParams = ProtocolFunctionCPBase + 0,
    ProtocolCPCmd = ProtocolFunctionCPBase + 1,
    ProtocolCPLECmd = ProtocolFunctionCPBase + 2,
    ProtocolCPRHeadHoldEnable = ProtocolFunctionCPBase + 3,
    ProtocolCPCommonParams = ProtocolFunctionCPBase + 4,
    ProtocolCP2Cmd = ProtocolFunctionCPBase + 5,

    // Function-ARC
    ProtocolFunctionARCBase = 100,
    ProtocolARCParams = ProtocolFunctionARCBase + 0,
    ProtocolARCCmd = ProtocolFunctionARCBase + 1,
    ProtocolCircleCmd = ProtocolFunctionARCBase + 2,
    ProtocolARCCommonParams = ProtocolFunctionARCBase + 3,

    CMD_ID_SET_WAIT_CMD = 110,

    // Function-TRIG
    ProtocolFunctionTRIGBase = 120,
    ProtocolTRIGCmd = ProtocolFunctionTRIGBase + 0,

    // Function-EIO
    ProtocolFunctionEIOBase = 130,
    ProtocolIOMultiplexing = ProtocolFunctionEIOBase + 0,
    ProtocolIODO = ProtocolFunctionEIOBase + 1,
    ProtocolIOPWM = ProtocolFunctionEIOBase + 2,
    ProtocolIODI = ProtocolFunctionEIOBase + 3,
    ProtocolIOADC = ProtocolFunctionEIOBase + 4,
//    ProtocolEMotor = ProtocolFunctionEIOBase + 5,
    ProtocolIICSensor = ProtocolFunctionEIOBase + 9,

    // Function-CAL
    ProtocolFunctionCALBase = 140,
    ProtocolAngleSensorStaticError = ProtocolFunctionCALBase + 0,
    ProtocolAngleSensorCoef = ProtocolFunctionCALBase + 1,
//    ProtocolBaseDecoderStaticError = ProtocolFunctionCALBase + 2,
//    ProtocolRobotParamsErrorCalibration = ProtocolFunctionCALBase + 4,
    ProtocolRobotUserCoordinateCal = ProtocolFunctionCALBase + 5,
    ProtocolRobotCalibrationParams = ProtocolFunctionCALBase + 6,
    ProtocolRobotDHParams = ProtocolFunctionCALBase + 7,

    //Function-LostStep
    CMD_ID_SET_LOST_STEP_VALUE = 170,
    CMD_ID_SET_LOST_STEP_CMD = 171,
    CMD_ID_COLLISION_CHECK = 172,

    //Fireware Update
    ProtocolFunctionFWUpdateBase = 190,
    ProtocolUpdatePrepare = ProtocolFunctionFWUpdateBase + 4,

    //Senseor Data
    ProtocolSensorData = 210,
    ProtocolMcuTemperature = ProtocolSensorData + 5,

    // Function-TEST
    ProtocolTESTBase = 220,
//    ProtocolUserParams = ProtocolTESTBase + 0,
    ProtocolPTPTime = ProtocolTESTBase + 1,
    ProtocolJointZeroCal = ProtocolTESTBase + 4,
    ProtocolMagneticCoderCalibStart = ProtocolTESTBase + 5,    //开始校准
    ProtocolMagneticCoderCalibStateGet = ProtocolTESTBase + 6,   //获取校准结束状态
    ProtocolMagneticCoderRawDataGet = ProtocolTESTBase + 7,     //获取校准的原始数据，uint16_t 格式，编码器读数
    ProtocolMakeFS = ProtocolTESTBase + 8,     //格式化文件系统，预留
    ProtocolMagneticCoderParamsConfig = ProtocolTESTBase + 9,

    // Function-ZDF
    ProtocolZDFBase = 230,

    CMD_ID_START_QUEUE = 240,
    CMD_ID_QUEUE_STOP,
    CMD_ID_QUEUE_FORCE_STOP,
    CMD_ID_QUEUE_START_DOWNLOAD,
    CMD_ID_QUEUE_STOP_DOWNLOAD,
    CMD_ID_QUEUE_CLEAR,
    CMD_ID_QUEUE_GET_INDEX,
    CMD_ID_QUEUE_GET_LEFT_SPACE,

    // Function-Debug
    ProtocolFunctionDebugBase = 250,
    ProtocolPcbaTest_AuxMcuToML_StartTest = ProtocolFunctionDebugBase + 0,
    ProtocolPcbaTest_AuxMcuToML_InquireResult = ProtocolFunctionDebugBase + 1,
    ProtocolPcbaTest_PcToAuxMcu_StartTest = ProtocolFunctionDebugBase + 2,
    ProtocolPcbaTest_PcToAuxMcu_InquireResult = ProtocolFunctionDebugBase + 3,
    ProtocolStepPlusDirCtrl = ProtocolFunctionDebugBase + 4,
    ProtocolVariableDebug = ProtocolFunctionDebugBase + 5,

    ProtocolMax = 256
};

#endif    // SIMULATION_MAGICIANLITE_SRC_RUNTIME_DCPS_CMD_ID_H_
