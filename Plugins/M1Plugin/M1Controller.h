#ifndef M1CONTROLLER_H
#define M1CONTROLLER_H

#include <QObject>
#include <QMap>
#include <QJsonObject>
#include <QTimer>
#include "M1Packet.h"
#include "M1WaitForFinish.h"

class DeviceInfo {
public:
    DeviceInfo();

    int id;
    bool isConnected;
    QString status;

    QString portName;
    QString SN;
    QString name;
};

class M1Controller : public QObject
{
    Q_OBJECT
public:
    explicit M1Controller(QObject *parent = nullptr);

    QMap<QString, DeviceInfo> m_DevInfoMap;
    QMap<QString, quint16> m_portNameClientMap;
    QMap<M1WaitForFinish*, int> m_waitFinishMap;

    bool isDeviceConnected(QString portName);
    void updataDeviceState();
    void disConnectDevices(quint16 port);

    QJsonObject getCmdResultObj(const M1Packet &packet, const int res = 0, QJsonValue data = QJsonValue());
    QJsonObject getConnectResultObj(const M1Packet &packet, const int res = 0);

    /* 1.2 指令超时 */
    QJsonObject pSetCmdTimeout(const M1Packet &packet);

    /* 1.3 连接/断开连接 */
    QJsonObject pSearchDobot(const M1Packet &packet);

    QJsonObject pConnectDobot(const M1Packet &packet);
    QJsonObject pDisConnectDobot(const M1Packet &packet);
    void pDisConnectDobot(const QString &portName);

    /* 1.4 指令队列控制 */
    QJsonObject pSetQueuedCmd(const M1Packet &packet);
    QJsonObject pQueuedCmdStart(const M1Packet &packet);
    QJsonObject pQueuedCmdStop(const M1Packet &packet);
    QJsonObject pQueuedCmdClear(const M1Packet &packet);
    QJsonObject pSetQueuedCmdForceStopExec(const M1Packet &packet);
    QJsonObject pSetQueuedCmdStartDownload(const M1Packet &packet);
    QJsonObject pSetQueuedCmdStopDownload(const M1Packet &packet);
    QJsonObject pGetQueuedCmdLeftSpace(const M1Packet &packet);

    /* 1.5 设备信息 */
    QJsonObject pSetDeviceSN(const M1Packet &packet);
    QJsonObject pGetDeviceSN(const M1Packet &packet);

    QJsonObject pSetDeviceName(const M1Packet &packet);
    QJsonObject pGetDeviceName(const M1Packet &packet);

    QJsonObject pSetDeviceWithL(const M1Packet &packet);  //滑轨
    QJsonObject pGetDeviceWithL(const M1Packet &packet);

    QJsonObject pGetDeviceVersion(const M1Packet &packet);

    QJsonObject pGetHardwareVersion(const M1Packet &packet);

    /* 1.6 实时位姿 */
    QJsonObject pGetPose(const M1Packet &packet);
    QJsonObject pResetPose(const M1Packet &packet);

    /* 1.7 报警功能 */
    QJsonObject pGetAlarmsState(const M1Packet &packet);
    QJsonObject pClearAllAlarmsState(const M1Packet &packet);

    /* 1.8 回零功能 */
    QJsonObject pSetHOMECmd(const M1Packet &packet);

    QJsonObject pSetHOMEParams(const M1Packet &packet);
    QJsonObject pGetHOMEParams(const M1Packet &packet);

    /* 1.9 HHT 手持示教器 */
    QJsonObject pSetHHTTrigMode(const M1Packet &packet);
    QJsonObject pGetHHTTrigMode(const M1Packet &packet);

    QJsonObject pSetHHTTrigOutputEnabled(const M1Packet &packet);
    QJsonObject pGetHHTTrigOutputEnabled(const M1Packet &packet);

    QJsonObject pGetHHTTrigOutput(const M1Packet &packet);

    /* 1.10 末端执行器 */
    QJsonObject pSetEndEffectorParams(const M1Packet &packet);    //1.设置末端坐标偏移量
    QJsonObject pGetEndEffectorParams(const M1Packet &packet);    //2.获取末端坐标偏移量

    QJsonObject pSetEndEffectorLaser(const M1Packet &packet);     //3.设置激光状态
    QJsonObject pGetEndEffectorLaser(const M1Packet &packet);     //4.获取激光状态

    QJsonObject pSetEndEffectorSuctionCup(const M1Packet &packet);//5.设置气泵状态
    QJsonObject pGetEndEffectorSuctionCup(const M1Packet &packet);//6.获取气泵状态

    QJsonObject pSetEndEffectorGripper(const M1Packet &packet);   //7.设置夹爪状态
    QJsonObject pGetEndEffectorGripper(const M1Packet &packet);   //8.获取夹爪状态

    /* 1.11 JOG 功能 */
    QJsonObject pSetJOGJointParams(const M1Packet &packet);
    QJsonObject pGetJOGJointParams(const M1Packet &packet);

    QJsonObject pSetJOGCoordinateParams(const M1Packet &packet);
    QJsonObject pGetJOGCoordinateParams(const M1Packet &packet);

    QJsonObject pSetJOGLParams(const M1Packet &packet);
    QJsonObject pGetJOGLParams(const M1Packet &packet);

    QJsonObject pSetJOGCommonParams(const M1Packet &packet);
    QJsonObject pGetJOGCommonParams(const M1Packet &packet);

    QJsonObject pSetJOGCmd(const M1Packet &packet);

    /* 1.12 PTP 功能 */
    QJsonObject pSetPTPJointParams(const M1Packet &packet);       //1.设置各关节坐标轴的速度和加速度
    QJsonObject pGetPTPJointParams(const M1Packet &packet);       //2.获取各关节坐标轴的速度和加速度

