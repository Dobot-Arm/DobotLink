#include "Device.h"
#include "Module.h"
#include <QTimer>
#include <QVariant>
#include <QDebug>
#include "DError/DError.h"
#include <QSettings>
#include <QJsonObject>
#include "IndustrialRobotPlugin.h"
const QString BASE_PORT = "22000";
#ifndef Q_OS_ANDROID
const QString DefaultIPAddress = "192.168.1.6";
#else
const QString DefaultIPAddress = "192.168.2.6";
#endif

QMap<QString,int> Device::m_portFCSmb;
Device::Device(QObject *parent) : QObject(parent)
{
    m_isConnected = false;

    _apiFunctionInit();
    _selfExchangeFunMapInit();

#ifdef USE_MOBDEBUG
    m_modebug = new Mobdebug(this);
    connect(m_modebug, &Mobdebug::onModebugMessage_signal,
            this, &Device::onModebugMessage_slot);
    connect(m_modebug, &Mobdebug::onFinish_signal,
            this, &Device::onModebugFinish_slot);
    connect(m_modebug, &Mobdebug::onErrorOccured_signal,
            this, &Device::onErrorOccured_slot);

    connect(m_modebug, &Mobdebug::readPendingClientMsg_signal,
            this, &Device::onreadPendingClientMsg_slot);
    connect(m_modebug, &Mobdebug::readPending1stCursorMsg_signal,
            this, &Device::onreadPending1stCursorMsg_slot);
    connect(m_modebug, &Mobdebug::readPending2ndCursorMsg_signal,
            this, &Device::onreadPending2ndCursorMsg_slot);
    connect(m_modebug, &Mobdebug::readPendingSpecialMsg_signal,
            this, &Device::onreadPendingSpecialMsg_slot);
    connect(m_modebug, &Mobdebug::readBlockHighlightId_signal,
            this, &Device::onreadBlockHighlightId_slot);

#endif

    m_module = new Module(BASE_PORT, this);
    connect(m_module, &Module::onSearch_signal,
            this, &Device::onSearch_slot);
    connect(m_module, &Module::onReceiveData_signal,
            this, &Device::onReplyMessage_slot);
    connect(m_module, &Module::onErrorOccured_signal,
            this, &Device::onErrorOccured_slot);
    connect(m_module, &Module::onGetConnectionStateOccuredError_signal,[this](quint64 id){
        Q_UNUSED(id);
        this->m_isConnected = false;

        m_module->setIpAddress("");
        m_fileControll->setIpAddress("");
        m_pFileCtrl->setIpAddress("");
        m_pFileCtrlSmb->setIpAddress("");

#ifdef USE_MOBDEBUG
        m_modebug->udpClose();
#endif
    });

    m_pFileCtrl = new FileControll("", this);
    m_pFileCtrlSmb = new FileControllSmb("", this);
    m_fileControll = m_pFileCtrlSmb;
}

void Device::pConnectDobot(quint64 id, QJsonObject params)
{
    checkStringValue(params, "portName");

    QString portName = params.value("portName").toString();

    if (m_isConnected) {
        if (portName == m_module->ip()){
            emit onReplyMessage_signal(id, true);
        } else {
            emit onErrorOccured_signal(id, ERROR_INDUSTRY_HAS_CONNECTED);
        }
        return;
    }

    if (!portName.isEmpty()) {
        m_module->setIpAddress(portName);
    } else {
#ifdef __arm__
        m_module->setIpAddress("127.0.0.1");
#else
        m_module->setIpAddress(DefaultIPAddress);
#endif
    }

    QObject* pErr = new QObject();
    connect(m_module, &Module::onErrorOccured_signal,pErr,
            [this,id,pErr](quint64 reqId, int errCode, QString errStr) {

        //如果不是自己，不要执行
        if (id != reqId) return ;
        //执行完毕后，释放对象以及所有信号槽，防止重复接收
        pErr->deleteLater();

        if (errStr.size() == 0) {
            errStr = DError::getErrorMessage(errCode);
        }

        qDebug() << errCode << errStr;
        emit onReplyMessage_signal(id, false);
    });

    QObject* pRecv = new QObject();
    connect(m_module, &Module::onReceiveData_signal,pRecv,
            [this, id,pRecv,portName](QJsonValue value, QString url, quint64 reqId, QString api) {
        Q_UNUSED(url);

        //如果不是自己，不要执行
        if (api != "ConnectDobot" || id!=reqId) {
            qDebug() << "invalid http data：" << value;
            return;
        }
        //执行完毕后，释放对象以及所有信号槽，防止重复接收
        pRecv->deleteLater();

        // 回包有问题
        QJsonObject obj = value.toObject();
        if (!obj.contains("value") ||
            obj.value("value").toString() != "connected") {
            m_module->setIpAddress("");
            emit onErrorOccured_signal(id, ERROR_DEVICE_LOST_CONNECTION);
            return;
        }

#ifdef USE_MOBDEBUG
        // 绑定端口
        if (!m_modebug->udpOpen(portName)) {
            m_module->setIpAddress("");
            emit onErrorOccured_signal(id, ERROR_MOBDEBUG_UDP_BIND_FAILED);
            return;
        }
#endif

        // 配置file对象的ip
        QString strPortNameTmp;
        if (!portName.isEmpty()) {
            strPortNameTmp = portName;
        } else {
#ifndef __arm__
            strPortNameTmp = DefaultIPAddress;
#endif
        }

        if (!strPortNameTmp.isEmpty())
        {
            if (m_portFCSmb.contains(strPortNameTmp))
            {
                if (m_portFCSmb[strPortNameTmp] == SMBType::SMBT_V2_SMB)
                {
                    m_fileControll = m_pFileCtrlSmb;
                }
                else
                {
                    m_fileControll = m_pFileCtrl;
                }
            }
            else
            {
                m_fileControll = m_pFileCtrl;
            }
            m_pFileCtrl->setIpAddress(strPortNameTmp);
            m_pFileCtrlSmb->setIpAddress(strPortNameTmp);
        }

        m_isConnected = true;
        emit onReplyMessage_signal(id, true);
    });

    m_module->sendGetRequest("/connection/state", id, "ConnectDobot");
}

void Device::pDisconnectDobot(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    if (!m_isConnected) {
        emit onReplyMessage_signal(id, true);
        return;
    }

    m_module->setIpAddress("");
    m_fileControll->setIpAddress("");
    m_pFileCtrl->setIpAddress("");
    m_pFileCtrlSmb->setIpAddress("");

#ifdef USE_MOBDEBUG
    m_modebug->udpClose();
#endif

    m_isConnected = false;
    emit onReplyMessage_signal(id, true);
}

void Device::sendCommand(QString api, quint64 id, QJsonObject params)
{
    APIFunction function = m_FuncMap.value(api);
    if (function) {
        if (!api.contains("SearchDobot") &&
            !api.contains("ConnectDobot") &&
            !api.contains("DisconnectDobot") &&
            !api.contains("OpenNetUse") &&
            !api.contains("OpenFireWall") &&
            !api.contains("CheckSamba") &&
            !api.contains("OpenSamba") &&
            !api.contains("RestartComputer") &&
            !api.contains("AddSearchIP") &&
            !m_isConnected)
        {
            emit onErrorOccured_signal(id, ERROR_INDUSTRY_NOT_CONNECTED);
        } else
        {
            m_requestMap.insert(id, params);
            (this->*function)(id, params);
        }
    } else {
        qDebug() << "No matching function with: " << api << " params:" << params;
        emit onErrorOccured_signal(id, ERROR_INDUSTRY_API);
    }
}

void Device::setWsPort(quint16 port)
{
    m_wsPort = port;
}

quint16 Device::getWsPort()
{
    return  m_wsPort;
}

bool Device::isConnected()
{
    return m_isConnected;
}

