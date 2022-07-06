/****************************************Copyright(c)*****************************************************
**                            Shenzhen Yuejiang Technology Co., LTD.
**
**                                 http://www.dobot.cc
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           ParaComm.h
** Latest modified Date:
** Latest Version:      V1.0.0
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created by:          Deng Xinjin
** Created date:        2019-3-12
** Version:             V1.0.0
** Descriptions:
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#ifndef __PARACOMM_H
#define __PARACOMM_H

#include "stdint.h"

#define FLASH_SIZE 512          //所选MCU的FLASH容量大小(单位为K)

#if FLASH_SIZE<256
    #define SECTOR_SIZE           1024    //字节
#else
    #define SECTOR_SIZE           2048    //字节
#endif

/*
flash_start   data_start   app_start                     app_end          flash_end
0x8000000     0x8008000    0x800C000                     0x804A800        0x8080000
^             ^            ^                             ^                ^
|             |            |                             |                |
|             |            |                             |                |
+-------------------------------------------------------------------------+
|             |            |                             |                |
|     loader  | data       | app                         | file_system    |
|     32k     | 16k        | 250k                        | 214k           |
+-------------+------------+-----------------------------+----------------+
*/


#define SIZE_OF_LAODER_SECTION          (32*1024)
#define SIZE_OF_DATA_SECTION            (16*1024)
#define SIZE_OF_APP_SECTION             (250*1024)
#define SIZE_OF_FS_SECTION              (214*1024)
#define MAX_SIZE_OF_APP_LEN             SIZE_OF_APP_SECTION
#define MIN_SIZE_OF_APP_LEN             (16 * 1024)
#define SIZE_OF_ENCODER_TABLE           (32*1024)
#define ADDR_FLASH_START                (0x8000000)
#define ADDR_DATA_START                 (ADDR_FLASH_START + SIZE_OF_LAODER_SECTION)     //0x8008000
#define ADDR_APP_START                  (ADDR_DATA_START + SIZE_OF_DATA_SECTION)        //0x800C000
#define ADDR_FS_START                   (ADDR_APP_START + SIZE_OF_APP_SECTION)          //0x804A800
/*保留：32K*3=96K*/
#define ADDR_ENCODER_ZERO_TABLE_START   (ADDR_FS_START+SIZE_OF_ENCODER_TABLE)           //0x8052800
#define ADDR_ENCODER_ONE_TABLE_START    (ADDR_ENCODER_ZERO_TABLE_START+SIZE_OF_ENCODER_TABLE)
#define ADDR_ENCODER_TWO_TABLE_START    (ADDR_ENCODER_ONE_TABLE_START +SIZE_OF_ENCODER_TABLE)
/*底座水平度校准起始地址*/
#define ADDR_BASELEVELING_START         0x8068800
/*编码器线性度校准（84KB）*/
/*底座校准的起始地址：0x806A800，大小14KB*/
/*大臂校准的起始地址：0x806E000，大小14KB*/
/*小臂校准的起始地址：0x8071800，大小14KB*/
/*底座输入轴校准的起始地址：0x8075000，大小14KB*/
/*临时数据的开始区域: 0x8078800, 大小为28K*/
#define ADDR_FLASH_END                  (0x8080000)

//
//  General Math
//
#define TWO_PI                      6.283185307179586476925286766559f
#define DIV_TWO_PI                  0.15915494309189533576888376337251f
#define DIVIDE_BY_65536             0.0000152587890625f     // 1/65536
#define DIV_2_POWER_24              0.00000005960464477539f // 1/(2^24)
#define TWO_POWER_22                4194304.0f
#define DIV_60                      0.01666666666666666666666666666667f
#define TWO_DIVIDE_BY_SQRT3         1.1547005383792515290182975610039f
#define TWO_POWER_16                65536
#define DIVIDE_BY_8192              0.0001220703125f
#define DIVIDE_BY_1024              0.0009765625

enum {
    MOTOR_STOP_MODE,
    MOTOR_PLUSDIR_MODE,
    MOTOR_SPEED_MODE,
    MOTOR_POSITION_MODE,
    MOTOR_OPENLOOP_MODE,
    HAND_TEACH_MODE
};

enum{ 
    SUCTIONCUP = 1,
    GRIPPER,
    PEN
};


#define HIGH              1
#define LOW               0

