#include "RunCmdPlugin.h"
#include "DError/DError.h"

#include <QCoreApplication>
#include <QDebug>
#include <QProcess>
#include <QString>
#include <QDateTime>
#include <QTemporaryFile>
#include <QDir>
#include <QStandardPaths>
#include <QTextCodec>
#include <QTimer>

#include "../version.h"

const QString RunCmdPlugin::PluginName = "RunCmd";
const QString RunCmdPlugin::Version = VERSION_PLUGIN_RUNCMD;

RunCmdPlugin::RunCmdPlugin(QObject *parent) :
    DPluginInterface(parent),
    m_process(nullptr),
    m_waitResultPack(nullptr),
    m_bWaitCommandCalled(false),
    m_bStopCommandCalled(false)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &RunCmdPlugin::notifyStdOutTimeout_slot);
    timer->setInterval(100);
    timer->start();
}

RunCmdPlugin::~RunCmdPlugin()
{
    if (m_process) {
        _stopProcess();

        m_process->deleteLater();
        m_process = nullptr;
    }
}

QString RunCmdPlugin::getVersion()
{
    return Version;
}

/* 收到消息 */
void RunCmdPlugin::pReceiveMassage_slot(QString id, QJsonObject obj)
{
    if (id == "ALL") {
        qDebug() << "[ALL] {RunCmdPlugin} get obj" << obj;
        if (obj.contains("METHOD")) {
            QString method = obj.value("METHOD").toString();
            if (method == "EXIT" || method == "CloseWebSocket") {
                qDebug() << "RunCmdPlugin process will EXIT.";
                QJsonObject params = obj.value("params").toObject();
                quint16 port = static_cast<quint16>(params.value("WSport").toInt());
                _stopProcess(port);
            }
        }
    } else if (id == PluginName) {
        DRequestPacket packet;
        packet.setPacketFromObj(obj);
        if (packet.api == "Start") {
            handleStartCommand(packet);
        } else if (packet.api == "Wait") {
            handleWaitCommand(packet);
        } else if (packet.api == "Stop") {
            handleStopCommand(packet);
        } else {
            DResultPacket resPacket(packet.id, packet.wsPort);
            QJsonObject resultObj = resPacket.getErrorObjWithCode(ERROR_INVALID_METHOD);
            emit pSendMessage_signal(PluginName, resultObj);
        }
    }
}

void RunCmdPlugin::_stopProcess(const quint16 wPort, DResultPacket* const resPacket)
{
    m_bStopCommandCalled = true;
    if (m_process) {
        if(wPort == 0 || wPort == m_process->property("wPort").toUInt()) {
#ifdef Q_OS_WIN
            QProcess p;
            QString cmd("taskkill");
            QStringList arguments;
            arguments << "/T" << "/F" << "/PID" << QString::number(m_process->property("pid").toInt());

            qDebug() << __FUNCTION__ << cmd << arguments;

            p.execute(cmd, arguments);
            p.waitForFinished();
            p.close();
#elif (defined (Q_OS_MAC) || defined (Q_OS_LINUX))
            m_process->kill();
#endif
            m_process->kill();
            m_process->terminate();
            m_process->disconnect();
            m_process->close();

            // todo: 这里写的不好
            if (resPacket) {
                emit pSendMessage_signal(PluginName, resPacket->getResultObj());
                _sendWaitResult(0, QProcess::NormalExit);
            } else {
                notifyStdOutTimeout_slot();
                m_process->deleteLater();
                m_process = nullptr;
            }

        }
    }
}

void RunCmdPlugin::_notify(const QString method, const QJsonObject &resObj, const quint16 wPort)
{
    DNotificationPacket notPacket(wPort);
    emit pSendMessage_signal(PluginName, notPacket.getNotificationObj(method, resObj));
}

void RunCmdPlugin::notifyStdOutTimeout_slot()
{
    QProcess* const process = m_process;

    if (process == nullptr) return;
    QVariant data = process->property("stdoutDatas");
    if (data.isNull()) return;

    QJsonArray stdoutDatas = data.toJsonArray();
    if (stdoutDatas.size() > 0) {
        process->setProperty("stdoutDatas", QJsonArray());

        qint64 pid = process->property("pid").toUInt();
        quint16 wPort = process->property("wPort").toUInt();

        QJsonObject resObj;
        resObj.insert("pid", static_cast<int>(pid));
        resObj.insert("msg", stdoutDatas);

        _notify("procOut", resObj, wPort);
    }
}

