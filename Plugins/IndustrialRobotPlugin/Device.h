#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QMap>

#include "RobotStatus.h"
#include <QCoreApplication>
#include "FileControll.h"
#include "FileControllSmb.h"
#define USE_MOBDEBUG

#ifdef USE_MOBDEBUG
#include "Mobdebug.h"
#endif

class Module;
class FileControll;
class Device : public QObject
{
    Q_OBJECT
public:
    typedef void (Device::*APIFunction)(quint64, QJsonObject);

    explicit Device(QObject *parent = nullptr);

    void pSearchDobot(quint64 id, QJsonObject params);
    void pConnectDobot(quint64 id, QJsonObject params);
    void pDisconnectDobot(quint64 id = 0, QJsonObject params = QJsonObject());

    void sendCommand(QString api, quint64 id, QJsonObject params);

    void setWsPort(quint16 port);
    quint16 getWsPort();

    bool eventLoopSuccess(quint64 id, QEventLoop &loop);
    bool isConnected();

private:
    /* 3. 连接状态 */
    void pGetConnectionState(quint64 id, QJsonObject params);

    /* 万能函数 */
    void pSendPostCmd(quint64 id, QJsonObject params);
    void pSendGetCmd(quint64 id, QJsonObject params);
    void pReadFile(quint64 id, QJsonObject params);
    void pWriteFile(quint64 id, QJsonObject params);
    void pNewFile(quint64 id, QJsonObject params);
    void pNewFolder(quint64 id, QJsonObject params);
    void pDecodeBase64File(quint64 id, QJsonObject params);
    void pReadFolder(quint64 id, QJsonObject params);
    void pDeleteFolder(quint64 id, QJsonObject params);
    void pRenameFolder(quint64 id, QJsonObject params);
    void pCopyFolder(quint64 id, QJsonObject params);
    void pPathIsExist(quint64 id, QJsonObject params);
    void pCopyFileFromLocaleToSmb(quint64 id, QJsonObject params);

    /* 4. 基础数据交互 */
    void pExchange(quint64 id, QJsonObject params);

    void pGetDobotStatus(quint64 id, QJsonObject params);

    void pSetControlMode(quint64 id, QJsonObject params);
    void pGetControlMode(quint64 id, QJsonObject params);

    void pSetCoordinateMode(quint64 id, QJsonObject params);
    void pGetCoordinateMode(quint64 id, QJsonObject params);
    void pSetCoordinateMode2(quint64 id, QJsonObject params);

    void pSetJogMode(quint64 id, QJsonObject params);
    void pGetJogMode(quint64 id, QJsonObject params);
    void pSetJogMode2(quint64 id, QJsonObject params);

    void pSetToolCoordPattern(quint64 id, QJsonObject params);
    void pGetToolCoordPattern(quint64 id, QJsonObject params);

    void pSetUserCoordPattern(quint64 id, QJsonObject params);
    void pGetUserCoordPattern(quint64 id, QJsonObject params);

    void pGetAutoManualMode(quint64 id, QJsonObject params);

    void pGetJointCoordinate(quint64 id, QJsonObject params);
    void pGetCartesianCoordinate(quint64 id, QJsonObject params);
    void pGetAlarms(quint64 id, QJsonObject params);
    void pClearAlarms(quint64 id, QJsonObject params);
    void pClearAlarms2(quint64 id, QJsonObject params);
    void pGetInput(quint64 id, QJsonObject params);
    void pSetOutput(quint64 id, QJsonObject params);
    void pSetOutput2(quint64 id, QJsonObject params);
    void pGetOutput(quint64 id, QJsonObject params);

    void pSetExtendIO(quint64 id, QJsonObject params);
    void pGetExtendDI(quint64 id, QJsonObject params);
    void pGetExtendDO(quint64 id, QJsonObject params);

    void pGetisCollision(quint64 id, QJsonObject params);
    void pGetProjectState(quint64 id, QJsonObject params);

