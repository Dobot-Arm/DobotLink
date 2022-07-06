#include "DebuggerlitePlugin.h"
#include "DError/DError.h"

#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QDir>
#include <QFile>

#include "../version.h"

const QString DebuggerLitePlugin::PluginName = "Debuggerlite";
const QString DebuggerLitePlugin::Version = VERSION_PLUGIN_DEBUGGERLITE;


DebuggerLitePlugin::DebuggerLitePlugin(QObject *parent) :
    DPluginInterface(parent)
{
}

DebuggerLitePlugin::~DebuggerLitePlugin()
{
    _handleCloseWebSocket();
}

void DebuggerLitePlugin::_handleCloseWebSocket(const quint16 &wPort)
{
    QMapIterator<quint64, DProcess*> iter(m_procs);
    while (iter.hasNext())
    {
        iter.next();
        quint64 dpid = iter.key();
        DProcess *proc = iter.value();

        if (wPort == 0 || wPort == proc->wPort()) {
            if (proc->stop()) {
                delete proc;
                m_procs.remove(dpid);
            }
        }
    }
}

void DebuggerLitePlugin::stopProcess()
{
    QMapIterator<quint64, DProcess*> iter(m_procs);
    while (iter.hasNext())
    {
        iter.next();
        DProcess *proc = iter.value();
        proc->stopProcess();
    }
    DProcess::forceStopProcessAll();
}

QString DebuggerLitePlugin::getVersion()
{
    return Version;
}

/* 收到消息 */
void DebuggerLitePlugin::pReceiveMassage_slot(QString id, QJsonObject obj)
{
    if (id == "ALL") {
        qDebug() << "[ALL] {DebuggerlitePlugin} get obj" << obj;
        if (obj.contains("METHOD")) {
            QString method = obj.value("METHOD").toString();
            if (method == "EXIT") {
                stopProcess();
                _handleCloseWebSocket();
            } else if (method == "CloseWebSocket") {
                QJsonObject params = obj.value("params").toObject();
                quint16 wPort = static_cast<quint16>(params.value("WSport").toInt());
                _handleCloseWebSocket(wPort);
            }
        }
    } else if (id == PluginName) {
        DRequestPacket packet;
        packet.setPacketFromObj(obj);
        if (packet.api == "Start") {
            _handleStartCommand(packet);
        } else if (packet.api == "Prepare") {
            _handlePrepareCommand(packet);
        } else if (packet.api == "Wait") {
            _handleWaitCommand(packet);
        } else if (packet.api == "Stop") {
            _handleStopCommand(packet);
        } else if (packet.api == "EmergencyStop") {
            _handleEmergencyStopCommand(packet);
        } else if (packet.api == "Input") {
            _handleInputCommand(packet);
        } else if (packet.api == "SetupCustomModule"){
            _handleSetupCustomModuleCommand(packet);
        } else if (packet.api == "StartAll"){
            _handleStartAllCommand(packet);
        } else if (packet.api == "StopAll"){
            _handleStopAllCommand(packet);
        }  else if (packet.api == "GetProcessStatus"){
            _handleGetProcessStatusCommand(packet);
        } else {
            DResultPacket resPacket(packet.id, packet.wsPort);
            QJsonObject resultObj = resPacket.getErrorObjWithCode(ERROR_INVALID_METHOD);
            emit pSendMessage_signal(PluginName, resultObj);
        }
    }
}

void DebuggerLitePlugin::slotFinish()
{
    DProcess *proc = static_cast<DProcess*>(sender());

    /*
     * 1. 收到前端流程是：XXX--->_handlePrepareCommand--->_handleStartCommand--->_handleWaitCommand--->XXX
     * 2. 调试发现，当在_handleStartCommand时，启动脚本并执行，执行完毕后，DProcess发射了信号，DebuggerLitePlugin::slotFinish接收
     *    并处理该信号，因为proc->waitId()==0，所以不会通知前端。
     *    再后来，收到前端指令并执行_handleWaitCommand，结果这个等待里面啥也没做，就导致前端一直等待。
     * 3. 产生2的那个问题，是因为 DebuggerLitePlugin::slotFinish 先于 _handleWaitCommand 执行（可能是因为接口调用时间差和快慢导致），所以导致问题产生。
     * 4. 解决3的方式，就是进入事件等待循环，同时判断 _handleWaitCommand 有没有被执行。
    **/
    while (!proc->isWaitCmdFinished())
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
    }
    DResultPacket resPacket(proc->waitId(), proc->wPort());
    emit pSendMessage_signal(PluginName, resPacket.getResultObj());
}

void DebuggerLitePlugin::slotNotifyStdout(QJsonArray datas)
{
    DProcess *proc = static_cast<DProcess*>(sender());
    DNotificationPacket notPacket(proc->wPort());
    QJsonObject resObj;
    resObj.insert("dpid", proc->dpid());
    resObj.insert("msgs", datas);

    emit pSendMessage_signal(PluginName, notPacket.getNotificationObj("procOut", resObj));
}