//
////通用条件编译宏
//
#define REPEATPOSACCURACY       1       //1:修复浮点运算精度导致的累加误差，0：试产版本暂不修复此问题（测试稳定后再加）
#define MCU_GD                  1       //MCU芯片选择，0:Stm32f103Vet,1:GD(选择GD，可以兼容Stm32f103Vet)
#define FOURAXIS_TEST           1       //第四轴控制
#define EXFLASH_WR              0       //建表参数保存到内部Flash
#define STEP_OPENCTRL           1       //1：步进开环控制 0：步进闭环控制
/*TMC2208芯片驱动引脚配置*/
#define TMC2208_UART_RUN        1       //0：关闭该功能；1：IO模拟串口
/*烧写不同的程序时需配置*/
#define GPIOA_DEINIT            0        //0：Bootloader+App  1：App
/*校准参数上传*/
#define DEBUG_UPDATACALPARAM    1
//
////校准相关的宏定义
//
/*编码器校准类型*/
#define ENCODERLINEARITYCALIBRATION         1        //1:编码器线性度校准
#define ENCODERSTEPTABLE                    0        //1:编码器整步校准
#define LOOKUP                              0        //如果底座有双编码器，则只校准底座，否则不校准
/*机械臂参数校准相关的宏定义*/
#define ROT_MOTORANGLEERR_ZOOM              10      //底座零点偏移的角度误差范围在正负10度
#define L_MOTORANGLEERR_ZOOM                15      //大臂零点偏移的角度误差范围在正负15度
#define R_MOTORANGLEERR_ZOOM                15      //小臂零点偏移的角度误差范围在正负15度
#define BIGARM_LINKERR_ZOOM                 10      //大臂臂长误差范围在正负10mm
#define SMALLARM_LINKERR_ZOOM               10      //小臂臂长误差范围在正负10mm
#define LEVING_DEBUG                        1       //上传底座水平度校准模式的数据
/*编码器线性度校准运行范围的宏定义（以实测为准）*/
#define ROT_ENCCAL_ZOOM                     73            //底座电机编码器线性度校准运行范围(输入轴)
#define ROT_ENCCAL_ZOOM_OUTPUTAXIS          278            //底座电机编码器线性度校准运行范围（输出轴）
#define L_ENCCAL_ZOOM                       98            //大臂电机编码器线性度校准运行范围
#define R_ENCCAL_ZOOM                       102           //小臂电机编码器线性度校准运行范围
/*编码器整步校准运行范围的宏定义（以实测为准）*/
#define ROT_STEPCAL_ZOOM                    270            //底座电机整步校准运行范围
#define L_STEPCAL_ZOOM                      90            //大臂电机整步校准运行范围
#define R_STEPCAL_ZOOM                      90           //小臂电机整步校准运行范围
#define STEPCAL_MAXERR                      6           //整步校准允许的最大误差
/*机械臂理论臂长参数(单位mm)*/
#define BIGARM_LENGTH                       150     
#define SMALLARM_LENGTH                     150
#define END_LENGTH                          65      //45+20
//
////电机控制相关的宏定义
//
/*电机轴*/
#define AXIS_NUM               (4)
#define MOTOR_ROT_OUTPUTAXIS    0
#define MOTOR_L_OUTPUTAXIS      1
#define MOTOR_R_OUTPUTAXIS      2
#define MOTOR_ROT_INPUTAXIS     3
/*编码器相关*/
#define EncType_AS5600          1
#define EncType_MA730           2  
#define ENC_RESOLUTION          1        //分辩率的位数,1:14位，0：12位
/*步进电机步距角*/
#define STEP_TYPE               1        //1:1.8度步距角  0：0.9度步距角
#if STEP_TYPE
    #define STEP_ANLGE          1.8
#else
    #define STEP_ANLGE          0.9
#endif 
#define STEP_ROT_ANGLE          1       //底座电机选择 0：1.8度电机(默认)， 1:0.9度电机
#define TMC2208_CTRLMODE        0       //0:只运行于低速超静音模式，1：根据设定的速度阀值可切换至高速防抖模式

/*运算相关宏*/
#define DIVIDE_BY_8192          0.0001220703125f    
#define SERVODUTY               6.666667    //2000/290=6.89655    //2000/300=6.6666666666666666666666666666667
#if ENC_RESOLUTION
    #define PlUSTOANGLE             0.02197265625           //360/16384=0.02197265625
    #define OUTAXIS_TO_INPUTAXIS    0.9765625               //16000/16384 = 0.9765625 
    #define CIRCLE_DIV_ENCRES       0.1953125               //3200/16384=0.1953125
    #define PLUS_S_RATIO            48.828125                //50*5*3200/16384 = 48.828125
    #define RPM_RATIO               0.018310546875           //5*60/16384= 0.018310546875 
#else
    #define PlUSTOANGLE             0.087890625             //360/4096=0.087890625
    #define OUTAXIS_TO_INPUTAXIS    3.90625                 //16000/4096 = 3.90625 
    #define CIRCLE_DIV_ENCRES       0.78125                 //3200/4096 = 0.78125
    #define PLUS_S_RATIO            195.3125                //50*5*3200/4096 = 195.3125
    #define RPM_RATIO               0.0732421875                
#endif
//
////运动控制模块相关宏定义
//
#define CP_VELOCITY     100

//
////打印相关的调试宏定义（可修改）
//
#define DEBUG_PTPSTOP           0
#define DEBUG_JOINTANGLE        0
#define DEBUG_SPEEDACCPRINTF    0
#define DEBUG_SPDFDB            0       //测试速度时可以设置为1
#define DEBUG_COMMAND           0
#define DEBUG_PWMDMA            0
#define DEBUG_OFFLINEHOME       0
#define DEBUG_ALARMINFO         0
/*脉冲计数调试宏*/
#define DEBUG_PLUS              0       //0,关闭，1，开启

#endif