    /* 5.2 全局比例 */
    void pSetCommonSetting(quint64 id, QJsonObject params);
    void pGetCommonSetting(quint64 id, QJsonObject params);

    /* 5.3 示教参数 */
    void pSetTeachJoint(quint64 id, QJsonObject params);
    void pGetTeachJoint(quint64 id, QJsonObject params);

    void pSetTeachCoordinate(quint64 id, QJsonObject params);
    void pGetTeachCoordinate(quint64 id, QJsonObject params);

    void pSetTeachInch(quint64 id, QJsonObject params);
    void pGetTeachInch(quint64 id, QJsonObject params);

    /* 5.4 再现参数 */
    void pSetPlaybackJoint(quint64 id, QJsonObject params);
    void pGetPlaybackJoint(quint64 id, QJsonObject params);

    void pSetPlaybackCoordinate(quint64 id, QJsonObject params);
    void pGetPlaybackCoordinate(quint64 id, QJsonObject params);

    void pSetPlaybackArch(quint64 id, QJsonObject params);
    void pGetPlaybackArch(quint64 id, QJsonObject params);

    /* 5.5~5.6 坐标系 */
    void pSetToolCoordinate(quint64 id, QJsonObject params);
    void pGetToolCoordinate(quint64 id, QJsonObject params);
    void pSetToolCoordinate2(quint64 id, QJsonObject params);
    void pSetUserCoordinate(quint64 id, QJsonObject params);
    void pGetUserCoordinate(quint64 id, QJsonObject params);
    void pSetUserCoordinate2(quint64 id, QJsonObject params);

    /* 5.7 示教点列表 */
    void pSetTeachFileUpdate(quint64 id, QJsonObject params);
    void pDelTeachFileDelete(quint64 id, QJsonObject params);

    /*拖拽示教灵敏度设置/获取*/
    void pSetDragSensivity(quint64 id, QJsonObject params);
    void pGetDragSensivity(quint64 id, QJsonObject params);

    /* 6.1 标定 */
    void pSetCalibrateToolCoor(quint64 id, QJsonObject params);
    void pSetCalibrateToolPosition(quint64 id, QJsonObject params);
    void pSetCalibrateToolPose(quint64 id, QJsonObject params);
    void pSetCalibrateUserCoor(quint64 id, QJsonObject params);
    void pSetCalibrateLeftRightHand(quint64 id, QJsonObject params);
    void pSetCalibrateHome(quint64 id, QJsonObject params);
    void pSetCalibrateAxis(quint64 id, QJsonObject params);

#ifdef USE_MOBDEBUG
    /* 7.1~7.10 调试器 */
    void pGetDebuggerState(quint64 id, QJsonObject params);
    void pSetDebuggerStart(quint64 id, QJsonObject params);
    void pSetDebuggerRun(quint64 id, QJsonObject params);
    void pSetDebuggerStop(quint64 id, QJsonObject params);
    void pSetDebuggerSuspend(quint64 id, QJsonObject params);
    void pSetDebuggerStepIn(quint64 id, QJsonObject params);
    void pSetDebuggerStepOver(quint64 id, QJsonObject params);
    void pSetDebuggerSetb(quint64 id, QJsonObject params);
    void pSetDebuggerDelb(quint64 id, QJsonObject params);
    void pSetDebuggerDelAllb(quint64 id, QJsonObject params);
#endif

    /* 8. 面板模拟 */
    void pSetAutoManualMode(quint64 id, QJsonObject params);
    void pSetThreeSwitch(quint64 id, QJsonObject params);  
    void pSetEmergencyStop(quint64 id, QJsonObject params);
    void pGetEmergencyStop(quint64 id, QJsonObject params);
    void pSetJogCmd(quint64 id, QJsonObject params);