    QJsonObject pSetPTPCoordinateParams(const M1Packet &packet);  //3.设置各笛卡尔坐标轴的速度和加速度
    QJsonObject pGetPTPCoordinateParams(const M1Packet &packet);  //4.获取各笛卡尔坐标轴的速度和加速度

    QJsonObject pSetPTPJumpParams(const M1Packet &packet);        //5.设置JUMP模式下抬升高度和最大抬升高度
    QJsonObject pGetPTPJumpParams(const M1Packet &packet);        //6.获取JUMP模式下抬升高度和最大抬升高度

    QJsonObject pSetPTPJump2Params(const M1Packet &packet);       //7.设置JUMP模式下扩展参数
    QJsonObject pGetPTPJump2Params(const M1Packet &packet);       //8.获取JUMP模式下扩展参数

//    QJsonObject pSetPTPLParams(const M1Packet &packet);           //9.设置滑轨速度和加速度
//    QJsonObject pGetPTPLParams(const M1Packet &packet);           //10.获取滑轨速度和加速度

    QJsonObject pSetPTPCommonParams(const M1Packet &packet);      //11.设置运动的速度百分比和加速度百分比
    QJsonObject pGetPTPCommonParams(const M1Packet &packet);      //12.获取运动的速度百分比和加速度百分比

    QJsonObject pSetPTPCmd(const M1Packet &packet);               //13.执行 PTP 指令
    QJsonObject pSetPTPPOCmd(const M1Packet &packet);             //14.执行带 I/O 控制的 PTP 指令
//    QJsonObject pSetPTPWithLCmd(const M1Packet &packet);          //15.执行带滑轨的 PTP 指令
//    QJsonObject pSetPTPPOWithLCmd(const M1Packet &packet);        //16.执行带 I/O 控制 和 滑轨 的 PTP 指令

    /* 1.13 CP 功能 */
    QJsonObject pSetCPParams(const M1Packet &packet);
    QJsonObject pGetCPParams(const M1Packet &packet);

    QJsonObject pSetCPCmd(const M1Packet &packet);
    QJsonObject pSetCPLECmd(const M1Packet &packet);

    /* 1.14 ARC 功能 */
    QJsonObject pSetARCParams(const M1Packet &packet);
    QJsonObject pGetARCParams(const M1Packet &packet);

    QJsonObject pSetARCCmd(const M1Packet &packet);
    QJsonObject pSetCircleCmd(const M1Packet &packet);

    /* 1.16 WAIT 功能 */
    QJsonObject pSetWAITCmd(const M1Packet &packet);
    QJsonObject pSetTRIGCmd(const M1Packet &packet);

//    /* 1.17 EIO 功能 */
//    QJsonObject pSetIOMultiplexing(const M1Packet &packet);       //1.设置 I/O 复用
//    QJsonObject pGetIOMultiplexing(const M1Packet &packet);       //2.读取 I/O 复用

    QJsonObject pSetIODO(const M1Packet &packet);                 //3.设置 I/O 输出电平
    QJsonObject pGetIODO(const M1Packet &packet);                 //4.读取 I/O 输出电平

    QJsonObject pGetIODI(const M1Packet &packet);                 //7.读取 I/O 输入电平
    QJsonObject pGetIOADC(const M1Packet &packet);                //8.读取 A/D 输入

    /* 1.18 机械臂方向 */
    QJsonObject pSetArmOrientation(const M1Packet &packet);
    QJsonObject pGetArmOrientation(const M1Packet &packet);

    /* 1.19 固件相关 */
    QJsonObject pSetFirmwareLanConfig(const M1Packet &packet);
    QJsonObject pGetFirmwareLanConfig(const M1Packet &packet);

    /* 1.20 重启*/
    QJsonObject pFirmwareReboot(const M1Packet &packet);

    /* 1.21 队列index */
    QJsonObject pGetQueuedCmdCurrentIndex(const M1Packet &packet);

    /* 1.22 */
    QJsonObject pSetExcitCmd(const M1Packet &packet);

    /* 1.23 */
    QJsonObject pSetSafeMode(const M1Packet &packet);
    QJsonObject pGetSafeMode(const M1Packet &packet);
    QJsonObject pSetExcitMode(const M1Packet &packet);
    QJsonObject pGetExcitMode(const M1Packet &packet);
    QJsonObject pSetCollisionThreshold(const M1Packet &packet);
    QJsonObject pGetCollisionThreshold(const M1Packet &packet);
    QJsonObject pSetBasicDynamicParams(const M1Packet &packet);
    QJsonObject pGetBasicDynamicParams(const M1Packet &packet);
    QJsonObject pSetPlayLoad(const M1Packet &packet);
    QJsonObject pGetPlayLoad(const M1Packet &packet);
    QJsonObject pGetTrajectory(const M1Packet &packet);
    QJsonObject pGetCollisionCount(const M1Packet &packet);
    QJsonObject pSetSafeStrategy(const M1Packet &packet);
    QJsonObject pGetSafeStrategy(const M1Packet &packet);
    QJsonObject pSetSafeGuardMode(const M1Packet &packet);
    QJsonObject pGetSafeGuardMode(const M1Packet &packet);
    QJsonObject pSetSafeGuardStatus(const M1Packet &packet);
    QJsonObject pGetSafeGuardStatus(const M1Packet &packet);


signals:
    void deviceDisconnected_signal(QString portName, quint16 wsPort);
    void sendFinishPacket_signal(QJsonObject resObj);

public slots:
    void closeAllDevice_slot();
    void waitForFinish_slot(int res);

private:
    bool isCheckState;
    QTimer *checkStateTimer;
    int m_syncTimeout;

private slots:
    void handleCheckTimeout_slot();
};

#endif // M1CONTROLLER_H