void RunCmdPlugin::_notifyStdOut(const QString &data, QProcess* const process, const bool isFiltter)
{
    qint64 pid = process->property("pid").toUInt();
    quint16 wPort = process->property("wPort").toUInt();
    QJsonArray stdoutDatas = process->property("stdoutDatas").toJsonArray();

    if (!data.contains("Active code page: 437", Qt::CaseSensitive)) {
        QJsonObject msgObj;
        msgObj.insert("data", data);
        stdoutDatas.append(msgObj);

        if (!isFiltter) {
            process->setProperty("stdoutDatas", QJsonArray());

            QJsonObject resObj;
            resObj.insert("pid", static_cast<int>(pid));
            resObj.insert("msg", stdoutDatas);

            _notify("procOut", resObj, wPort);
        } else {
            process->setProperty("stdoutDatas", stdoutDatas);
        }
    }

}

void RunCmdPlugin::_notifyStdErr(const QString &data, const QProcess * const process)
{
    qint64 pid = process->property("pid").toUInt();
    quint16 wPort = process->property("wPort").toUInt();

    QJsonObject msgObj;
    msgObj.insert("data", QString(data));

    QJsonArray msgObjs;
    msgObjs.append(msgObj);

    QJsonObject resObj;
    resObj.insert("pid", static_cast<int>(pid));
    resObj.insert("msg", msgObjs);

    _notify("procErr", resObj, wPort);
}

void RunCmdPlugin::_sendWaitResult(int code, QProcess::ExitStatus status)
{
    Q_UNUSED(code)
    Q_UNUSED(status)


    /*
     * 1. 收到前端流程是：XXX--->_handlePrepareCommand--->_handleStartCommand--->_handleWaitCommand--->XXX
     * 2. 调试发现，当在_handleStartCommand时，启动脚本并执行，执行完毕后，DProcess发射了信号，DebuggerLitePlugin::slotFinish接收
     *    并处理该信号，因为proc->waitId()==0，所以不会通知前端。
     *    再后来，收到前端指令并执行_handleWaitCommand，结果这个等待里面啥也没做，就导致前端一直等待。
     * 3. 产生2的那个问题，是因为 DebuggerLitePlugin::slotFinish 先于 _handleWaitCommand 执行（可能是因为接口调用时间差和快慢导致），所以导致问题产生。
     * 4. 解决3的方式，就是进入事件等待循环，同时判断 _handleWaitCommand 有没有被执行。
    **/
    while (!m_bWaitCommandCalled && !m_bStopCommandCalled)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
    }

    if (m_waitResultPack) {
        emit pSendMessage_signal(PluginName, m_waitResultPack->getResultObj());
        delete m_waitResultPack;
        m_waitResultPack = nullptr;
    }

    notifyStdOutTimeout_slot();
    if(m_process) {
        m_process->deleteLater();
        m_process = nullptr;
    }
}

void RunCmdPlugin::processStdout_slot()
{
    QProcess *process = static_cast<QProcess*>(sender());

    QByteArray data = process->readAllStandardOutput();
    QString msg = QTextCodec::codecForName("GBK")->toUnicode(data);

    _notifyStdOut(msg, process);
}

void RunCmdPlugin::processStderr_slot()
{
    QProcess *process = static_cast<QProcess*>(sender());

    QByteArray data = process->readAllStandardError();
    QString msg = QTextCodec::codecForName("GBK")->toUnicode(data);

    _notifyStdErr(msg, process);
}

void RunCmdPlugin::processStarted_slot()
{
//    QProcess *process = static_cast<QProcess*>(sender());
//    quint64 pid = process->processId();
//    QString msg = QString("script running... proccess[%1]").arg(pid);
//    _notifyStdOut(msg, pid);
//    qDebug() << msg;
}

void RunCmdPlugin::processFinlish_slot(int code, QProcess::ExitStatus status)
{
    qDebug() << __FUNCTION__ << code << status;
    _sendWaitResult(code, status);
}