void DebuggerLitePlugin::slotNotifyStderr(QJsonArray datas)
{
    DProcess *proc = static_cast<DProcess*>(sender());
    DNotificationPacket notPacket(proc->wPort());
    QJsonObject resObj;
    resObj.insert("dpid", proc->dpid());
    resObj.insert("msgs", datas);
    emit pSendMessage_signal(PluginName, notPacket.getNotificationObj("procErr", resObj));
}

void DebuggerLitePlugin::_handlePrepareCommand(const DRequestPacket &packet)
{
    DResultPacket resPacket(packet.id, packet.wsPort);
    QJsonObject resultObj;

    if (!packet.paramsObj.contains("script")) {
        resultObj = resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_INVALID_PARAMS);
        emit pSendMessage_signal(PluginName, resultObj);
        return;
    }

    QString base64Str = packet.paramsObj.value("script").toString();
    QString portName = packet.paramsObj.value("portName").toString();
    QString token = packet.paramsObj.value("token").toString();
    QString host = packet.paramsObj.value("host").toString();

    DProcess *proc = new DProcess(packet.wsPort);
    connect(proc, &DProcess::sigFinish, this, &DebuggerLitePlugin::slotFinish);
    connect(proc, &DProcess::sigNotifyStdout, this, &DebuggerLitePlugin::slotNotifyStdout);
    connect(proc, &DProcess::sigNotifyStderr, this, &DebuggerLitePlugin::slotNotifyStderr);

    if (proc->prepare(base64Str, portName, token, host)) {
        m_procs.insert(proc->dpid(), proc);
        QJsonObject resObj;
        resObj.insert("dpid", proc->dpid());
        emit pSendMessage_signal(PluginName, resPacket.getResultObj(resObj));
    } else {
        delete proc;
        resultObj = resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_PREPARE_FAILED);
        emit pSendMessage_signal(PluginName, resultObj);
    }
}

DProcess *DebuggerLitePlugin::_getProc(const DRequestPacket &packet)
{
    DResultPacket resPacket(packet.id, packet.wsPort);
    QJsonObject resultObj;

    if (!packet.paramsObj.contains("dpid")) {
        resultObj = resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_INVALID_PARAMS);
        emit pSendMessage_signal(PluginName, resultObj);
        return nullptr;
    }

    quint64 dpid = packet.paramsObj.value("dpid").toInt();

    if (!m_procs.contains(dpid)) {
        QJsonObject resultObj = resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_INVALID_DPID);
        emit pSendMessage_signal(PluginName, resultObj);
        return nullptr;
    }

    DProcess *proc = m_procs.value(dpid);

    if (proc->wPort() != packet.wsPort) {
        QJsonObject resultObj = resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_NO_PERMISSION);
        emit pSendMessage_signal(PluginName, resultObj);
        return nullptr;
    }

    return proc;
}

void DebuggerLitePlugin::_handleStartCommand(const DRequestPacket &packet)
{
    DResultPacket resPacket(packet.id, packet.wsPort);
    QJsonObject resultObj;

    DProcess *proc = _getProc(packet);
    if (proc == nullptr) return;

    if (!proc->start()) {
        resultObj = resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_START_FAILED);
        emit pSendMessage_signal(PluginName, resultObj);
        return;
    }

    emit pSendMessage_signal(PluginName, resPacket.getResultObj());
}

void DebuggerLitePlugin::_handleWaitCommand(const DRequestPacket &packet)
{
    DResultPacket resPacket(packet.id, packet.wsPort);
    QJsonObject resultObj;

    DProcess *proc = _getProc(packet);
    if (proc == nullptr) return;

    proc->setWaitId(packet.id);

    proc->setWaitCmdFinished(true);

    // 不要手贱在这里回包，等待指令的回包有特殊处理
}

void DebuggerLitePlugin::_handleStopCommand(const DRequestPacket &packet)
{
    DResultPacket resPacket(packet.id, packet.wsPort);
    QJsonObject resultObj;

    DProcess *proc = _getProc(packet);
    if (proc == nullptr) return;

    if (!proc->stop()) {
        QJsonObject resultObj = resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_NO_PERMISSION);
        emit pSendMessage_signal(PluginName, resultObj);
        return;
    }

    m_procs.remove(proc->dpid());
    delete proc;
    emit pSendMessage_signal(PluginName, resPacket.getResultObj());
}

void DebuggerLitePlugin::_handleEmergencyStopCommand(const DRequestPacket &packet)
{
    _handleStopCommand(packet);
}

void DebuggerLitePlugin::_handleInputCommand(const DRequestPacket &packet)
{
    DResultPacket resPacket(packet.id, packet.wsPort);
    QJsonObject resultObj;

    DProcess *proc = _getProc(packet);
    if (proc == nullptr) return;

    if (!packet.paramsObj.contains("cmd")) {
        resultObj = resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_INVALID_PARAMS);
        emit pSendMessage_signal(PluginName, resultObj);
        return;
    }

    QString base64Str = packet.paramsObj.value("cmd").toString();
    QByteArray base64Bytes = base64Str.toUtf8();