    /* 9. 网络设置 */
    void pSetEthernet(quint64 id, QJsonObject params);
    void pGetEthernet(quint64 id, QJsonObject params);
    void pSetAP(quint64 id, QJsonObject params);
    void pGetAP(quint64 id, QJsonObject params);

    /* 10.1 获取相机坐标 */
    void pSetVisionCoordinate(quint64 id, QJsonObject params);

    /* 10.2 运动到指定点 */
    void pSetMoveCmd(quint64 id, QJsonObject params);

    /* 11. 传送带跟踪 */
    void pGetCurrentEncoder(quint64 id, QJsonObject params);
    void pGetSensorEncoder(quint64 id, QJsonObject params);
    void pSetSensorEncoderListen(quint64 id, QJsonObject params);

    /* 12. 手动模式中全局速度比例 */
    void pSetManualSpeedRatio(quint64 id, QJsonObject params);
    void pGetManualSpeedRatio(quint64 id, QJsonObject params);

    /* 13. DI 模拟输入 */
    void pSetDIMode(quint64 id, QJsonObject params);
    void pGetDIMode(quint64 id, QJsonObject params);
    void pSetDIValue(quint64 id, QJsonObject params);

    /* 14. 通用IO模拟量 */
    void pSetGPIOAO(quint64 id, QJsonObject params);
    void pGetGPIOAO(quint64 id, QJsonObject params);
    void pSetGPIOAI(quint64 id, QJsonObject params);
    void pGetGPIOAI(quint64 id, QJsonObject params);

    /* 15. 协作机器人安全配置 */
    void pSetGeneralSafeSetting(quint64 id, QJsonObject params);
    void pSetLoadParams(quint64 id, QJsonObject params);
    void pGetLoadParams(quint64 id, QJsonObject params);
    void pSetAdvancedFunc(quint64 id, QJsonObject params);
    void pSetCollisionDetect(quint64 id, QJsonObject params);
    void pSetCollisionDetectLevel(quint64 id, QJsonObject params);
    void pSetCollisionDetectResumeType(quint64 id, QJsonObject params);
    void pGetCollisionDetect(quint64 id, QJsonObject params);
    /*对pSetCollisionDetectXXX接口的补充，万能接口，可以设置任意字段内容，而不用每个字段都要重新写接口*/
    void pSetCollisionDetectParam(quint64 id, QJsonObject params);

    void pSetSafeParams(quint64 id, QJsonObject params);

    /* 16. 电子皮肤 */
    void pSetElecSkinEnable(quint64 id, QJsonObject params);
    void pSetElecSkinReset(quint64 id, QJsonObject params);
    void pGetElecSkinReset(quint64 id, QJsonObject params);
    void pSetElecSkinParams(quint64 id, QJsonObject params);
    void pGetElecSkinParams(quint64 id, QJsonObject params);

    /* 17. 回零 */
    void pSetGoHomeCmd(quint64 id, QJsonObject params);
    void pGetGoHomeCmd(quint64 id, QJsonObject params);

    /* 18. 开关抱闸 */
    void pSetAxisJointBrake(quint64 id, QJsonObject params);
    void pGetAxisJointBrake(quint64 id, QJsonObject params);

    /* 19. 轨迹复现 */
    void pSetRecurrentTrack(quint64 id, QJsonObject params);
    void pSetDebugReTrace(quint64 id, QJsonObject params);
    void pGetDebugReTrace(quint64 id, QJsonObject params);

    /* 20. 协作机器人自动识别 */
    void pSetAutoIdentify(quint64 id, QJsonObject params);

    /* 21.协作机器人安装位置 */
    void pSetInstallPosture(quint64 id, QJsonObject params);

    /* 22.协作机器人实轴、虚轴 */
    void pSetSimulatedAxies(quint64 id, QJsonObject params);

    /*协作控制器获取当前轴数*/
    void pGetPropertyAxis(quint64 id, QJsonObject params);