void Device::_apiFunctionInit()
{
    /* 万能函数 */
    m_FuncMap.insert("SendPostCmd", &Device::pSendPostCmd);
    m_FuncMap.insert("SendGetCmd", &Device::pSendGetCmd);
    m_FuncMap.insert("ReadFile", &Device::pReadFile);
    m_FuncMap.insert("ReadFolder", &Device::pReadFolder);
    m_FuncMap.insert("WriteFile", &Device::pWriteFile);
    m_FuncMap.insert("NewFile", &Device::pNewFile);
    m_FuncMap.insert("NewFolder", &Device::pNewFolder);
    m_FuncMap.insert("DecodeBase64File", &Device::pDecodeBase64File);
    m_FuncMap.insert("DeleteFolder", &Device::pDeleteFolder);
    m_FuncMap.insert("RenameFolder", &Device::pRenameFolder);
    m_FuncMap.insert("CopyFolder", &Device::pCopyFolder);
    m_FuncMap.insert("PathIsExist", &Device::pPathIsExist);
    m_FuncMap.insert("CopyFileFromLocaleToSmb", &Device::pCopyFileFromLocaleToSmb);

    /* 3. 连接状态 */
    m_FuncMap.insert("SearchDobot", &Device::pSearchDobot);
    m_FuncMap.insert("ConnectDobot", &Device::pConnectDobot);
    m_FuncMap.insert("DisconnectDobot", &Device::pDisconnectDobot);
    m_FuncMap.insert("GetConnectionState", &Device::pGetConnectionState);

    /* 4. 基础数据交互 */
    m_FuncMap.insert("Exchange", &Device::pExchange);

    m_FuncMap.insert("GetDobotStatus", &Device::pGetDobotStatus);

    m_FuncMap.insert("SetControlMode", &Device::pSetControlMode);
    m_FuncMap.insert("GetControlMode", &Device::pGetControlMode);

    m_FuncMap.insert("SetCoordinateMode", &Device::pSetCoordinateMode);
    m_FuncMap.insert("GetCoordinateMode", &Device::pGetCoordinateMode);
    m_FuncMap.insert("SetCoordinateMode2", &Device::pSetCoordinateMode2);

    m_FuncMap.insert("SetJogMode", &Device::pSetJogMode);
    m_FuncMap.insert("GetJogMode", &Device::pGetJogMode);
    m_FuncMap.insert("SetJogMode2", &Device::pSetJogMode2);

    m_FuncMap.insert("SetToolCoordPattern", &Device::pSetToolCoordPattern);
    m_FuncMap.insert("GetToolCoordPattern", &Device::pGetToolCoordPattern);

    m_FuncMap.insert("SetUserCoordPattern", &Device::pSetUserCoordPattern);
    m_FuncMap.insert("GetUserCoordPattern", &Device::pGetUserCoordPattern);

    m_FuncMap.insert("GetAutoManualMode", &Device::pGetAutoManualMode);

    m_FuncMap.insert("GetJointCoordinate", &Device::pGetJointCoordinate);
    m_FuncMap.insert("GetCartesianCoordinate", &Device::pGetCartesianCoordinate);
    m_FuncMap.insert("GetAlarms", &Device::pGetAlarms);
    m_FuncMap.insert("ClearAlarms", &Device::pClearAlarms);
    m_FuncMap.insert("ClearAlarms2", &Device::pClearAlarms2);
    m_FuncMap.insert("GetInput", &Device::pGetInput);
    m_FuncMap.insert("SetOutput", &Device::pSetOutput);
    m_FuncMap.insert("SetOutput2", &Device::pSetOutput2);
    m_FuncMap.insert("GetOutput", &Device::pGetOutput);

    m_FuncMap.insert("SetExtendIO", &Device::pSetExtendIO);
    m_FuncMap.insert("GetExtendDI", &Device::pGetExtendDI);
    m_FuncMap.insert("GetExtendDO", &Device::pGetExtendDO);

    m_FuncMap.insert("GetisCollision", &Device::pGetisCollision);
    m_FuncMap.insert("GetProjectState", &Device::pGetProjectState);

    /* 5. 参数设置 */
    /* 5.2 全局比例 */
    m_FuncMap.insert("SetCommonSetting", &Device::pSetCommonSetting);
    m_FuncMap.insert("GetCommonSetting", &Device::pGetCommonSetting);

    /* 5.3 示教参数 */
    m_FuncMap.insert("SetTeachJoint", &Device::pSetTeachJoint);
    m_FuncMap.insert("GetTeachJoint", &Device::pGetTeachJoint);

    m_FuncMap.insert("SetTeachCoordinate", &Device::pSetTeachCoordinate);
    m_FuncMap.insert("GetTeachCoordinate", &Device::pGetTeachCoordinate);
    m_FuncMap.insert("SetTeachInch", &Device::pSetTeachInch);
    m_FuncMap.insert("GetTeachInch", &Device::pGetTeachInch);

    /* 5.4 再现参数 */
    m_FuncMap.insert("SetPlaybackJoint", &Device::pSetPlaybackJoint);
    m_FuncMap.insert("GetPlaybackJoint", &Device::pGetPlaybackJoint);

    m_FuncMap.insert("SetPlaybackCoordinate", &Device::pSetPlaybackCoordinate);
    m_FuncMap.insert("GetPlaybackCoordinate", &Device::pGetPlaybackCoordinate);

    m_FuncMap.insert("SetPlaybackArch", &Device::pSetPlaybackArch);
    m_FuncMap.insert("GetPlaybackArch", &Device::pGetPlaybackArch);

    /* 5.5~5.6 坐标系 */
    m_FuncMap.insert("SetToolCoordinate", &Device::pSetToolCoordinate);
    m_FuncMap.insert("SetToolCoordinate2", &Device::pSetToolCoordinate2);
    m_FuncMap.insert("GetToolCoordinate", &Device::pGetToolCoordinate);
    m_FuncMap.insert("SetUserCoordinate", &Device::pSetUserCoordinate);
    m_FuncMap.insert("SetUserCoordinate2", &Device::pSetUserCoordinate2);
    m_FuncMap.insert("GetUserCoordinate", &Device::pGetUserCoordinate);

    /* 5.7 示教点列表 */
    m_FuncMap.insert("SetTeachFileUpdate", &Device::pSetTeachFileUpdate);
    m_FuncMap.insert("DelTeachFileDelete", &Device::pDelTeachFileDelete);

    m_FuncMap.insert("SetDragSensivity", &Device::pSetDragSensivity);
    m_FuncMap.insert("GetDragSensivity", &Device::pGetDragSensivity);

    /* 6.1 标定 */
    m_FuncMap.insert("SetCalibrateToolCoor", &Device::pSetCalibrateToolCoor);
    m_FuncMap.insert("SetCalibrateToolPosition", &Device::pSetCalibrateToolPosition);
    m_FuncMap.insert("SetCalibrateToolPose", &Device::pSetCalibrateToolPose);
    m_FuncMap.insert("SetCalibrateUserCoor", &Device::pSetCalibrateUserCoor);
    m_FuncMap.insert("SetCalibrateLeftRightHand", &Device::pSetCalibrateLeftRightHand);
    m_FuncMap.insert("SetCalibrateHome", &Device::pSetCalibrateHome);
    m_FuncMap.insert("SetCalibrateAxis", &Device::pSetCalibrateAxis);

#ifdef USE_MOBDEBUG
    /* 7.1~7.10 调试器 */
    m_FuncMap.insert("GetDebuggerState", &Device::pGetDebuggerState);
    m_FuncMap.insert("SetDebuggerStart", &Device::pSetDebuggerStart);
    m_FuncMap.insert("SetDebuggerStop", &Device::pSetDebuggerStop);
    m_FuncMap.insert("SetDebuggerRun", &Device::pSetDebuggerRun);
    m_FuncMap.insert("SetDebuggerSuspend", &Device::pSetDebuggerSuspend);
    m_FuncMap.insert("SetDebuggerStepIn", &Device::pSetDebuggerStepIn);
    m_FuncMap.insert("SetDebuggerStepOver", &Device::pSetDebuggerStepOver);
    m_FuncMap.insert("SetDebuggerSetb", &Device::pSetDebuggerSetb);
    m_FuncMap.insert("SetDebuggerDelb", &Device::pSetDebuggerDelb);
    m_FuncMap.insert("SetDebuggerDelAllb", &Device::pSetDebuggerDelAllb);
#endif

    /* 8. 面板模拟 */
    m_FuncMap.insert("SetThreeSwitch", &Device::pSetThreeSwitch);
    m_FuncMap.insert("SetAutoManualMode", &Device::pSetAutoManualMode);
    m_FuncMap.insert("SetEmergencyStop", &Device::pSetEmergencyStop);
    m_FuncMap.insert("GetEmergencyStop", &Device::pGetEmergencyStop);
    m_FuncMap.insert("SetJogCmd", &Device::pSetJogCmd);

    /* 9. 网络设置 */
    m_FuncMap.insert("SetEthernet", &Device::pSetEthernet);
    m_FuncMap.insert("GetEthernet", &Device::pGetEthernet);
    m_FuncMap.insert("SetAP", &Device::pSetAP);
    m_FuncMap.insert("GetAP", &Device::pGetAP);

    /* 10.1 获取相机坐标 */
    m_FuncMap.insert("SetVisionCoordinate", &Device::pSetVisionCoordinate);

    /* 10.2 运动到指定点 */
    m_FuncMap.insert("SetMoveCmd", &Device::pSetMoveCmd);

    /* 11. 传送带跟踪 */
    m_FuncMap.insert("GetCurrentEncoder", &Device::pGetCurrentEncoder);
    m_FuncMap.insert("GetSensorEncoder", &Device::pGetSensorEncoder);
    m_FuncMap.insert("SetSensorEncoderListen", &Device::pSetSensorEncoderListen);

    /* 12. 手动模式中全局速度比例 */
    m_FuncMap.insert("SetManualSpeedRatio", &Device::pSetManualSpeedRatio);
    m_FuncMap.insert("GetManualSpeedRatio", &Device::pGetManualSpeedRatio);

    /* 13. DI 模拟输入 */
    m_FuncMap.insert("SetDIMode", &Device::pSetDIMode);
    m_FuncMap.insert("GetDIMode", &Device::pGetDIMode);
    m_FuncMap.insert("SetDIValue", &Device::pSetDIValue);

    /* 14. 通用IO模拟量 */
    m_FuncMap.insert("SetGPIOAO", &Device::pSetGPIOAO);
    m_FuncMap.insert("GetGPIOAO", &Device::pGetGPIOAO);
    m_FuncMap.insert("SetGPIOAI", &Device::pSetGPIOAI);
    m_FuncMap.insert("GetGPIOAI", &Device::pGetGPIOAI);

    /* 15. 协作机器人安全配置 */
    m_FuncMap.insert("SetGeneralSafeSetting", &Device::pSetGeneralSafeSetting);
    m_FuncMap.insert("SetLoadParams", &Device::pSetLoadParams);
    m_FuncMap.insert("GetLoadParams", &Device::pGetLoadParams);
    m_FuncMap.insert("SetAdvancedFunc", &Device::pSetAdvancedFunc);
    m_FuncMap.insert("SetCollisionDetect", &Device::pSetCollisionDetect);
    m_FuncMap.insert("SetCollisionDetectLevel", &Device::pSetCollisionDetectLevel);
    m_FuncMap.insert("SetCollisionDetectResumeType", &Device::pSetCollisionDetectResumeType);
    m_FuncMap.insert("GetCollisionDetect", &Device::pGetCollisionDetect);
    m_FuncMap.insert("SetCollisionDetectParam", &Device::pSetCollisionDetectParam);
    m_FuncMap.insert("SetSafeParams", &Device::pSetSafeParams);

    /* 16. 电子皮肤 */
    m_FuncMap.insert("SetElecSkinEnable", &Device::pSetElecSkinEnable);
    m_FuncMap.insert("SetElecSkinReset", &Device::pSetElecSkinReset);
    m_FuncMap.insert("GetElecSkinReset", &Device::pGetElecSkinReset);
    m_FuncMap.insert("SetElecSkinParams", &Device::pSetElecSkinParams);
    m_FuncMap.insert("GetElecSkinParams", &Device::pGetElecSkinParams);

    /* 17. 回零 */
    m_FuncMap.insert("SetGoHomeCmd", &Device::pSetGoHomeCmd);
    m_FuncMap.insert("GetGoHomeCmd", &Device::pGetGoHomeCmd);

    /* 18. 开关抱闸 */
    m_FuncMap.insert("SetAxisJointBrake", &Device::pSetAxisJointBrake);
    m_FuncMap.insert("GetAxisJointBrake", &Device::pGetAxisJointBrake);

    /* 19. 轨迹复现 */
    m_FuncMap.insert("SetRecurrentTrack", &Device::pSetRecurrentTrack);
    m_FuncMap.insert("SetDebugReTrace", &Device::pSetDebugReTrace);
    m_FuncMap.insert("GetDebugReTrace", &Device::pGetDebugReTrace);

    /* 20. 协作机器人自动识别 */
    m_FuncMap.insert("SetAutoIdentify", &Device::pSetAutoIdentify);

    /* 21.协作机器人安装位置 */
    m_FuncMap.insert("SetInstallPosture", &Device::pSetInstallPosture);

    /* 22.协作机器人实轴、虚轴 */
    m_FuncMap.insert("SetSimulatedAxies", &Device::pSetSimulatedAxies);
    m_FuncMap.insert("GetPropertyAxis", &Device::pGetPropertyAxis);
    m_FuncMap.insert("GetBusinessType", &Device::pGetBusinessType);

    /* 23.协作机器人末端执行器 */
    m_FuncMap.insert("SetRobottiqGripperEnable", &Device::pSetRobottiqGripperEnable);
    m_FuncMap.insert("GetRobottiqGripperEnable", &Device::pGetRobottiqGripperEnable);
    m_FuncMap.insert("SetRobottiqGripperEnableK", &Device::pSetRobottiqGripperEnableK);
    m_FuncMap.insert("SetHitbotGripperEnable", &Device::pSetHitbotGripperEnable);
    m_FuncMap.insert("GetHitbotGripperEnable", &Device::pGetHitbotGripperEnable);
    m_FuncMap.insert("SetHitbotGripper", &Device::pSetHitbotGripper);
    m_FuncMap.insert("SetDHGripperEnable", &Device::pSetDHGripperEnable);
    m_FuncMap.insert("GetDHGripperEnable", &Device::pGetDHGripperEnable);
    m_FuncMap.insert("SetDHGripper", &Device::pSetDHGripper);
    m_FuncMap.insert("GetDHGripper", &Device::pGetDHGripper);

    /* 24.协作机器人 6维力传感器接口 */
    m_FuncMap.insert("SetRobotiqSixForce", &Device::pSetRobotiqSixForce);
    m_FuncMap.insert("SetRobotiqSixForce", &Device::pSetRobotiqSixForce);
    m_FuncMap.insert("GetRobotiqSixForce", &Device::pGetRobotiqSixForce);

    /* 24.2 模拟量接口 */
    m_FuncMap.insert("SetEndAI", &Device::pSetEndAI);
    m_FuncMap.insert("GetEndAI", &Device::pGetEndAI);

    /* 25.协作控制器上下电 */
    m_FuncMap.insert("SetPowerControl", &Device::pSetPowerControl);
    m_FuncMap.insert("GetPowerControl", &Device::pGetPowerControl);

    /* 27.协作机器人RUNTO */
    m_FuncMap.insert("SetCRRunTo", &Device::pSetCRRunTo);
    m_FuncMap.insert("GetCRRunTo", &Device::pGetCRRunTo);

    /* 28. 版本号 */
    m_FuncMap.insert("GetVersion", &Device::pGetVersion);

    /* 29. 公共接口*/
    m_FuncMap.insert("PublicInterface", &Device::pPublicInterface);

    /* 30. MagicianPro特殊接口*/
    m_FuncMap.insert("SetPanelDragTeach", &Device::pSetPanelDragTeach);
    m_FuncMap.insert("SetDriverBasicParam", &Device::pSetDriverBasicParam);
    m_FuncMap.insert("GetDriverBasicParam", &Device::pGetDriverBasicParam);
    m_FuncMap.insert("SetDriverParam", &Device::pSetDriverParam);
    m_FuncMap.insert("GetDrivercParam", &Device::pGetDriverParam);
    m_FuncMap.insert("GetAlarmController", &Device::pGetAlarmController);
    m_FuncMap.insert("GetAlarmServo", &Device::pGetAlarmServo);
    m_FuncMap.insert("SetFunctionIoctrl", &Device::pSetFunctionIoctrl);
    m_FuncMap.insert("GetFunctionIoctrl", &Device::pGetFunctionIoctrl);
    m_FuncMap.insert("SetFunctionRemoteControl", &Device::pSetFunctionRemoteControl);
    m_FuncMap.insert("GetFunctionRemoteControl", &Device::pGetFunctionRemoteControl);
    m_FuncMap.insert("SetUserParam", &Device::pSetUserParam);
    m_FuncMap.insert("GetUserParam", &Device::pGetUserParam);
    m_FuncMap.insert("SetPropertiesHardwareInfo", &Device::pSetPropertiesHardwareInfo);
    m_FuncMap.insert("GetPropertiesHardwareInfo", &Device::pGetPropertiesHardwareInfo);
    m_FuncMap.insert("SetSystemTime", &Device::pSetSystemTime);
    m_FuncMap.insert("GetSystemTime", &Device::pGetSystemTime);
    m_FuncMap.insert("SetFirmwareUpgrade", &Device::pSetFirmwareUpgrade);
    m_FuncMap.insert("SetWorkTimeRec", &Device::pSetWorkTimeRec);
    m_FuncMap.insert("GetWorkTimeRec", &Device::pGetWorkTimeRec);
    m_FuncMap.insert("SetReboot", &Device::pSetReboot);
    m_FuncMap.insert("ResetCollision", &Device::pResetCollision);
    m_FuncMap.insert("SetScriptRun", &Device::pSetScriptRun);
    m_FuncMap.insert("SetScriptStop", &Device::pSetScriptStop);
    m_FuncMap.insert("SetWiFiParams", &Device::pSetWiFiParams);
    m_FuncMap.insert("SetFunctionPostureCalc", &Device::pSetFunctionPostureCalc);
    m_FuncMap.insert("SetTruemotion", &Device::pSetTruemotion);
    m_FuncMap.insert("GetTruemotion", &Device::pGetTruemotion);
    m_FuncMap.insert("SetPallet", &Device::pSetPallet);
    m_FuncMap.insert("GetPallet", &Device::pGetPallet);
    m_FuncMap.insert("GetFunctionScriptParams", &Device::pGetFunctionScriptParams);
    m_FuncMap.insert("CheckSamba", &Device::pCheckSamba);
    m_FuncMap.insert("OpenSamba", &Device::pOpenSamba);
    m_FuncMap.insert("OpenNetUse", &Device::pOpenNetUse);
    m_FuncMap.insert("GetDeviceName", &Device::pGetDeviceName);
    m_FuncMap.insert("RestartComputer", &Device::pRestartComputer);
    m_FuncMap.insert("OpenFireWall", &Device::pOpenFireWall);

    /* 31. 扩展轴*/
    m_FuncMap.insert("AuxSetSwitch", &Device::pAuxSetSwitch);
    m_FuncMap.insert("AuxGetSwitch", &Device::pAuxGetSwitch);
    m_FuncMap.insert("AuxSetJogCmd", &Device::pAuxSetJogCmd);
    m_FuncMap.insert("AuxSetTeachJoint", &Device::pAuxSetTeachJoint);
    m_FuncMap.insert("AuxGetTeachJoint", &Device::pAuxGetTeachJoint);
    m_FuncMap.insert("AuxSetPlaybackJoint", &Device::pAuxSetPlaybackJoint);
    m_FuncMap.insert("AuxGetPlaybackJoint", &Device::pAuxGetPlaybackJoint);
    m_FuncMap.insert("AuxSetStructure", &Device::pAuxSetStructure);
    m_FuncMap.insert("AuxGetStructure", &Device::pAuxGetStructure);

    /**33.CR接口***********************************************/
    m_FuncMap.insert("SetHotkey", &Device::pSetHotkey);
    m_FuncMap.insert("GetHotkey", &Device::pGetHotkey);
    m_FuncMap.insert("SetCustomPoint", &Device::pSetCustomPoint);
    m_FuncMap.insert("GetCustomPoint", &Device::pGetCustomPoint);
    m_FuncMap.insert("SetMoveJCmd", &Device::pSetMoveJCmd);
    m_FuncMap.insert("GetMoveJCmd", &Device::pGetMoveJCmd);
    m_FuncMap.insert("SetPackPoint", &Device::pSetPackPoint);
    m_FuncMap.insert("GetPackPoint", &Device::pGetPackPoint);
    m_FuncMap.insert("SetJumpStructure", &Device::pSetJumpStructure);
    m_FuncMap.insert("GetJumpStructure", &Device::pGetJumpStructure);
    m_FuncMap.insert("SetSpeedDefault", &Device::pSetSpeedDefault);
    m_FuncMap.insert("GetSpeedDefault", &Device::pGetSpeedDefault);
    m_FuncMap.insert("SetApiUpdate", &Device::pSetApiUpdate);
    m_FuncMap.insert("SetGlobalVar", &Device::pSetGlobalVar);
    m_FuncMap.insert("GetGlobalVar", &Device::pGetGlobalVar);
    m_FuncMap.insert("SetCalcInstall", &Device::pSetCalcInstall);
    m_FuncMap.insert("SetFunctionInstall", &Device::pSetFunctionInstall);
    m_FuncMap.insert("GetFunctionInstall", &Device::pGetFunctionInstall);
    m_FuncMap.insert("SetHardwareEnable", &Device::pSetHardwareEnable);
    m_FuncMap.insert("GetDragPlayback", &Device::pGetDragPlayback);
    m_FuncMap.insert("GetSkinCollison", &Device::pGetSkinCollison);

    m_FuncMap.insert("GetSkinValue", &Device::pGetSkinValue);
    m_FuncMap.insert("GetControlParams", &Device::pGetControlParams);
    m_FuncMap.insert("GetJointCurrent", &Device::pGetJointCurrent);
    m_FuncMap.insert("GetJointVoltage", &Device::pGetJointVoltage);
    m_FuncMap.insert("GetJointTemp", &Device::pGetJointTemp);
    m_FuncMap.insert("GetRDNCoordinate", &Device::pGetRDNCoordinate);
    m_FuncMap.insert("GetGPIOAIField", &Device::pGetGPIOAIField);
    m_FuncMap.insert("GetEndAIField", &Device::pGetEndAIField);

    m_FuncMap.insert("SetRetraceParams", &Device::pSetRetraceParams);
    m_FuncMap.insert("GetRetraceParams", &Device::pGetRetraceParams);
    m_FuncMap.insert("SetDragAndTeach", &Device::pSetDragAndTeach);
    m_FuncMap.insert("GetDragTeachFileList", &Device::pGetDragTeachFileList);
    m_FuncMap.insert("DeleteDragTeachFile", &Device::pDeleteDragTeachFile);

    m_FuncMap.insert("SetForwardCal", &Device::pSetForwardCal);
    m_FuncMap.insert("SetInverseCal", &Device::pSetInverseCal);

    m_FuncMap.insert("GetSafeSignal", &Device::pGetSafeSignal);
    m_FuncMap.insert("SetSafeSignal", &Device::pSetSafeSignal);

    m_FuncMap.insert("GetModbusctrl", &Device::pGetModbusctrl);
    m_FuncMap.insert("SetModbusctrl", &Device::pSetModbusctrl);

    m_FuncMap.insert("AddSearchIP", &Device::pAddSearchIP);

    m_FuncMap.insert("SetIONote", &Device::pSetIONote);
    m_FuncMap.insert("GetIONote", &Device::pGetIONote);

    m_FuncMap.insert("GetCabinetType", &Device::pGetCabinetType);
    m_FuncMap.insert("GetCCBoxVoltage", &Device::pGetCCBoxVoltage);
    m_FuncMap.insert("SetCCBoxVoltage", &Device::pSetCCBoxVoltage);
}