#ifdef Q_OS_WIN
    QString cmd = QString(QByteArray::fromBase64(base64Bytes)) + "\r\n";
    qDebug() << "Q_OS_WIN: cmd :" << cmd << endl;
#else
    QString cmd = QString(QByteArray::fromBase64(base64Bytes)) + "\n";
    qDebug() << "Q_OS_MAC: cmd :" << cmd << endl;
#endif

    if (!proc->write(cmd)) {
        DResultPacket resPacket(packet.id, packet.wsPort);
        QJsonObject resultObj = resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_WRITE_FAILD);
        emit pSendMessage_signal(PluginName, resultObj);
        return;
    }

    resultObj = resPacket.getResultObj();
    emit pSendMessage_signal(PluginName, resultObj);
}

void DebuggerLitePlugin::_handleSetupCustomModuleCommand(const DRequestPacket &packet)
{
    DResultPacket resPacket(packet.id, packet.wsPort);
    QJsonObject resultObj;

    if (!packet.paramsObj.contains("fileName") ||
        !packet.paramsObj.contains("fileContext")) {
        resultObj = resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_INVALID_PARAMS);
        emit pSendMessage_signal(PluginName, resultObj);
        return;
    }

    QString fileName = packet.paramsObj.value("fileName").toString();
    QString fileContextB64 = packet.paramsObj.value("fileContext").toString();

    QByteArray fileContextBytesB64 = fileContextB64.toUtf8();
    QString fileContext =  QString(QByteArray::fromBase64(fileContextBytesB64));

    QDir dirTarget;
#ifdef Q_OS_WIN
    dirTarget.setPath(qApp->applicationDirPath() + "/tool/py38");
#elif defined (Q_OS_MAC)
    dirTarget.setPath(qApp->applicationDirPath() + "/../Resources/tool/py38");
#elif defined (Q_OS_LINUX)
    dirTarget.setPath(qApp->applicationDirPath() + "/tool/py38");
#endif
    if (!dirTarget.exists("custom")) {
        if(!dirTarget.mkdir("custom")) {
            emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_SETUP_MODULE_FAILED));
            return;
        }
    }
    dirTarget.cd("custom");

    QString pathTarget;
    pathTarget = dirTarget.absolutePath().replace("\\", "/") + "/" + fileName;

    QFile file(pathTarget);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (file.write(fileContext.toUtf8())) {
            emit pSendMessage_signal(PluginName, resPacket.getResultObj());
        } else {
            emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_FILE_CANNOT_WRITE));
        }
        file.close();
    } else {
        emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_FILE_CANNOT_OPEN));
    }
}

void DebuggerLitePlugin::_handleStartAllCommand(const DRequestPacket &packet)
{
    DResultPacket resPacket(packet.id, packet.wsPort);
    QJsonObject resultObj;
    QJsonObject resObj;
    QJsonArray detialArry;

    QMapIterator<quint64, DProcess*> iter(m_procs);
    bool isSuccess(true);
    while (iter.hasNext())
    {
        iter.next();
        DProcess *proc = iter.value();

        if (proc->wPort() == packet.wsPort) {
            bool res = proc->start();
            if (!res) isSuccess = false;

            QJsonObject procObj;
            procObj.insert("dpid", proc->dpid());
            procObj.insert("result", res);

            detialArry.append(procObj);
        }
    }

    resObj.insert("detial", detialArry);
    resObj.insert("result", isSuccess);
    emit pSendMessage_signal(PluginName, resPacket.getResultObj(resObj));
}

void DebuggerLitePlugin::_handleStopAllCommand(const DRequestPacket &packet)
{
    DResultPacket resPacket(packet.id, packet.wsPort);
    QJsonObject resultObj;
    QJsonObject resObj;
    QJsonArray detialArry;

    QMapIterator<quint64, DProcess*> iter(m_procs);
    bool isSuccess(true);
    while (iter.hasNext())
    {
        iter.next();
        DProcess *proc = iter.value();

        if (proc->wPort() == packet.wsPort) {
            bool res = proc->stop();
            if (!res) isSuccess = false;

            QJsonObject procObj;
            procObj.insert("dpid", proc->dpid());
            procObj.insert("result", res);

            detialArry.append(procObj);
        }
    }

    resObj.insert("detial", detialArry);
    resObj.insert("result", isSuccess);
    emit pSendMessage_signal(PluginName, resPacket.getResultObj(resObj));
}

void DebuggerLitePlugin::_handleGetProcessStatusCommand(const DRequestPacket &packet)
{
    DResultPacket resPacket(packet.id, packet.wsPort);
    QJsonObject resultObj;
    QJsonArray procArry;

    QMapIterator<quint64, DProcess*> iter(m_procs);
    while (iter.hasNext())
    {
        iter.next();
        DProcess *proc = iter.value();

        if (proc->wPort() == packet.wsPort) {
            QJsonObject procObj;
            procObj.insert("dpid", proc->dpid());
            procObj.insert("isRunning", proc->isRunning());

            procArry.append(procObj);
        }
    }
    emit pSendMessage_signal(PluginName, resPacket.getResultObj(procArry));
}