    /*获取当前设备类型，是工业还是教育的*/
    void pGetBusinessType(quint64 id, QJsonObject params);

    /* 23.协作机器人末端执行器 */
    void pSetRobottiqGripperEnable(quint64 id, QJsonObject params);
    void pGetRobottiqGripperEnable(quint64 id, QJsonObject params);
    void pSetRobottiqGripperEnableK(quint64 id, QJsonObject params);
    void pSetHitbotGripperEnable(quint64 id, QJsonObject params);
    void pGetHitbotGripperEnable(quint64 id, QJsonObject params);
    void pSetHitbotGripper(quint64 id, QJsonObject params);
    void pSetDHGripperEnable(quint64 id, QJsonObject params);
    void pGetDHGripperEnable(quint64 id, QJsonObject params);
    void pSetDHGripper(quint64 id, QJsonObject params);
    void pGetDHGripper(quint64 id, QJsonObject params);

    /* 24.协作机器人 6维力传感器接口 */
    void pSetRobotiqSixForce(quint64 id, QJsonObject params);
    void pGetRobotiqSixForce(quint64 id, QJsonObject params);

    /* 24.2 模拟量接口 */
    void pSetEndAI(quint64 id, QJsonObject params);
    void pGetEndAI(quint64 id, QJsonObject params);

    /* 25.协作控制器上下电 */
    void pSetPowerControl(quint64 id, QJsonObject params);
    void pGetPowerControl(quint64 id, QJsonObject params);

    /* 27.协作机器人RUNTO */
    void pSetCRRunTo(quint64 id, QJsonObject params);
    void pGetCRRunTo(quint64 id, QJsonObject params);

    /* 28. 版本号 */
    void pGetVersion(quint64 id, QJsonObject params);

    /* 30.公共接口 */
    void pPublicInterface(quint64 id, QJsonObject params);

    /* 31.MagicianPro特殊接口 */
    void pSetPanelDragTeach(quint64 id, QJsonObject params);
    void pSetDriverBasicParam(quint64 id, QJsonObject params);
    void pGetDriverBasicParam(quint64 id, QJsonObject params);
    void pSetDriverParam(quint64 id, QJsonObject params);
    void pGetDriverParam(quint64 id, QJsonObject params);
    void pSetFunctionloadParams(quint64 id, QJsonObject params);
    void pGetAlarmController(quint64 id, QJsonObject params);
    void pGetAlarmServo(quint64 id, QJsonObject params);
    void pSetFunctionIoctrl(quint64 id, QJsonObject params);
    void pGetFunctionIoctrl(quint64 id, QJsonObject params);
    void pSetFunctionRemoteControl(quint64 id, QJsonObject params);
    void pGetFunctionRemoteControl(quint64 id, QJsonObject params);
    void pSetUserParam(quint64 id, QJsonObject params);
    void pGetUserParam(quint64 id, QJsonObject params);
    void pSetPropertiesHardwareInfo(quint64 id, QJsonObject params);
    void pGetPropertiesHardwareInfo(quint64 id, QJsonObject params);
    void pSetSystemTime(quint64 id, QJsonObject params);
    void pGetSystemTime(quint64 id, QJsonObject params);
    void pSetFirmwareUpgrade(quint64 id, QJsonObject params);
    void pSetWorkTimeRec(quint64 id, QJsonObject params);
    void pGetWorkTimeRec(quint64 id, QJsonObject params);
    void pSetReboot(quint64 id, QJsonObject params);
    void pResetCollision(quint64 id, QJsonObject params);
    void pSetScriptRun(quint64 id, QJsonObject params);
    void pSetScriptStop(quint64 id, QJsonObject params);
    void pSetWiFiParams(quint64 id, QJsonObject params);
    void pSetFunctionPostureCalc(quint64 id, QJsonObject params);
    void pSetTruemotion(quint64 id, QJsonObject params);
    void pGetTruemotion(quint64 id, QJsonObject params);
    void pSetPallet(quint64 id, QJsonObject params);
    void pGetPallet(quint64 id, QJsonObject params);
    void pGetFunctionScriptParams(quint64 id, QJsonObject params);
    void pCheckSamba(quint64 id, QJsonObject params);
    void pOpenSamba(quint64 id, QJsonObject params);
    void pOpenNetUse(quint64 id, QJsonObject params);
    void pGetDeviceName(quint64 id, QJsonObject params);
    void pRestartComputer(quint64 id, QJsonObject params);
    void pOpenFireWall(quint64 id, QJsonObject params);