void Device::_selfExchangeFunMapInit()
{
    m_selfExchangeFunMap.insert("SetControlMode", "controlMode");
    m_selfExchangeFunMap.insert("GetControlMode", "controlMode");
    m_selfExchangeFunMap.insert("SetCoordinateMode", "coordinate");
    m_selfExchangeFunMap.insert("GetCoordinateMode", "coordinate");
    m_selfExchangeFunMap.insert("SetJogMode", "jogMode");
    m_selfExchangeFunMap.insert("GetJogMode", "jogMode");
    m_selfExchangeFunMap.insert("SetToolCoordPattern", "toolCoordinate");
    m_selfExchangeFunMap.insert("GetToolCoordPattern", "toolCoordinate");
    m_selfExchangeFunMap.insert("SetUserCoordPattern", "userCoordinate");
    m_selfExchangeFunMap.insert("GetUserCoordPattern", "userCoordinate");
    m_selfExchangeFunMap.insert("GetAutoManualMode", "autoManual");
    m_selfExchangeFunMap.insert("GetJointCoordinate", "jointCoordinate");
    m_selfExchangeFunMap.insert("GetCartesianCoordinate", "cartesianCoordinate");
    m_selfExchangeFunMap.insert("ClearAlarms", "alarms");
    m_selfExchangeFunMap.insert("GetAlarms", "alarms");
    m_selfExchangeFunMap.insert("GetInput", "inputs");
    m_selfExchangeFunMap.insert("SetOutput", "outputs");
    m_selfExchangeFunMap.insert("GetOutput", "outputs");
    m_selfExchangeFunMap.insert("GetExtendDI", "extendDI");
    m_selfExchangeFunMap.insert("GetExtendDO", "extendDO");
    m_selfExchangeFunMap.insert("GetisCollision", "isCollision");
    m_selfExchangeFunMap.insert("GetProjectState", "prjState");
    m_selfExchangeFunMap.insert("GetDragPlayback", "dragPlayback");
    m_selfExchangeFunMap.insert("GetSkinCollison", "skinCollison");
    m_selfExchangeFunMap.insert("GetSkinValue", "skinValue");
    m_selfExchangeFunMap.insert("GetControlParams", "controlParams");
    m_selfExchangeFunMap.insert("GetJointCurrent", "jointCurrent");
    m_selfExchangeFunMap.insert("GetJointVoltage", "jointVoltage");
    m_selfExchangeFunMap.insert("GetJointTemp", "jointTemp");
    m_selfExchangeFunMap.insert("GetRDNCoordinate", "rdnCoordinate");
    m_selfExchangeFunMap.insert("GetGPIOAIField", "gpioAI");
    m_selfExchangeFunMap.insert("GetEndAIField", "endAI");
}

void Device::pSearchDobot(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendSearchRequest("/properties/controllerType", id);
}

//![万能函数]
void Device::pSendPostCmd(quint64 id, QJsonObject params)
{
    checkStringValue(params, "url");
    checkObjectValue(params, "body");

    QString portName = params.value("portName").toString();
    QString url = params.value("url").toString();
    QJsonObject body = params.value("body").toObject();

    m_module->sendPostRequest(url, body, id);
}

void Device::pSendGetCmd(quint64 id, QJsonObject params)
{
    checkStringValue(params, "url");

    QString url = params.value("url").toString();
    m_module->sendGetRequest(url, id);
}

void Device::pReadFile(quint64 id, QJsonObject params)
{
    checkStringValue(params, "fileName");

    QString fileName = params.value("fileName").toString();

    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, fileName);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                QString str(data);
                if (!value.isNull()){
                    //说明是json文件
                    emit onReplyMessage_signal(id, value);
                } else {
                    emit onReplyMessage_signal(id, str);
                }
            } else {
                qDebug() << "read file failed.";
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pWriteFile(quint64 id, QJsonObject params)
{
    checkStringValue(params, "url");
    checkStringValue(params, "fileName");

    QString url = params.value("url").toString();
    QString fileName = params.value("fileName").toString();
    if (checkObjectValue(params, "content")) {
        QJsonObject contentObj = params.value("content").toObject();
        QObject *obj = new QObject(this);
        m_fileControll->writeFile(id, fileName, contentObj);
        connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
            if (id != c_id) {
                return ;
            } else {
                obj->deleteLater();
                m_requestMap.remove(id);
                if (code == NOERROR) {
                    emit onReplyMessage_signal(id, true);
                } else {
                    emit onErrorOccured_signal(id, code);
                }
            }

        });
    } else if (checkStringValue(params, "content")) {
        QString contentValue = params.value("content").toString();
        QObject *obj = new QObject(this);
        m_fileControll->writeFile(id, fileName, contentValue);
        connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
            if (id != c_id) {
                return ;
            } else {
                obj->deleteLater();
                m_requestMap.remove(id);
                if (code == NOERROR) {
                    emit onReplyMessage_signal(id, true);
                } else {
                    emit onErrorOccured_signal(id, code);
                }
            }
        });
    }
}

void Device::pNewFile(quint64 id, QJsonObject params)
{
    checkStringValue(params, "fileName");
    checkStringValue(params, "url");
    QString fileName = params.value("fileName").toString();
    QString url = params.value("url").toString();
    if (checkObjectValue(params, "content")) {
        QJsonObject contentObj = params.value("content").toObject();
        QObject *obj = new QObject(this);
        m_fileControll->newFile(id, fileName, contentObj);
        connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
            if (id != c_id) {
                return ;
            } else {
                obj->deleteLater();
                m_requestMap.remove(id);
                if (code == NOERROR) {
                    emit onReplyMessage_signal(id, true);
                } else {
                    emit onErrorOccured_signal(id, code);
                }
            }

        });
    } else if (checkStringValue(params, "content")) {
        QString contentValue = params.value("content").toString();
        QObject *obj = new QObject(this);
        m_fileControll->newFile(id, fileName, contentValue);
        connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
            if (id != c_id) {
                return ;
            } else {
                obj->deleteLater();
                m_requestMap.remove(id);
                if (code == NOERROR) {
                    emit onReplyMessage_signal(id, true);
                } else {
                    emit onErrorOccured_signal(id, code);
                }
            }
        });
    }
}

void Device::pDecodeBase64File(quint64 id, QJsonObject params)
{
    checkStringValue(params, "fileName");
    checkObjectValue(params, "content");
    QString fileName = params.value("fileName").toString();
    QString contentValue = params.value("content").toString();

    QObject *obj = new QObject(this);

    m_fileControll->decodeFile(id, fileName, contentValue);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                emit onReplyMessage_signal(id, true);
            } else {
                qDebug() << "decode file failed.";
                emit onErrorOccured_signal(id, code);
            }
        }


    });
}

void Device::pNewFolder(quint64 id, QJsonObject params)
{
    checkStringValue(params, "folderName");
    checkStringValue(params, "url");
    QString folderName = params.value("folderName").toString();
    QString url = params.value("url").toString();

    QObject *obj = new QObject(this);
    m_fileControll->newFolder(id, url, folderName);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                emit onReplyMessage_signal(id, true);
            } else {
                qDebug() << "decode file failed.";
                emit onErrorOccured_signal(id, code);
            }
        }
    });

}