void RunCmdPlugin::handleStartCommand(const DRequestPacket &packet)
{
    cmd = packet.paramsObj.value("cmd").toString();
    QString language = packet.paramsObj.value("language").toString();

    DResultPacket resPacket(packet.id, packet.wsPort);
    QJsonObject resultObj;

    m_bWaitCommandCalled = false;
    m_bStopCommandCalled = false;
    if (m_process) {
        resultObj = resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_RUNNING);
    } else {
        m_process = new QProcess(this);

#if defined (Q_OS_MAC)
        QString pyPath = qApp->applicationDirPath() + "/../Resources/tool/py38/bin";
        m_process->setWorkingDirectory(pyPath);

        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("PATH", qApp->applicationDirPath() + ":" + env.value("PATH"));
        m_process->setProcessEnvironment(env);

        qDebug() << "Process env:" << m_process->environment();
        qDebug() << "Process workspace:" << m_process->workingDirectory();
        qDebug() << "Process cmd:" << cmd;
        m_process->start(cmd);
#elif defined (Q_OS_LINUX)
        QString pyPath = qApp->applicationDirPath() + "/tool/py38/";
        m_process->setWorkingDirectory(pyPath);

        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("PATH", qApp->applicationDirPath() + ":" + env.value("PATH"));
        m_process->setProcessEnvironment(env);

        qDebug() << "Process env:" << m_process->environment();
        qDebug() << "Process workspace:" << m_process->workingDirectory();
        qDebug() << "Process cmd:" << cmd;
        m_process->start(cmd);
#else
        QStringList args;
        if (language == "en") {
            args << "/c" << "chcp 437 && cmd /c " << cmd;
        } else {
            args << "/c" << cmd;
        }
        m_process->setProgram("cmd");
        m_process->setArguments(args);

        QString dir_path = qApp->applicationDirPath() + "/tool/py38/Scripts";
        m_process->setWorkingDirectory(dir_path);

        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("PATH", qApp->applicationDirPath() + ";" + env.value("PATH"));
        m_process->setProcessEnvironment(env);

        qDebug() << "Process env:" << m_process->environment();
        qDebug() << "Process workspace:" << m_process->workingDirectory();
        qDebug() << "Process cmd:" << m_process->program() << m_process->arguments();
        m_process->start();
#endif

        bool isSuccess = m_process->waitForStarted(1000);

        if (isSuccess) {
            connect(m_process, &QProcess::readyReadStandardOutput, this, &RunCmdPlugin::processStdout_slot);
            connect(m_process, &QProcess::readyReadStandardError, this, &RunCmdPlugin::processStderr_slot);
            connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                    this, &RunCmdPlugin::processFinlish_slot);
            connect(m_process, &QProcess::started, this, &RunCmdPlugin::processStarted_slot);

            QJsonObject resObj;
            resObj.insert("pid", m_process->processId());
            resultObj = resPacket.getResultObj(resObj);
            emit pSendMessage_signal(PluginName, resultObj);

            m_process->setProperty("pid", m_process->processId());
            m_process->setProperty("wPort", packet.wsPort);
            m_process->setProperty("stdoutDatas", QJsonArray());

        } else {
            resultObj = resPacket.getErrorObj(ERROR_DEBUGGERLITE_START_FAILED, m_process->errorString());
            m_process->close();
            m_process->deleteLater();
            m_process = nullptr;
            emit pSendMessage_signal(PluginName, resultObj);
        }

}
}

void RunCmdPlugin::handleWaitCommand(const DRequestPacket &packet)
{
    m_bWaitCommandCalled = true;
    if (!_checkProcess(packet)) return;

    m_waitResultPack = new DResultPacket(packet.id, packet.wsPort);
}

void RunCmdPlugin::handleStopCommand(const DRequestPacket &packet)
{
    if (!_checkProcess(packet)) return;

    DResultPacket resPacket(packet.id, packet.wsPort);
    _stopProcess(0, &resPacket);
}

bool RunCmdPlugin::_checkProcess(const DRequestPacket &packet)
{
    if(!m_process) {
        DResultPacket resPacket(packet.id, packet.wsPort);
        QJsonObject resultObj = resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_PROCESS_INVALID);
        emit pSendMessage_signal(PluginName, resultObj);
        return false;
    }

    qint64 pid = packet.paramsObj.value("pid").toDouble();
    if (pid != m_process->property("pid").toInt()) {
        DResultPacket resPacket(packet.id, packet.wsPort);
        QJsonObject resultObj = resPacket.getErrorObjWithCode(ERROR_DEBUGGERLITE_INVALID_DPID);
        emit pSendMessage_signal(PluginName, resultObj);
        return false;
    }

    return true;
}