    /* 32.扩展轴 */
    void pAuxSetSwitch(quint64 id, QJsonObject params);
    void pAuxGetSwitch(quint64 id, QJsonObject params);
    void pAuxSetJogCmd(quint64 id, QJsonObject params);
    void pAuxSetTeachJoint(quint64 id, QJsonObject params);
    void pAuxGetTeachJoint(quint64 id, QJsonObject params);
    void pAuxSetPlaybackJoint(quint64 id, QJsonObject params);
    void pAuxGetPlaybackJoint(quint64 id, QJsonObject params);
    void pAuxSetStructure(quint64 id, QJsonObject params);
    void pAuxGetStructure(quint64 id, QJsonObject params);

    /**33.CR接口***********************************************/
    //快捷键设置/查询接口,用于第六轴末端的按钮控制
    void pSetHotkey(quint64 id, QJsonObject params);
    void pGetHotkey(quint64 id, QJsonObject params);
    /*设置/获取初始姿态*/
    void pSetCustomPoint(quint64 id, QJsonObject params);
    void pGetCustomPoint(quint64 id, QJsonObject params);
    /*设置/获取 运动到打包姿态与初始姿态*/
    void pSetMoveJCmd(quint64 id, QJsonObject params);
    void pGetMoveJCmd(quint64 id, QJsonObject params);
    /*获取打包姿态接口（打包姿态）*/
    void pSetPackPoint(quint64 id, QJsonObject params);
    void pGetPackPoint(quint64 id, QJsonObject params);
    /*设置/获取jump速度参数*/
    void pSetJumpStructure(quint64 id, QJsonObject params);
    void pGetJumpStructure(quint64 id, QJsonObject params);
    /*获取所有速度参数默认值*/
    void pSetSpeedDefault(quint64 id, QJsonObject params);
    void pGetSpeedDefault(quint64 id, QJsonObject params);
    /*通知控制器更新Dobot+的api*/
    void pSetApiUpdate(quint64 id, QJsonObject params);
    /*全局变量的设置*/
    void pSetGlobalVar(quint64 id, QJsonObject params);
    void pGetGlobalVar(quint64 id, QJsonObject params);
    /*获取安装姿态*/
    void pSetCalcInstall(quint64 id, QJsonObject params);
    /*设置/获取当前安装状态*/
    void pSetFunctionInstall(quint64 id, QJsonObject params);
    void pGetFunctionInstall(quint64 id, QJsonObject params);
    /*用于控制器确定是否判断示教器的硬件I/O*/
    void pSetHardwareEnable(quint64 id, QJsonObject params);
    /**exchange*************************************************/
    void pGetDragPlayback(quint64 id, QJsonObject params);
    void pGetSkinCollison(quint64 id, QJsonObject params);
    void pGetSkinValue(quint64 id, QJsonObject params);
    void pGetControlParams(quint64 id, QJsonObject params);
    void pGetJointCurrent(quint64 id, QJsonObject params);
    void pGetJointVoltage(quint64 id, QJsonObject params);
    void pGetJointTemp(quint64 id, QJsonObject params);
    void pGetRDNCoordinate(quint64 id, QJsonObject params);
    void pGetGPIOAIField(quint64 id, QJsonObject params);
    void pGetEndAIField(quint64 id, QJsonObject params);