void Device::pReadFolder(quint64 id, QJsonObject params)
{
    checkStringValue(params, "folderName");

    QString folderName = params.value("folderName").toString();

    QObject *obj = new QObject(this);
    m_fileControll->readFolder(id, folderName);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data, QJsonValue va){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                if (va.isObject()){
                    QJsonObject resObj;
                    resObj.insert("result", va.toObject());
                    emit onReplyMessage_signal(id, resObj);
                } else {
                    QJsonObject resObj;
                    emit onReplyMessage_signal(id, resObj);
                }
            } else {
                qDebug() << "read folder failed.";
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pDeleteFolder(quint64 id, QJsonObject params)
{
    checkStringValue(params, "url");
    checkStringValue(params, "folderName");
    QString folderName = params.value("folderName").toString();
    QString url = params.value("url").toString();

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->deleteFolder(id, url, folderName);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                emit onReplyMessage_signal(id, true);
            } else {
                qDebug() << "decode file failed.";
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pRenameFolder(quint64 id, QJsonObject params)
{
    checkStringValue(params, "newfolderName");
    checkStringValue(params, "folderName");
    QString folderName = params.value("folderName").toString();
    QString newfolderName = params.value("newfolderName").toString();

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->renameFolder(id, folderName, newfolderName);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                emit onReplyMessage_signal(id, true);
            } else {
                qDebug() << "rename file failed.";
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pCopyFolder(quint64 id, QJsonObject params)
{
    checkStringValue(params, "url");
    checkStringValue(params, "newfolderName");
    checkStringValue(params, "folderName");
    QString url = params.value("url").toString();
    QString folderName = params.value("folderName").toString();
    QString newfolderName = params.value("newfolderName").toString();

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->copyFolder(id, url, folderName, newfolderName);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                emit onReplyMessage_signal(id, true);
            } else {
                qDebug() << "copy folder failed.";
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pPathIsExist(quint64 id, QJsonObject params)
{
    checkStringValue(params, "path");
    QString path = params.value("path").toString();

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->pathIsExist(id, path);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code,QByteArray array, QJsonValue va){
        Q_UNUSED(array);
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                emit onReplyMessage_signal(id, va);
            } else {
                qDebug() << "copy folder failed.";
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pCopyFileFromLocaleToSmb(quint64 id, QJsonObject params)
{
    checkStringValue(params, "localPath");
    checkStringValue(params, "smbPath");
    QString strLocalPath = params.value("localPath").toString();
    QString strSmbPath = params.value("smbPath").toString();
    QString ip = params.value("portName").toString();
    quint32 timeout = params.value("timeout").toInt(2000);
    bool truncate = params.value("truncate").toBool(false);

    QObject *obj = new QObject(this);
    m_fileControll->copyFileFromLocaleToSmb(id, strLocalPath,strSmbPath,truncate,timeout);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code,QByteArray array, QJsonValue va){
        Q_UNUSED(array);
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                emit onReplyMessage_signal(id, va);
            } else {
                qDebug() << "copy file failed.";
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

//![3.1] 连接状态
void Device::pGetConnectionState(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    m_module->sendGetRequest("/connection/state", id, "GetConnectionState");
}

//![4.1] 周期性命令和数据交互
void Device::pExchange(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    if (!dataObj.isEmpty()) {
        m_module->sendPostRequest("/protocol/exchange", dataObj, id, "Exchange");
    } else {
        m_requestMap.remove(id);
        qDebug() << "data obj is missing.";
        emit onErrorOccured_signal(id, ERROR_INDUSTRY_JSONOBJECT);
    }
}

void Device::pGetDobotStatus(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetDobotStatus");
}

bool Device::eventLoopSuccess(quint64 id, QEventLoop &eventLoop)
{
    QTimer timer;
    timer.setInterval(1000);
    timer.setSingleShot(true);
    timer.start();

    bool isSuccess(true);
    connect(&timer, &QTimer::timeout, [&]() {
        eventLoop.quit();
        m_eventloopMap.remove(id);
        isSuccess = false;
        qDebug() << "exchange timeout !!!" ;
        emit onErrorOccured_signal(id, ERROR_COMMUNICATION_TIMEOUT);
    });
    qDebug() << "exchange success !!!" ;
    eventLoop.exec();
    if (isSuccess)
        timer.stop();
    return isSuccess;
}

void Device::pSetControlMode(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    m_module->sendPostRequest("/settings/controlMode", dataObj, id, "SetControlMode");
}

void Device::pGetControlMode(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetControlMode");
}

void Device::pSetCoordinateMode(quint64 id, QJsonObject params)
{
    //don't use like this:m_module->sendPostRequest("/interface/coordinate", dataObj, id, "SetCoordinate");
    if (checkStringValue(params, "mode")) {
        _getStatus(id, "GetPrivateDobotStatus");

        QEventLoop eventLoop;
        m_eventloopMap.insert(id, &eventLoop);
        if(!eventLoopSuccess(id, eventLoop)) return;

        QJsonObject obj = eventLoop.property("resObj").toJsonObject();
        RobotStatus status;
        status.setStatus(obj);
        status.setCoordinateMode(params.value("mode").toString());

        _sendStatus(id, status, "SetCoordinateMode");
    } else {
        m_requestMap.remove(id);
        emit onErrorOccured_signal(id, ERROR_INDUSTRY_MISSKEY);
    }
}

void Device::pGetCoordinateMode(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetCoordinateMode");
}

void Device::pSetCoordinateMode2(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    m_module->sendPostRequest("/interface/coordinate", dataObj, id, "SetCoordinateMode2");
}

void Device::pSetJogMode(quint64 id, QJsonObject params)
{
    //donot use like this:m_module->sendPostRequest("/interface/jogMode", dataObj, id, "SetCRJogMode");
    if (checkStringValue(params, "mode")) {
        _getStatus(id, "GetPrivateDobotStatus");

        QEventLoop eventLoop;
        m_eventloopMap.insert(id, &eventLoop);
        if(!eventLoopSuccess(id, eventLoop)) return;

        QJsonObject obj = eventLoop.property("resObj").toJsonObject();
        RobotStatus status;
        status.setStatus(obj);
        status.setJogMode(params.value("mode").toString());

        _sendStatus(id, status, "SetJogMode");
    } else {
        m_requestMap.remove(id);
        emit onErrorOccured_signal(id, ERROR_INDUSTRY_MISSKEY);
    }
}

void Device::pGetJogMode(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetJogMode");
}

void Device::pSetJogMode2(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    m_module->sendPostRequest("/interface/jogMode", dataObj, id, "SetJogMode2");
}

void Device::pSetToolCoordPattern(quint64 id, QJsonObject params)
{
    if (checkIntValue(params, "pattern")) {
        _getStatus(id, "GetPrivateDobotStatus");

        QEventLoop eventLoop;
        m_eventloopMap.insert(id, &eventLoop);
        if(!eventLoopSuccess(id, eventLoop)) return;

        QJsonObject obj = eventLoop.property("resObj").toJsonObject();
        RobotStatus status;
        status.setStatus(obj);
        status.setToolCoordPattern(params.value("pattern").toInt());

        _sendStatus(id, status, "SetToolCoordPattern");
    } else {
        m_requestMap.remove(id);
        emit onErrorOccured_signal(id, ERROR_INDUSTRY_MISSKEY);
    }
}

void Device::pGetToolCoordPattern(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetToolCoordPattern");
}

void Device::pSetUserCoordPattern(quint64 id, QJsonObject params)
{
    if (checkIntValue(params, "pattern")) {
        _getStatus(id, "GetPrivateDobotStatus");

        QEventLoop eventLoop;
        m_eventloopMap.insert(id, &eventLoop);

        if(!eventLoopSuccess(id, eventLoop)) return;

        QJsonObject obj = eventLoop.property("resObj").toJsonObject();
        RobotStatus status;
        status.setStatus(obj);
        status.setUserCoordPattern(params.value("pattern").toInt());

        _sendStatus(id, status, "SetUserCoordPattern");
    } else {
        m_requestMap.remove(id);
        emit onErrorOccured_signal(id, ERROR_INDUSTRY_MISSKEY);
    }
}

void Device::pGetUserCoordPattern(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetUserCoordPattern");
}

void Device::pGetAutoManualMode(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetAutoManualMode");
}

void Device::pGetJointCoordinate(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetJointCoordinate");
}

void Device::pGetCartesianCoordinate(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetCartesianCoordinate");
}

void Device::pGetisCollision(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetisCollision");
}

void Device::pGetProjectState(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetProjectState");
}

void Device::pGetAlarms(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetAlarms");
}

void Device::pClearAlarms(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetPrivateDobotStatus");

    QEventLoop eventLoop;
    m_eventloopMap.insert(id, &eventLoop);
    if(!eventLoopSuccess(id, eventLoop)) return;

    QJsonObject obj = eventLoop.property("resObj").toJsonObject();
    RobotStatus status;
    status.setStatus(obj);
    status.clearAlarms();

    _sendStatus(id, status, "ClearAlarms");
}

void Device::pClearAlarms2(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    m_module->sendPostRequest("/interface/clearAlarms", dataObj, id, "ClearAlarms2");
}

void Device::pGetInput(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetInput");
}

void Device::pSetOutput(quint64 id, QJsonObject params)
{

    if (checkBoolValue(params, "enable")) {
        _getStatus(id, "GetPrivateDobotStatus");

        QEventLoop eventLoop;
        m_eventloopMap.insert(id, &eventLoop);
        if(!eventLoopSuccess(id, eventLoop)) return;

        QJsonObject obj = eventLoop.property("resObj").toJsonObject();
        RobotStatus status;
        status.setStatus(obj);

        bool enable = params.value("enable").toBool();

        checkArrayValue(params, "outputs");
        QJsonArray outputs = params.value("outputs").toArray();
        status.setOutputs(enable, outputs);

        _sendStatus(id, status, "SetOutput");
    } else {
        m_requestMap.remove(id);
        emit onErrorOccured_signal(id, ERROR_INDUSTRY_MISSKEY);
    }
}

void Device::pSetOutput2(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    m_module->sendPostRequest("/interface/outputs", dataObj, id, "SetOutput2");
}

void Device::pGetOutput(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetOutput");
}

void Device::pSetExtendIO(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    m_module->sendPostRequest("/interface/extendIO", dataObj, id, "SetExtendIO");
}

void Device::pGetExtendDI(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetExtendDI");
}

void Device::pGetExtendDO(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    _getStatus(id, "GetExtendDO");
}

/* 5.参数设定 */
//![5.2] 全局比例
void Device::pSetCommonSetting(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/common.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/common", dataObj, id, "SetCommonSetting");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pGetCommonSetting(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/common.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });

}

/* 5.3 示教参数 */
//![5.3.1] 示教关节点动
void Device::pSetTeachJoint(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/teach/joint.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/teach/joint", dataObj, id, "SetTeachJoint");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }
    });

}

void Device::pGetTeachJoint(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/teach/joint.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
        //
    });
}

//![5.3.2] 示教坐标系点动
void Device::pSetTeachCoordinate(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/teach/coordinate.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/teach/coordinate", dataObj, id, "SetTeachCoordinate");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetTeachCoordinate(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QByteArray data;
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/teach/coordinate.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}
//![5.3.2] 示教坐标系寸动
void Device::pSetTeachInch(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/teach/inch.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/teach/inch", dataObj, id, "SetTeachInch");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pGetTeachInch(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/teach/inch.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

/* 5.4 再现参数*/
//![5.4.1] 关节空间规划参数
void Device::pSetPlaybackJoint(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/playback/joint.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/playback/joint", dataObj, id, "SetPlaybackJoint");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pGetPlaybackJoint(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/playback/joint.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

//![5.4.2] 笛卡尔空间规划参数
void Device::pSetPlaybackCoordinate(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/playback/coordinate.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/playback/coordinate", dataObj, id, "SetPlaybackCoordinate");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pGetPlaybackCoordinate(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)


    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/playback/coordinate.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        // obj->deleteLater();qDebug() << "id:" << id << "c_id" << c_id << isSuccess;
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

//![5.4.3] Arch参数
void Device::pSetPlaybackArch(quint64 id, QJsonObject params)
{
    QJsonValue dataValue = params.value("data");
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/playback/arch.json", dataValue);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/playback/arch", dataValue, id, "SetPlaybackArch");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pGetPlaybackArch(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/playback/arch.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

//![5.5] 工具坐标系
void Device::pSetToolCoordinate(quint64 id, QJsonObject params)
{
    QJsonValue data = params.value("data");
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/coordinate/tool.json", data);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/coordinate/tool", data, id, "SetToolCoordinate");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pSetToolCoordinate2(quint64 id, QJsonObject params)
{

    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/coordinate", dataObj, id, "SetToolCoordinate2");
}

void Device::pGetToolCoordinate(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/coordinate/tool.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

//![5.6] 用户坐标系参数
void Device::pSetUserCoordinate(quint64 id, QJsonObject params)
{
    QJsonValue data = params.value("data");
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/coordinate/user.json", data);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/coordinate/user", data, id, "SetUserCoordinate");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pSetUserCoordinate2(quint64 id, QJsonObject params)
{

    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/userCoordinate", dataObj, id, "SetUserCoordinate2");
}

void Device::pGetUserCoordinate(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/coordinate/user.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                if (data.isEmpty())data.append("[]");
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

/* 5.7 示教点列表 */
//![5.7.1] 增加示教点文件
void Device::pSetTeachFileUpdate(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkStringValue(dataObj, "file");

    m_module->sendPostRequest("/project/teachFileUpdate", dataObj, id, "SetTeachFileUpdate");
}

//![5.7.2] 删除示教点文件
void Device::pDelTeachFileDelete(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkStringValue(dataObj, "file");

    m_module->sendPostRequest("/project/teachFileDelete", dataObj, id, "DelTeachFileDelete");
}

void Device::pSetDragSensivity(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/function/dragSensivity.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/function/dragSensivity", dataObj, id, "SetDragSensivity");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetDragSensivity(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/function/dragSensivity.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

/* 6. 标定 */
void Device::pSetCalibrateToolCoor(quint64 id, QJsonObject params)
{
    QJsonValue dataValue = params.value("data");

    m_module->sendPostRequest("/calibrate/coordinate/tool", dataValue, id, "SetCalibrateToolCoor");
}

void Device::pSetCalibrateToolPosition(quint64 id, QJsonObject params)
{
    QJsonValue dataValue = params.value("data");

    m_module->sendPostRequest("/calibrate/coordinate/tool/position", dataValue, id, "SetCalibrateToolPosition");
}

void Device::pSetCalibrateToolPose(quint64 id, QJsonObject params)
{
    QJsonValue dataValue = params.value("data");

    m_module->sendPostRequest("/calibrate/coordinate/tool/pose", dataValue, id, "SetCalibrateToolPose");
}

void Device::pSetCalibrateUserCoor(quint64 id, QJsonObject params)
{
    QJsonValue dataValue = params.value("data");

    m_module->sendPostRequest("/calibrate/coordinate/user", dataValue, id, "SetCalibrateUserCoor");
}

void Device::pSetCalibrateLeftRightHand(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    if (checkFloatValue(dataObj, "left") && checkFloatValue(dataObj, "right")){
        m_module->sendPostRequest("/calibrate/leftRightHand", dataObj, id, "SetCalibrateLeftRightHand");
    } else {
        m_requestMap.remove(id);
        emit onErrorOccured_signal(id, ERROR_INDUSTRY_MISSKEY);
    }
}

void Device::pSetCalibrateHome(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    m_module->sendPostRequest("/calibrate/home", dataObj, id, "SetCalibrateHome");
}

void Device::pSetCalibrateAxis(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    m_module->sendPostRequest("/calibrate/homeSingleAxis", dataObj, id, "SetCalibrateAxis");
}


#ifdef USE_MOBDEBUG
/* 7. 调试器 */
//![7.1] 调试器状态
void Device::pGetDebuggerState(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/debugger/state", id, "GetDebuggerState");
}

//![7.2] 启动调试器（start)
void Device::pSetDebuggerStart(quint64 id, QJsonObject params)
{
    emit onDebuggerNotify_signal("Debugger starting...");
    // 一键运行标志位
    bool isMobdebug = params.value("isMobdebug").toBool();
    if (isMobdebug) {
        if (!m_modebug->start(id)) {
            m_requestMap.remove(id);
            emit onErrorOccured_signal(id, ERROR_MOBDEBUG_START_FAILED);
            return;
        }
        QObject *obj = new QObject(this);
        connect(m_modebug, &Mobdebug::onModebugStateChanged_signal, obj, [=](Mobdebug::ModebugState state, quint64 id){
            qDebug() << "modebug state changed:" << state<<"  id:"<<id;
            if (state == Mobdebug::MODEBUG_LISTENING) {
                obj->deleteLater();
                QJsonObject params = m_requestMap.value(id);
                m_module->sendPostRequest("/debugger/start", params.value("data").toObject(), id, "SetPrivateDebuggerStart");
            }
        });
    } else {
        m_module->sendPostRequest("/debugger/start", params.value("data").toObject(), id, "SetDebuggerStart");
    }
}

//![7.3] 停止调试器(stop)
void Device::pSetDebuggerStop(quint64 id, QJsonObject params)
{
    bool isMobdebug = params.value("isMobdebug").toBool();
    if (isMobdebug) {
        // It will take quite a while to stop the mobdebug process.
        // So, send stop cmd before process stop.
        m_module->sendPostRequest("/debugger/stop", QJsonObject(), id, "SetPrivateDebuggerStop");

        // todo: bad idea, firmware have a bug.
        QObject *obj = new QObject(this);
        connect(m_module, &Module::onReceiveData_signal, obj, [=](QJsonValue value, QString url, quint64 _id, QString api) {
            Q_UNUSED(url)

            if (_id == id && api == "SetPrivateDebuggerStop") {
                m_modebug->mo_exit();
                obj->deleteLater();
                emit onReplyMessage_signal(id, value);
            }
        });
    } else {
        m_module->sendPostRequest("/debugger/stop", QJsonObject(), id, "SetDebuggerStop");
    }

}

//![7.4] 运行(run)
void Device::pSetDebuggerRun(quint64 id, QJsonObject params)
{
    emit onDebuggerNotify_signal("Debugger running...");
    bool isMobdebug = params.value("isMobdebug").toBool();
    if (isMobdebug) {
        //MD的Run指令要放在HTTP的Run指令之后
        //不然会卡在运动指令之中
        m_module->sendPostRequest("/debugger/run", params, id, "SetDebuggerRun");
        m_modebug->mo_run();
    } else {
        m_module->sendPostRequest("/debugger/run", params, id, "SetDebuggerRun");
    }
}

//![7.5] 暂停(suspend)
void Device::pSetDebuggerSuspend(quint64 id, QJsonObject params)
{
    bool isMobdebug = params.value("isMobdebug").toBool();
    if (isMobdebug) {
        m_modebug->mo_suspend(id);
        QObject *obj = new QObject(this);
        connect(m_modebug, &Mobdebug::onModebugStateChanged_signal, obj, [=](Mobdebug::ModebugState state, quint64 id){
            qDebug() << "connect modebug state changed:" << state <<"  id:"<<id;
            if (state == Mobdebug::MODEBUG_SUSPENDED) {
                obj->deleteLater();
                m_module->sendPostRequest("/debugger/suspend", params, id, "SetDebuggerSuspend");
            }
        });
        m_module->sendPostRequest("/debugger/suspend", params, id, "SetDebuggerSuspend");
        emit onDebuggerNotify_signal("Script suspended!");
    } else {
        m_module->sendPostRequest("/debugger/suspend", params, id, "SetDebuggerSuspend");
        emit onDebuggerNotify_signal("Script suspended!");
    }
}

//![7.6] 单步进入函数(stepIn)
void Device::pSetDebuggerStepIn(quint64 id, QJsonObject params)
{
    bool isMobdebug = params.value("isMobdebug").toBool();
    if (isMobdebug) {
        m_modebug->mo_step();
        m_module->sendPostRequest("/debugger/stepIn", params, id, "SetDebuggerStepIn");
    } else {
        m_module->sendPostRequest("/debugger/stepIn", params, id, "SetDebuggerStepIn");
    }
}

//![7.7] 单步跳过函数(stepOver)
void Device::pSetDebuggerStepOver(quint64 id, QJsonObject params)
{
    bool isMobdebug = params.value("isMobdebug").toBool();
    if (isMobdebug) {
        m_modebug->mo_over();
        m_module->sendPostRequest("/debugger/stepOver", params, id, "SetDebuggerStepOver");
    } else {
        m_module->sendPostRequest("/debugger/stepOver", params, id, "SetDebuggerStepOver");
    }

}

//![7.8] 设置断点(setb)
void Device::pSetDebuggerSetb(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    bool isMobdebug = params.value("isMobdebug").toBool();
    if (isMobdebug) {
        if (dataObj.contains("path") and dataObj.contains("line")) {
            m_module->sendPostRequest("/debugger/setb", dataObj, id, "SetDebuggerSetb");

            QString path = dataObj.value("path").toString();
            int line = dataObj.value("line").toInt();
            m_modebug->mo_setb(path, line);
        } else {
            m_requestMap.remove(id);
            qDebug() << "parameter missing.";
            emit onErrorOccured_signal(id, ERROR_INDUSTRY_MISSKEY);
        }
    } else {
        m_module->sendPostRequest("/debugger/setb", dataObj, id, "SetDebuggerSetb");
    }
}

//![7.9] 移除断点(delb)
void Device::pSetDebuggerDelb(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    bool isMobdebug = params.value("isMobdebug").toBool();
    if (isMobdebug) {
        if (dataObj.contains("path") and dataObj.contains("line")) {
            m_module->sendPostRequest("/debugger/delb", dataObj, id, "SetDebuggerDelb");

            QString path = dataObj.value("path").toString();
            int line = dataObj.value("line").toInt();
            m_modebug->mo_delb(path, line);
        } else {
            m_requestMap.remove(id);
            qDebug() << "parameter missing.";
            emit onErrorOccured_signal(id, ERROR_INDUSTRY_MISSKEY);
        }
    } else {
        m_module->sendPostRequest("/debugger/delb", dataObj, id, "SetDebuggerDelb");
    }

}

//![7.10] 移除所有断点(delallb)
void Device::pSetDebuggerDelAllb(quint64 id, QJsonObject params)
{
    bool isMobdebug = params.value("isMobdebug").toBool();
    if (isMobdebug) {
        m_module->sendPostRequest("/debugger/delallb", params, id, "SetDebuggerDelAllb");
        m_modebug->mo_delallb();
    } else {
        m_module->sendPostRequest("/debugger/delallb", params, id, "SetDebuggerDelAllb");
    }
}
#endif

/* 8. 面板模拟 */
//![8.1] 自动/手动
void Device::pSetAutoManualMode(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkStringValue(dataObj, "value");

    m_module->sendPostRequest("/panel/autoManual", dataObj, id, "SetAutoManualMode");
}

//![8.2] 三位开关
void Device::pSetThreeSwitch(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkStringValue(dataObj, "value");

    m_module->sendPostRequest("/panel/threeSwitch", dataObj, id, "SetThreeSwitch");
}

//![8.3] 急停状态
void Device::pSetEmergencyStop(quint64 id, QJsonObject params)
{
    emit onDebuggerNotify_signal("Debugger on .");
    QJsonObject dataObj = params.value("data").toObject();

    checkBoolValue(dataObj, "value");

    m_module->sendPostRequest("/panel/emergencyStop", dataObj, id, "SetEmergencyStop");
}

void Device::pGetEmergencyStop(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    m_module->sendGetRequest("/panel/emergencyStop", id, "GetEmergencyStop");
}

//![8.4] 点动按键
void Device::pSetJogCmd(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkArrayValue(dataObj, "posBtns");
    checkArrayValue(dataObj, "negBtns");

    m_module->sendPostRequest("/panel/jog", dataObj, id, "SetJogCmd");
}

/* 9. 网络设置 */
//![9.1] 设置eth2
void Device::pSetEthernet(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkBoolValue(dataObj, "dhcp");
    checkStringValue(dataObj, "ip");
    checkStringValue(dataObj, "netmask");
    checkStringValue(dataObj, "gateway");

    m_module->sendPostRequest("/interface/ethernet", dataObj, id, "SetEthernet");
}

void Device::pGetEthernet(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/interface/ethernet", id, "GetEthernet");
}

void Device::pSetAP(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    m_module->sendPostRequest("/interface/setAP", dataObj, id, "SetAP");
}

void Device::pGetAP(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/interface/setAP", id, "GetAP");
}

/* 10.1 获取相机坐标 */
void Device::pSetVisionCoordinate(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkStringValue(dataObj, "ip");
    checkIntValue(dataObj, "port");
    checkIntValue(dataObj, "index");

    m_module->sendPostRequest("/interface/visionCoordinate", dataObj, id, "SetVisionCoordinate");
}

/* 10.2 运动到指定点 */
void Device::pSetMoveCmd(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkFloatValue(dataObj, "x");
    checkFloatValue(dataObj, "y");
    checkFloatValue(dataObj, "z");
    checkFloatValue(dataObj, "r");

    m_module->sendPostRequest("/interface/move", dataObj, id, "SetMoveCmd");
}

/* 11. 传送带跟踪 */
void Device::pGetCurrentEncoder(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkIntValue(dataObj, "conveyor_index");

    m_module->sendPostRequest("/interface/readCurrentEncoder", dataObj, id, "GetCurrentEncoder");
}

void Device::pGetSensorEncoder(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkIntValue(dataObj, "conveyor_index");

    m_module->sendPostRequest("/interface/readSensorEncoder", dataObj, id, "GetSensorEncoder");
}

void Device::pSetSensorEncoderListen(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QJsonObject dataObj = params.value("data").toObject();
    if(checkStringValue(dataObj, "cmd")&&
            checkIntValue(dataObj, "conveyor_index")&&
            checkIntValue(dataObj, "DI_index")&&
            checkIntValue(dataObj, "trigger_type")){
        m_module->sendPostRequest("/interface/listenSensor", dataObj ,id, "SetSensorEncoderListen");
    }else{
        emit onErrorOccured_signal(id, ERROR_INDUSTRY_JSONOBJECT);
    }


}

/* 12. 手动模式中全局速度比例 */
//Deprecated MG400控制器版本1.4.6.0及以上后弃用，CR弃用
void Device::pSetManualSpeedRatio(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkIntValue(dataObj, "ratio");

    m_module->sendPostRequest("/interface/manualCommon", dataObj, id, "SetManualSpeedRatio");
}
//Deprecated MG400控制器版本1.4.6.0及以上后弃用，CR弃用
void Device::pGetManualSpeedRatio(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/interface/manualCommon", id, "GetManualSpeedRatio");
}

/* 13. DI 模拟输入 */
void Device::pSetDIMode(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkIntValue(dataObj, "index");
    checkIntValue(dataObj, "mode");

    m_module->sendPostRequest("/interface/setDIMode", dataObj, id, "SetDIMode");
}

void Device::pGetDIMode(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkIntValue(dataObj, "index");

    m_module->sendPostRequest("/interface/getDIMode", dataObj, id, "GetDIMode");
}

void Device::pSetDIValue(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkIntValue(dataObj, "index");
    checkIntValue(dataObj, "value");

    m_module->sendPostRequest("/interface/setDIValue", dataObj, id, "SetDIValue");
}

/* 14. 通用IO模拟量 */
void Device::pSetGPIOAO(quint64 id, QJsonObject params)
{
    QJsonValue dataObj = params.value("data");
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/function/gpioAO.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                QJsonValue dataValue = params.value("data");
                m_module->sendPostRequest("/settings/function/gpioAO", dataValue, id, "SetGPIOAO");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetGPIOAO(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    //需要通过读文件的方式来获取结果。
    //m_module->sendGetRequest("/settings/function/gpioAO", id, "GetGPIOAO");
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/function/gpioAO.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pSetGPIOAI(quint64 id, QJsonObject params)
{
    QJsonValue dataObj = params.value("data");
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/function/gpioAI.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                QJsonValue dataValue = params.value("data");
                m_module->sendPostRequest("/settings/function/gpioAI", dataValue, id, "SetGPIOAI");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetGPIOAI(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    //需要通过读文件的方式来获取结果。
    //m_module->sendGetRequest("/settings/function/gpioAI", id, "GetGPIOAI");
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/function/gpioAI.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

/* 15. 协作机器人安全配置 */
void Device::pSetGeneralSafeSetting(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/settings/function/generalSafeSetting", dataObj, id, "SetGeneralSafeSetting");
}

void Device::pSetLoadParams(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    checkFloatValue(dataObj, "inertiaX");
    checkFloatValue(dataObj, "inertiaY");
    checkFloatValue(dataObj, "inertiaZ");
    checkFloatValue(dataObj, "loadValue");

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/function/loadParams.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/function/loadParams", dataObj, id, "SetLoadParams");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pGetLoadParams(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/settings/function/loadParams", id, "GetLoadParams");
}

void Device::pSetAdvancedFunc(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/settings/function/advancedFunction", dataObj, id, "SetAdvancedFunc");
}

void Device::pSetCollisionDetect(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkBoolValue(dataObj, "value");

    bool value = dataObj.value("value").toBool();

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->changeFile(id, "/project/settings/function/collisionDect.json", "value", value);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if(id != c_id) return ;
        if (code == NOERROR) {
            obj->deleteLater();
            m_module->sendPostRequest("/settings/function/collisionDect", dataObj, id, "SetCollisionDetect");
        } else {
            m_requestMap.remove(id);
            emit onErrorOccured_signal(id, code);
        }

    });
}

void Device::pSetCollisionDetectLevel(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkIntValue(dataObj, "level");

    int level = dataObj.value("level").toInt();

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->changeFile(id, "/project/settings/function/collisionDect.json", "level", level);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if(id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/function/collisionDect", dataObj, id, "SetCollisionDetectLevel");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pSetCollisionDetectResumeType(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkIntValue(dataObj, "resumeType");

    int resumeType = dataObj.value("resumeType").toInt();

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->changeFile(id, "/project/settings/function/collisionDect.json", "resumeType", resumeType);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if(id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/function/collisionDect", dataObj, id, "SetCollisionDetectResumeType");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pSetCollisionDetectParam(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    if (!checkStringValue(dataObj, "key") || !dataObj.contains("value"))
    {//data中不包含字段key或value，就不允许操作
        qDebug().noquote() << "the params missing 'key' or 'value' field!!";
        emit onErrorOccured_signal(id, ERROR_INDUSTRY_MISSKEY);
        return ;
    }
    QString strKey = dataObj.value("key").toString();
    QJsonValue value = dataObj.value("value");
    if (strKey.isEmpty() || value.isNull() || value.isUndefined())
    {
        qDebug().noquote() << "invalid params!!";
        emit onErrorOccured_signal(id, ERROR_INVALID_PARAMS);
        return ;
    }

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->changeFile(id, "/project/settings/function/collisionDect.json", strKey, value);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if(id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/function/collisionDect", dataObj, id, "SetCollisionParameter");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetCollisionDetect(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    QString ip = params.value("portName").toString();

    m_fileControll->readFile(id, "/project/settings/function/collisionDect.json");
    QObject *obj = new QObject(this);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pSetSafeParams(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/settings/safeparams", dataObj, id, "SetSafeParams");
}

/* 16. 电子皮肤 */
void Device::pSetElecSkinEnable(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkBoolValue(dataObj, "value");

    m_module->sendPostRequest("/settings/function/elecSkin", dataObj, id, "SetElecSkinEnable");
}

void Device::pSetElecSkinReset(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/resetElecSkin", dataObj, id, "SetElecSkinReset");
}

void Device::pGetElecSkinReset(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/interface/resetElecSkin", id, "GetElecSkinReset");
}

void Device::pSetElecSkinParams(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/settings/function/elecSkinParams", dataObj, id, "SetElecSkinParams");
}

void Device::pGetElecSkinParams(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/settings/function/elecSkinParams", id, "GetElecSkinParams");
}

/* 17. 回零 */
void Device::pSetGoHomeCmd(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/goHome", dataObj, id, "SetGoHomeCmd");
}

void Device::pGetGoHomeCmd(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/interface/goHome", id, "GetGoHomeCmd");
}

/* 18. 开关抱闸 */
void Device::pSetAxisJointBrake(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/6axisJointBrake", dataObj, id, "SetAxisJointBrake");
}

void Device::pGetAxisJointBrake(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/interface/6axisJointBrake", id, "GetAxisJointBrake");
}

/* 19. 轨迹复现 */
void Device::pSetRecurrentTrack(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/recurrentTrack", dataObj, id, "SetRecurrentTrack");
}

void Device::pSetDebugReTrace(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/debugReTrace", dataObj, id, "SetDebugReTrace");
}

void Device::pGetDebugReTrace(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/interface/debugReTrace", id, "GetDebugReTrace");
}

/* 20. 协作机器人自动识别 */
void Device::pSetAutoIdentify(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    checkBoolValue(dataObj, "value");
    m_module->sendPostRequest("/settings/autoIdentify", dataObj, id, "SetAutoIdentify");
}

/* 21.协作机器人安装位置 */
void Device::pSetInstallPosture(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/setting/installPosture", dataObj, id, "SetInstallPosture");
}

/* 22.协作机器人实轴、虚轴 */
void Device::pSetSimulatedAxies(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/setting/setSimulatedAxies", dataObj, id, "SetSimulatedAxies");
}

void Device::pGetPropertyAxis(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/properties/axis", id, "GetPropertyAxis");
}

void Device::pGetBusinessType(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/properties/businessType", id, "GetBusinessType");
}

/* 23.协作机器人末端执行器 */
void Device::pSetRobottiqGripperEnable(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/setting/function/robotiqGripperEnable", dataObj, id, "SetRobottiqGripperEnable");
}

void Device::pGetRobottiqGripperEnable(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/setting/function/robotiqGripperEnable", id, "GetRobottiqGripperEnable");
}

void Device::pSetRobottiqGripperEnableK(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/robotiqGripperEnable", dataObj, id, "SetRobottiqGripperEnableK");
}

void Device::pSetHitbotGripperEnable(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/hitbotGripper", dataObj, id, "SetHitbotGripperEnable");
}

void Device::pGetHitbotGripperEnable(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/interface/hitbotGripper", id, "GetHitbotGripperEnable");
}

void Device::pSetHitbotGripper(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/DHGripper", dataObj, id, "SetHitbotGripper");
}

void Device::pSetDHGripperEnable(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/DHGripperEnable", dataObj, id, "SetDHGripperEnable");
}

void Device::pGetDHGripperEnable(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/interface/DHGripperEnable", id, "GetDHGripperEnable");
}

void Device::pSetDHGripper(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/DHGripper", dataObj, id, "SetDHGripper");
}

void Device::pGetDHGripper(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/interface/DHGripper", id, "GetDHGripper");
}

/* 24.协作机器人 6维力传感器接口 */
void Device::pSetRobotiqSixForce(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/settings/function/robotiqSixForce", dataObj, id, "SetRobotiqSixForce");
}

void Device::pGetRobotiqSixForce(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/settings/function/robotiqSixForce", id, "GetRobotiqSixForce");
}

/* 24.2 模拟量接口 */
void Device::pSetEndAI(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/function/endAI.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/function/endAI", dataObj, id, "SetEndAI");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetEndAI(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    //m_module->sendGetRequest("/settings/function/endAI", id, "GetEndAI");
    //需要通过读文件的方式来获取结果。
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/function/endAI.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

/* 25. */
void Device::pSetPowerControl(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/powerControl", dataObj, id, "SetPowerControl");
}

void Device::pGetPowerControl(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/interface/powerControl", id, "GetPowerControl");
}

/* 27.协作机器人RUNTO */
void Device::pSetCRRunTo(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/go", dataObj, id, "SetCRRunTo");
}

void Device::pGetCRRunTo(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    m_module->sendGetRequest("/interface/go", id, "GetCRRunTo");
}

/* 28. 版本号 */
void Device::pGetVersion(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendPostRequest("/settings/version", 0, id, "GetVersion");
}

/* 30.公共接口 */
void Device::pPublicInterface(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QString url = params.value("url").toString();
    QString httpmethod = params.value("httpmethod").toString();
    if(httpmethod == "post"){
        m_module->sendPostRequest(url, dataObj, id, "PublicInterface");
    }
    else if(httpmethod == "get"){
        m_module->sendGetRequest(url, id, "PublicInterface");
    }
    else {
        //若传入http方法错误，则默认发送get请求
        m_module->sendGetRequest(url, id, "PublicInterface");
    }
}

/* 31.MagicianPro特殊接口 */
void Device::pSetPanelDragTeach(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/panel/dragTeach", dataObj, id, "SetPanelDragTeach");
}

void Device::pSetDriverBasicParam(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/settings/driver/userParam", dataObj, id, "SetDriverBasicParam");

}

void Device::pGetDriverBasicParam(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/settings/driver/userParam", id, "GetDriverBasicParam");
}

void Device::pSetDriverParam(quint64 id, QJsonObject params)
{
    QJsonValue dataObj = params.value("data");

    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/properties/driverParams.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);

            if (code == NOERROR) {
                emit onReplyMessage_signal(id, true);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetDriverParam(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/properties/driverParams.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetAlarmController(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/properties/alarm_controller.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }

    });

}

void Device::pGetAlarmServo(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/properties/alarm_servo.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }

    });

}

void Device::pSetFunctionIoctrl(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    checkStringValue(dataObj, "in");
    checkStringValue(dataObj, "out");
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/function/ioctrl.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/function/ioctrl", dataObj, id, "SetFunctionIoctrl");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pGetFunctionIoctrl(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)


    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/function/ioctrl.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pSetFunctionRemoteControl(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    checkStringValue(dataObj, "mode");
    checkStringValue(dataObj, "name");
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/function/remoteControl.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/function/remoteControl", dataObj, id, "SetFunctionRemoteControl");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetModbusctrl(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/function/modbusctrl.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pSetModbusctrl(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    checkStringValue(dataObj, "mode");
    checkStringValue(dataObj, "name");
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/function/modbusctrl.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/function/modbusctrl", dataObj, id, "SetModbusctrl");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pAddSearchIP(quint64 id, QJsonObject params)
{
    if (params.isEmpty() || !params.contains("data") || !params["data"].isArray())
    {
        m_module->SetSearchIP(QStringList());
        emit onReplyMessage_signal(id, true);
        return ;
    }
    QStringList lstNewIp;
    QJsonArray arrIp = params["data"].toArray();
    for(auto itr = arrIp.begin(); itr != arrIp.end(); ++itr)
    {
        if (!itr->isObject())
        {
            continue;
        }
        QJsonObject obj = itr->toObject();
        if (obj.contains("ip") && obj["ip"].isString())
        {
            QString strNewIp = obj["ip"].toString();

            QRegExp reg("^((25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)\\.){3}(25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)$");
            if (strNewIp.isEmpty() || reg.exactMatch(strNewIp))
            {
                lstNewIp << strNewIp;
            }
        }
    }

    m_module->SetSearchIP(lstNewIp);
    emit onReplyMessage_signal(id, true);
}

void Device::pGetCabinetType(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    m_module->sendGetRequest("/properties/cabinetType", id, "GetCabinetType");
}

void Device::pGetCCBoxVoltage(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    m_module->sendGetRequest("/settings/function/ccboxVoltage", id, "GetCCBoxVoltage");
}

void Device::pSetCCBoxVoltage(quint64 id, QJsonObject params)
{
    QJsonValue dataObj = params.value("data");
    m_module->sendPostRequest("/settings/function/ccboxVoltage", dataObj, id, "SetCCBoxVoltage");
}

void Device::pGetFunctionRemoteControl(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QString ip = params.value("portName").toString();
    quint32 timeout = params.value("timeout").toInt();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/function/remoteControl.json",timeout);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);

            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pSetUserParam(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    checkStringValue(dataObj, "DeviceName");
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/productInfo/userParam.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);

            if (code == NOERROR) {
                emit onReplyMessage_signal(id, true);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pGetUserParam(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/productInfo/userParam.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        // obj->deleteLater();qDebug() << "id:" << id << "c_id" << c_id << isSuccess;
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);

            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pSetPropertiesHardwareInfo(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkStringValue(dataObj, "SNcode");
    checkStringValue(dataObj, "ControllerVersion");
    checkStringValue(dataObj, "ServoVersion");
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/productInfo/hardwareInfo.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        // obj->deleteLater();qDebug() << "id:" << id << "c_id" << c_id << isSuccess;
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);

            if (code == NOERROR) {
                emit onReplyMessage_signal(id, true);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pGetPropertiesHardwareInfo(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QByteArray data;

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/productInfo/hardwareInfo.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);

            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pSetSystemTime(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/settings/systemTime", dataObj, id, "SetSystemTime");

}

void Device::pGetSystemTime(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/settings/systemTime", id, "GetSystemTime");
}

void Device::pSetFirmwareUpgrade(quint64 id, QJsonObject params)
{
    QJsonObject dataobj = params.value("data").toObject();
    m_module->sendPostRequest("/firmware/upgrade", dataobj, id, "SetFirmwareUpgrade");
}

void Device::pSetWorkTimeRec(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendPostRequest("/settings/workTimeRec", 0, id, "SetWorkTimeRec");
}

void Device::pGetWorkTimeRec(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/settings/workTimeRec", id, "GetWorkTimeRec");
}

void Device::pSetReboot(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendPostRequest("/interface/reboot", 0, id, "SetReboot");
}

void Device::pResetCollision(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendPostRequest("/interface/resetCollision", 0, id, "ResetCollision");
}

void Device::pSetScriptRun(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendPostRequest("/debugger/run", 0, id, "SetScriptRun");
}

void Device::pSetScriptStop(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendPostRequest("/debugger/stop", 0, id, "SetScriptStop");
}

void Device::pSetFunctionPostureCalc(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkIntValue(dataObj, "mode");
    checkIntValue(dataObj, "tool");
    checkIntValue(dataObj, "user");
    checkArrayValue(dataObj, "rawData");
    checkBoolValue(dataObj, "handtype");

    m_module->sendPostRequest("/settings/function/postureCalc", dataObj, id, "SetFunctionPostureCalc");
}

void Device::pSetTruemotion(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkBoolValue(dataObj, "UsingTrueMotion");

    bool UsingTrueMotion = dataObj.value("UsingTrueMotion").toBool();
    QByteArray data;
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/properties/AdvancedFunction.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data) {
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            QJsonValue value;
            QJsonObject obj;
            if (code == NOERROR) {
                if (UsingTrueMotion == true){
                    value = m_module->parseJsonData(data);
                    obj = value.toObject();
                    obj["UsingTrueMotion"] = UsingTrueMotion;
                    obj["InputShapingFreq"] = 0;
                    obj["InputShapingDamp"] = 0;
                } else {
                    value = m_module->parseJsonData(data);
                    obj = value.toObject();
                    obj["UsingTrueMotion"] = UsingTrueMotion;
                    obj["InputShapingFreq"] = 5.4348;
                    obj["InputShapingDamp"] = 0.1238;
                }
                m_fileControll->writeFile(id, "/project/properties/AdvancedFunction.json", obj);
                QObject *wobj = new QObject(this);
                connect(m_fileControll, &FileControll::onFinish_signal, wobj, [=](quint64 c_id, int code) {
                    if (id != c_id) {
                        return ;
                    } else {
                        wobj->deleteLater();
                        m_requestMap.remove(id);
                        if (code == NOERROR) {
                            QJsonObject res;
                            res.insert("status", true);
                            emit onReplyMessage_signal(id, res);
                        } else {
                            emit onErrorOccured_signal(id, code);
                        }
                    }
                });
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, ERROR_SERIALPORT_READ);
            }
        }

    });
}

void Device::pGetTruemotion(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/properties/AdvancedFunction.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }

    });
}

void Device::pSetPallet(quint64 id, QJsonObject params)
{
    QJsonArray dataObj = params.value("data").toArray();
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/process/pallet/matrixParams.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        }
        obj->disconnect();
        obj->deleteLater();
        m_requestMap.remove(id);
        if (code == NOERROR) {
            emit onReplyMessage_signal(id, true);
            return ;
        }
        //写失败，可能是目录不存在，则创建
        QObject *objWrite = new QObject(this);
        m_fileControll->newFolderRecursive(id,"/project/process/pallet/");
        connect(m_fileControll, &FileControll::onFinish_signal, objWrite, [this, id, dataObj,objWrite](quint64 c_id, int code){
            if (id != c_id)
            {
                return;
            }
            objWrite->disconnect();
            objWrite->deleteLater();
            if (code != NOERROR)
            {//创建目录也失败了，那就真的失败了
                emit onErrorOccured_signal(id, code);
                return;
            }
            //再次写文件
            QObject *obj = new QObject(this);
            m_fileControll->writeFile(c_id, "/project/process/pallet/matrixParams.json", dataObj);
            connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
                if (id != c_id) {
                    return ;
                }
                obj->disconnect();
                obj->deleteLater();
                if (code == NOERROR) {
                    emit onReplyMessage_signal(id, true);
                }else{
                    emit onErrorOccured_signal(id, code);
                }
            });
        });
    });
}

void Device::pGetPallet(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);

    m_fileControll->readFile(id, "/project/process/pallet/matrixParams.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            m_requestMap.remove(id);
            obj->deleteLater();
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pSetWiFiParams(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    checkStringValue(dataObj, "ssid");
    checkStringValue(dataObj, "passwd");

    m_module->sendPostRequest("/interface/setAP", dataObj, id, "SetWiFiParams");
}

void Device::pGetFunctionScriptParams(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/settings/function/scriptParams", id, "GetFunctionScriptParams");
}

/* 32.扩展轴 */
void Device::pAuxSetSwitch(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/process/auxJoint/switch", dataObj, id, "AuxSetSwitch");
}

void Device::pAuxGetSwitch(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/process/auxJoint/switch", id, "AuxGetSwitch");
}

void Device::pAuxSetJogCmd(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/panel/auxjog", dataObj, id, "AuxSetJogCmd");
}

void Device::pAuxSetTeachJoint(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/settings/teach/auxJoint", dataObj, id, "AuxSetTeachJoint");
}

void Device::pAuxGetTeachJoint(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/settings/teach/auxJoint", id, "AuxGetTeachJoint");
}

void Device::pAuxSetPlaybackJoint(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/settings/playback/auxJoint", dataObj, id, "AuxSetPlaybackJoint");
}

void Device::pAuxGetPlaybackJoint(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/settings/playback/auxJoint", id, "AuxGetPlaybackJoint");
}

void Device::pAuxSetStructure(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/process/auxJoint/structure", dataObj, id, "AuxSetStructure");
}

void Device::pAuxGetStructure(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    m_module->sendGetRequest("/process/auxJoint/structure", id, "AuxGetStructure");
}

void Device::pSetHotkey(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/hot_key.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                emit onReplyMessage_signal(id, true);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetHotkey(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QByteArray data;
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/hot_key.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pSetCustomPoint(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/function/CustomPoint.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                emit onReplyMessage_signal(id, true);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetCustomPoint(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QByteArray data;
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/function/CustomPoint.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pSetMoveJCmd(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/moveJ", dataObj, id, "SetMoveJCmd");
}

void Device::pGetMoveJCmd(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    m_module->sendGetRequest("/interface/moveJ", id, "GetMoveJCmd");
}

void Device::pSetPackPoint(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/function/PackPoint.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                emit onReplyMessage_signal(id, true);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetPackPoint(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QByteArray data;
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/function/PackPoint.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pSetJumpStructure(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/properties/structure.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                emit onReplyMessage_signal(id, true);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetJumpStructure(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QByteArray data;
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/properties/structure.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pSetSpeedDefault(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/properties/default.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                emit onReplyMessage_signal(id, true);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetSpeedDefault(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QByteArray data;
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/properties/default.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pSetApiUpdate(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/apiUpdate", dataObj, id, "SetApiUpdate");
}

void Device::pSetGlobalVar(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/project/globalVar.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/debugger/globalVar", dataObj, id, "SetGlobalVar");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetGlobalVar(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    m_module->sendGetRequest("/debugger/globalVar", id, "GetGlobalVar");
}

void Device::pSetCalcInstall(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/settings/calcInstall", dataObj, id, "SetCalcInstall");
}

void Device::pSetFunctionInstall(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/function/install.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/function/install", dataObj, id, "SetFunctionInstall");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetFunctionInstall(quint64 id, QJsonObject params)
{
/*
控制器对这个数据的读写存在不同步，连续下发数据，会出现：下发Post http之后，Get http回来的数据是同步的，但samba中对应的json文件还不同步。
*/
    Q_UNUSED(params)
    m_module->sendGetRequest("/settings/function/install", id, "GetFunctionInstall",[=](const QJsonValue& value, QString strUrl){
        Q_UNUSED(strUrl);
        emit onReplyMessage_signal(id, value);
        m_requestMap.remove(id);
    },[=](int iErrCode, QString strErrMsg){
        Q_UNUSED(iErrCode)
        Q_UNUSED(strErrMsg)
        QObject *pobj = new QObject(this);
        m_fileControll->readFile(id, "/project/settings/function/install.json");
        connect(m_fileControll, &FileControll::onFinish_signal, pobj, [=](quint64 c_id, int code, QByteArray data){
            if (id != c_id) {
                return ;
            } else {
                pobj->deleteLater();
                m_requestMap.remove(id);
                if (code == NOERROR) {
                    QJsonValue value = m_module->parseJsonData(data);
                    emit onReplyMessage_signal(id, value);
                } else {
                    emit onErrorOccured_signal(id, code);
                }
            }
        });
    });
}

void Device::pSetHardwareEnable(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/hardware", dataObj, id, "SetHardwareEnable");
}

void Device::pGetDragPlayback(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    _getStatus(id, "GetDragPlayback");
}

void Device::pGetSkinCollison(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    _getStatus(id, "GetSkinCollison");
}

void Device::pGetSkinValue(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    _getStatus(id, "GetSkinValue");
}

void Device::pGetControlParams(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    _getStatus(id, "GetControlParams");
}

void Device::pGetJointCurrent(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    _getStatus(id, "GetJointCurrent");
}

void Device::pGetJointVoltage(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    _getStatus(id, "GetJointVoltage");
}

void Device::pGetJointTemp(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    _getStatus(id, "GetJointTemp");
}

void Device::pGetRDNCoordinate(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    _getStatus(id, "GetRDNCoordinate");
}

void Device::pGetGPIOAIField(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    _getStatus(id, "GetGPIOAIField");
}

void Device::pGetEndAIField(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    _getStatus(id, "GetEndAIField");
}

void Device::pSetRetraceParams(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();
    QObject *obj = new QObject(this);
    m_fileControll->writeFile(id, "/project/settings/function/reTraceParams.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            if (code == NOERROR) {
                m_module->sendPostRequest("/settings/function/reTraceParams", dataObj, id, "SetRetraceParams");
            } else {
                m_requestMap.remove(id);
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetRetraceParams(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/settings/function/reTraceParams.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pSetDragAndTeach(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/panel/threeSwitch", dataObj, id, "SetDragAndTeach");
}

void Device::pGetDragTeachFileList(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    QObject *obj = new QObject(this);
    m_fileControll->getFullFileNameList(id, "/project/process/trajectory/", QStringList("*.json"));
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray array, QJsonValue value){
        if (id != c_id)
        {
            return ;
        }
        else
        {
            m_requestMap.remove(id);
            obj->deleteLater();
            if (code == NOERROR)
            {
                emit onReplyMessage_signal(id, value);
            }
            else
            {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pDeleteDragTeachFile(quint64 id, QJsonObject params)
{
    QStringList delFiles;
    QJsonArray dataObj = params.value("data").toArray();
    foreach (QJsonValue value, dataObj)
    {
        delFiles << QString("/project/process/trajectory/%1").arg(value.toString());
    }

    QObject *obj = new QObject(this);
    m_fileControll->DeleteFileName(id, delFiles);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray array){
        if (id != c_id)
        {
            return ;
        }
        else
        {
            m_requestMap.remove(id);
            obj->deleteLater();
            if (code == NOERROR)
            {
                emit onReplyMessage_signal(id, QJsonValue("true"));
            }
            else
            {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pSetForwardCal(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/ForwardCal", dataObj, id, "SetForwardCal");
}

void Device::pSetInverseCal(quint64 id, QJsonObject params)
{
    QJsonObject dataObj = params.value("data").toObject();

    m_module->sendPostRequest("/interface/InverseCal", dataObj, id, "SetInverseCal");
}

void Device::pGetSafeSignal(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)
    m_module->sendGetRequest("/settings/function/safeSignal", id, "GetSafeSignal");
}

void Device::pSetSafeSignal(quint64 id, QJsonObject params)
{
    QJsonValue dataObj = params.value("data");

    m_module->sendPostRequest("/settings/function/safeSignal", dataObj, id, "SetSafeSignal");
}

void Device::_sendStatus(quint64 id, const RobotStatus status, QString api)
{
    m_module->sendPostRequest("/protocol/exchange", status.getObjForSend(), id, api);
}

void Device::pCheckSamba(quint64 id, QJsonObject params)
{
    m_requestMap.remove(id);
    // judge system is win7 or win10
    QSysInfo::WinVersion ver = QSysInfo::windowsVersion();
    if (ver == QSysInfo::WV_WINDOWS7) {
        QSettings settings("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Lsa", QSettings::NativeFormat);
        int value = settings.value("LmCompatibilityLevel").toInt();
        if (value != 1) {
            emit onReplyMessage_signal(id, false);
        } else {
            emit onReplyMessage_signal(id, true);
        }
    } else if (ver == QSysInfo::WV_WINDOWS10) {
        QString ip = params.value("portName").toString();
        QString path = QString("\\\\%1%2").arg(ip).arg("/project/properties");

        auto* pThd = new QThread(0);
        connect(pThd, &QThread::started,[pThd, id, ip, this, path]{
            QDir dir(path);
            if (dir.exists("controllerType.json")) {
                emit onReplyMessage_signal(id, true);
            } else {
                emit onReplyMessage_signal(id, false);
            }
            pThd->quit();
        });
        connect(pThd,&QThread::finished,pThd,&QThread::deleteLater);
        pThd->start();
    }
}

void Device::pOpenSamba(quint64 id, QJsonObject params)
{
    m_requestMap.remove(id);
    // judge system is win7 or win10
    QSysInfo::WinVersion ver = QSysInfo::windowsVersion();
    if (ver == QSysInfo::WV_WINDOWS7) {
        qDebug() << Q_FUNC_INFO;
        QSettings settings("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Lsa", QSettings::NativeFormat);
        int iValue = settings.value("LmCompatibilityLevel").toInt();
        if (iValue != 1)
        {//如果不是1，就修改注册表
            settings.setValue("LmCompatibilityLevel", 1);
            iValue = settings.value("LmCompatibilityLevel").toInt();
            //可能因为权限问题导致修改注册表失败，所以在此判断是否修改成功
            if (iValue != 1)
            {
                emit onReplyMessage_signal(id, false);
            }
            else
            {
                emit onReplyMessage_signal(id, true);
            }
        }
        else
        {
            emit onReplyMessage_signal(id, true);
        }
    } else if (ver == QSysInfo::WV_WINDOWS10){
        QString ip = params.value("portName").toString();
        auto pThd = new QThread(0);
        connect(pThd, &QThread::started,[pThd, id, ip, this]{
            //先检查Samba是否已经启动了，如果启动，则没必要重复操作，否则可能因为各种权限问题导致操作失败，误判Samba启动失败。
            QString path = QString("\\\\%1%2").arg(ip).arg("/project/properties");
            QDir dir(path);
            if (dir.exists("controllerType.json"))
            {
                emit onReplyMessage_signal(id, true);
                pThd->quit();
                return ;
            }

            QSharedPointer<QString> sptrResult(new QString);
            QProcess *p = new QProcess(this);
            connect(p, &QProcess::readyRead, [p, sptrResult](){
                QString str =QString::fromLocal8Bit(p->readAllStandardOutput());
                sptrResult->append(str);
                qDebug() << __FUNCTION__ << str;
                if (str.contains("(Y/N)") or str.contains("completed successfully") or str.contains("成功完成"))
                {
                    //这里如果不杀掉cmd进程，那么将会一直阻塞，等待用户输入
                    p->terminate();
                    p->kill();
                }
            });
            p->start("cmd", QStringList()<<"/c"<<"Dism /online /Enable-Feature /FeatureName:SMB1Protocol");
            if (p->waitForFinished(50000))
            {
                if (sptrResult->contains("(Y/N)")
                   || sptrResult->contains("completed successfully")
                   || sptrResult->contains("成功完成"))
                {
                    emit onReplyMessage_signal(id, true);
                }
                else
                {//如果没有权限，启动Samba也会失败的
                    emit onReplyMessage_signal(id, false);
                }
            }
            else
            {
                qDebug() << p->errorString();
                emit onErrorOccured_signal(id, ERROR_INDUSTRY_SAMBA_FAILED);
            }
            p->deleteLater();

            pThd->quit();
        });
        connect(pThd,&QThread::finished,pThd,&QThread::deleteLater);
        pThd->start();
    }
}

void Device::pOpenNetUse(quint64 id, QJsonObject params)
{
    m_requestMap.remove(id);
    checkStringValue(params, "portName");
    QString ip = params.value("portName").toString();
    QProcess *p = new QProcess(this);
    QString cmd = QString("net use \\%1 \"dobot\" /user:\"root\" ").arg(ip);
    p->start(cmd);
    p->waitForFinished();
    emit onReplyMessage_signal(id, true);
    p->deleteLater();
}

void Device::pGetDeviceName(quint64 id, QJsonObject params)
{
    m_requestMap.remove(id);
    checkStringValue(params, "portName");
    QString ip = params.value("portName").toString();
    QString url = QString("\\\\%1%2").arg(ip).arg("/project/properties/controllerType.json");
    QFile file(url);
    QFileInfo info(file);
    QByteArray data;
    if (info.exists()) {
        if (file.open(QFile::ReadOnly)) {
            data = file.readAll();
            file.close();
            QJsonObject res = m_module->parseJsonData(data).toObject();
            QString device = res.value("name").toString();
            QJsonObject devObj;
            devObj.insert("device", device);
            emit onReplyMessage_signal(id, devObj);
        } else {
            qDebug() << "file cannot open.";
            emit onErrorOccured_signal(id, ERROR_SERIALPORT_READ);
        }
    } else {
        qDebug() << "file don't exist.";
        emit onErrorOccured_signal(id, ERROR_SERIALPORT_READ);
    }
}

void Device::pRestartComputer(quint64 id, QJsonObject params)
{
    m_requestMap.remove(id);
    Q_UNUSED(params)
    Q_UNUSED(id)
    QProcess::execute("cmd", QStringList()<<"/c"<<"shutdown -r -t 0");
}

void Device::pOpenFireWall(quint64 id, QJsonObject params)
{
    m_requestMap.remove(id);
    Q_UNUSED(params)
    QString udpCmd = IndustrialRobotPlugin::GetFirewallInRuleCmdUdp();

    QString tcpCmd = IndustrialRobotPlugin::GetFirewallInRuleCmdTcp();

    QProcess *p = new QProcess(this);
    p->start(udpCmd);
    if (!p->waitForFinished(1000)) {
        qDebug() << "tcp cmd error:" << p->errorString();
        emit onErrorOccured_signal(id, ERROR_INDUSTRY_FIREWALL_FAILED);
    } else {
        p->start(tcpCmd);
        if(!p->waitForFinished(1000)) {
            qDebug() << "udp cmd error:" << p->errorString();
            emit onErrorOccured_signal(id, ERROR_INDUSTRY_FIREWALL_FAILED);
        } else {
            emit onReplyMessage_signal(id, true);
        }
    }
    p->deleteLater();
}

void Device::_getStatus(quint64 id, QString api)
{
    m_module->sendGetRequest("/protocol/exchange", id, api);
}

bool Device::checkIntValue(const QJsonObject &obj, const QString &value)
{
    if (!obj.contains(value)) {
        qDebug().noquote() << QString("'%1':missing").arg(value);
        return false;
    } else if (!obj.value(value).isDouble()) {
        qWarning().noquote() << QString("'%1':type error").arg(value);
        return false;
    }
    return true;
}

bool Device::checkFloatValue(const QJsonObject &obj, const QString &value)
{
    if (!obj.contains(value)) {
        qDebug().noquote() << QString("'%1':missing").arg(value);
        return false;
    } else if (!obj.value(value).isDouble()) {
        qWarning().noquote() << QString("'%1':type error").arg(value);
        return false;
    }
    return true;
}

bool Device::checkBoolValue(const QJsonObject &obj, const QString &value)
{
    if (!obj.contains(value)) {
        qDebug().noquote() << QString("'%1':missing").arg(value);
        return false;
    } else if (!obj.value(value).isBool()) {
        qWarning().noquote() << QString("'%1':type error").arg(value);
        return false;
    }
    return true;
}

bool Device::checkStringValue(const QJsonObject &obj, const QString &value)
{
    if (!obj.contains(value)) {
        qDebug().noquote() << QString("'%1':missing").arg(value);
        return false;
    } else if (!obj.value(value).isString()) {
        qWarning().noquote() << QString("'%1':type error").arg(value);
        return false;
    }
    return true;
}

bool Device::checkObjectValue(const QJsonObject &obj, const QString &value)
{
    if (!obj.contains(value)) {
        qDebug().noquote() << QString("'%1':missing").arg(value);
        return false;
    } else if (!obj.value(value).isObject()) {
        qWarning().noquote() << QString("'%1':type error").arg(value);
        return false;
    }
    return true;
}

bool Device::checkArrayValue(const QJsonObject &obj, const QString &value)
{
    if (!obj.contains(value)) {
        qDebug().noquote() << QString("'%1':missing").arg(value);
        return false;
    } else if (!obj.value(value).isArray()) {
        qWarning().noquote() << QString("'%1':type error").arg(value);
        return false;
    }
    return true;
}

/* 收到reply */
void Device::onReplyMessage_slot(QJsonValue value, QString url, quint64 id, QString api)
{
    // todo: bad idea
    if (api == "SetPrivateDebuggerStop") {
        return;
    }

    bool isSelfhandle = false;

    if (value.isArray()) {
        QJsonArray arr = value.toArray();
    } else {
        QJsonObject obj = value.toObject();
        if (m_eventloopMap.contains(id) && api == "GetPrivateDobotStatus") {
            QEventLoop *loop = m_eventloopMap.take(id);
            loop->setProperty("resObj", obj);
            loop->quit();
        }

        isSelfhandle = hanleSelfFunction(url, api, id, obj);
    }

    if (isSelfhandle == false && api != "GetPrivateDobotStatus") {
        emit onReplyMessage_signal(id, value);
    }

    m_requestMap.remove(id);
}

bool Device::hanleSelfFunction(const QString url, const QString api, const quint64 id, const QJsonObject &obj)
{
    Q_UNUSED(url)

    if (m_selfExchangeFunMap.contains(api)) {
        handleSelfExchange(api, id, obj);
    } else if(api.contains("SetPrivateDebuggerStart")) {
        // 此处是为了防止开启mobdebug时 sendpost给上位机返回，需要在mobdebug的状态为client coneected才能返回
        return true;
    } else {
        return false;
    }
    return true;
}

void Device::handleSelfExchange(QString api, quint64 id, QJsonObject obj)
{
    Q_UNUSED(obj)

    QString key = m_selfExchangeFunMap.value(api);
    //    QVariant value = m_status.getStatus(key);

    QJsonObject statusTrue;
    statusTrue.insert("status", true);

    QJsonObject resObj;

    if (key.contains("toolCoordinate") or
        key.contains("userCoordinate"))
    {
        resObj.insert("value", obj.value(key).toInt());
    } else if (key.contains("controlMode") or
               key.contains("coordinate") or
               key.contains("jogMode") or
               key.contains("autoManual") or
               key.contains("prjState"))
    {
        resObj.insert("value", obj.value(key).toString());
    } else if (key.contains("isCollision"))
    {
        resObj.insert("value", obj.value(key).toInt());
    } else
    {
        resObj.insert("value", obj.value(key).toArray());
    }
    if (api.startsWith("Set")) {
        emit onReplyMessage_signal(id, statusTrue);
    } else {
        emit onReplyMessage_signal(id, resObj);
    }
}

void Device::onModebugMessage_slot(QString msg)
{
    Q_UNUSED(msg)

    qDebug() << __FUNCTION__ << "msg" << msg;
}

void Device::onModebugFinish_slot(quint64 id)
{
    qDebug() << __FUNCTION__;
    emit onReplyMessage_signal(id, true);
}

void Device::onErrorOccured_slot(quint64 id, int errCode, QString errStr)
{
    qDebug() << __FUNCTION__;
    emit onErrorOccured_signal(id, errCode, errStr);
}

void Device::onreadPendingClientMsg_slot(quint64 id, QString msg)
{
    QJsonObject resobj;
    resobj.insert("ClientMsg", msg);
    emit onNotifyMessage_signal(id, resobj);
}

void Device::onreadPending1stCursorMsg_slot(quint64 id, QString msg)
{
    QJsonObject resobj;
    resobj.insert("1stCursorMsg", msg);
    emit onNotifyMessage_signal(id, resobj);
}

void Device::onreadPending2ndCursorMsg_slot(quint64 id, QString msg)
{
    QJsonObject resobj;
    resobj.insert("2ndCursorMsg", msg);
    emit onNotifyMessage_signal(id, resobj);
}

void Device::onreadPendingSpecialMsg_slot(quint64 id, QString msg)
{
    QJsonObject resobj;
    resobj.insert("SpecialMsg", msg);
    emit onNotifyMessage_signal(id, resobj);
}

void Device::onreadBlockHighlightId_slot(quint64 id, QString msg)
{
    QJsonObject resobj;
    resobj.insert("BlockHighlightId", msg);
    emit onNotifyMessage_signal(id, resobj);
}

void Device::onSearch_slot(QJsonArray res, quint64 id)
{
    QJsonArray _res;
    if (m_isConnected) {
        foreach(const QJsonValue &obj, res) {
            QJsonObject json = obj.toObject();
            if (json.value("portName").toString() == m_module->ip()) {
                json.insert("status", "connected");
            }
            _res.append(json);
        }
    } else {
        _res = res;
    }

    m_portFCSmb.clear();
    foreach(const QJsonValue &obj, res) {
        QJsonObject json = obj.toObject();
        int smbType = json.value("smbType").toInt(1);
        QString portName = json.value("portName").toString();

        m_portFCSmb.insert(portName, smbType);
    }

    emit onReplyMessage_signal(id, _res);
}

void Device::pSetIONote(quint64 id, QJsonObject params)
{
    QJsonValue dataObj = params.value("data");
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);

    m_fileControll->writeFile(id, "/project/project/IONote.json", dataObj);
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                emit onReplyMessage_signal(id, true);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}

void Device::pGetIONote(quint64 id, QJsonObject params)
{
    Q_UNUSED(params)

    QByteArray data;
    QString ip = params.value("portName").toString();
    QObject *obj = new QObject(this);
    m_fileControll->readFile(id, "/project/project/IONote.json");
    connect(m_fileControll, &FileControll::onFinish_signal, obj, [=](quint64 c_id, int code, QByteArray data){
        if (id != c_id) {
            return ;
        } else {
            obj->deleteLater();
            m_requestMap.remove(id);
            if (code == NOERROR) {
                QJsonValue value = m_module->parseJsonData(data);
                emit onReplyMessage_signal(id, value);
            } else {
                emit onErrorOccured_signal(id, code);
            }
        }
    });
}