    /*轨迹复现高级设置*/
    void pSetRetraceParams(quint64 id, QJsonObject params);
    void pGetRetraceParams(quint64 id, QJsonObject params);
    /*开始/停止拖拽示教*/
    void pSetDragAndTeach(quint64 id, QJsonObject params);
    /*获取拖拽示教点列表*/
    void pGetDragTeachFileList(quint64 id, QJsonObject params);
    void pDeleteDragTeachFile(quint64 id, QJsonObject params);

    /*正解、逆解*/
    void pSetForwardCal(quint64 id, QJsonObject params);
    void pSetInverseCal(quint64 id, QJsonObject params);

    /*安全IO配置*/
    void pGetSafeSignal(quint64 id, QJsonObject params);
    void pSetSafeSignal(quint64 id, QJsonObject params);

    /*远程Modbus配置*/
    void pGetModbusctrl(quint64 id, QJsonObject params);
    void pSetModbusctrl(quint64 id, QJsonObject params);

    /*添加搜索ip*/
    void pAddSearchIP(quint64 id, QJsonObject params);

    /*IO数据读取及下发接口*/
    void pSetIONote(quint64 id, QJsonObject params);
    void pGetIONote(quint64 id, QJsonObject params);

signals:
    void onReplyMessage_signal(quint64 id, QJsonValue value = QJsonValue());
    void onNotifyMessage_signal(quint64 id, QJsonObject obj = QJsonObject());
    void onDebuggerNotify_signal(QString msg);
    void onErrorOccured_signal(quint64 id, int errCode, QString errStr = "");

private:
    RobotStatus m_status;
    Module *m_module;
    FileControll *m_fileControll;//the default object is FileControllSmb
    FileControll *m_pFileCtrl;//the real object
    FileControllSmb *m_pFileCtrlSmb;//the real object
    QMap<QString, //portName
        FileControll*> m_portFCSmb;
#ifdef USE_MOBDEBUG
   Mobdebug *m_modebug;
#endif
    bool m_isConnected;
    QMap<QString, QString> m_selfExchangeFunMap;

    QMap<QString, APIFunction> m_FuncMap;
    QMap<quint64, QJsonObject> m_requestMap;
    QMap<quint64, QEventLoop*> m_eventloopMap;

    quint16 m_wsPort;

    void _apiFunctionInit();
    void _selfExchangeFunMapInit();

    void _sendStatus(quint64 id, const RobotStatus status, QString api);
    void _getStatus(quint64 id, QString api);

    inline bool checkIntValue(const QJsonObject &obj, const QString &value);
    inline bool checkFloatValue(const QJsonObject &obj, const QString &value);
    inline bool checkBoolValue(const QJsonObject &obj, const QString &value);
    inline bool checkStringValue(const QJsonObject &obj, const QString &value);
    inline bool checkObjectValue(const QJsonObject &obj, const QString &value);
    inline bool checkArrayValue(const QJsonObject &obj, const QString &value);

    bool hanleSelfFunction(const QString url, const QString api, const quint64 id, const QJsonObject &obj);

    void handleSelfExchange(QString api, quint64 id, QJsonObject obj);

private slots:
    void onModebugMessage_slot(QString msg);
    void onSearch_slot(QJsonArray res, quint64 id);
    void onModebugFinish_slot(quint64 id);
    void onErrorOccured_slot(quint64 id, int errCode, QString errStr);

    void onReplyMessage_slot(QJsonValue value, QString url, quint64 id, QString api);

    void onreadPendingClientMsg_slot(quint64 id, QString msg);
    void onreadPending1stCursorMsg_slot(quint64 id, QString msg);
    void onreadPending2ndCursorMsg_slot(quint64 id, QString msg);
    void onreadPendingSpecialMsg_slot(quint64 id, QString msg);
    void onreadBlockHighlightId_slot(quint64 id, QString msg);
};

#endif // DEVICE_H
