#include "debuggerplugin.h"
#include <QCoreApplication>
#include <QFile>
#include <QThread>
#include <QDebug>
#include <QStandardPaths>

#include "../version.h"

const QString DebuggerPlugin::PluginName = "Debugger";
const QString DebuggerPlugin::Version = VERSION_PLUGIN_DEBUGGER;
const QString processName = "debugger.exe";

DebuggerPlugin::DebuggerPlugin(QObject *parent):
    DPluginInterface (parent)
{
    m_process = nullptr;
    /*//暂时去掉
    * _processInit();
    setLog_dir();
    m_process->startDetached();
    */
}

DebuggerPlugin::~DebuggerPlugin()
{
    //_stopProcess();
}

QString DebuggerPlugin::getVersion()
{
    return  Version;
}

void DebuggerPlugin::_processInit()
{
    QString appPath = QCoreApplication::applicationDirPath().remove("_d");
    m_workSpaceDir.setPath(appPath);
#ifdef Q_OS_MAC
    m_workSpaceDir.cdUp();
    m_workSpaceDir.cd("Resources");
#endif
    if (m_workSpaceDir.exists("tool")) {
        m_workSpaceDir.cd("tool");
#ifdef Q_OS_WIN
        m_workSpaceDir.cd("debugger");
#elif defined (Q_OS_MAC)
        m_workSpaceDir.cd("debugger-mac");
#endif
    } else {
        qDebug() << "work space error:" << m_workSpaceDir;
    }

    m_process = new QProcess();
//    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->setWorkingDirectory(m_workSpaceDir.absolutePath());
    QDir dir(m_workSpaceDir);

#ifdef Q_OS_WIN
    QString program = "debugger.exe";
#elif defined (Q_OS_MAC)
    QString program = "debugger";
#endif

    if (dir.exists(program)) {
        m_processPath = dir.absoluteFilePath(program);
        m_process->setProgram(dir.absoluteFilePath(program));
    } else {
        qDebug() << "debugger:" << program << "path is not correct.";
    }
}

void DebuggerPlugin::_stopProcess()
{
    if(m_process){
        m_process->kill();
        m_process->terminate();
        m_process->deleteLater();
        m_process = nullptr;
    }
#ifdef Q_OS_WIN
        QProcess p;
        QString c = QString("taskkill /im %1 /f /t").arg(processName);
        p.execute(c);
        p.waitForFinished();
        p.close();
#elif defined (Q_OS_MAC)

#endif
}

void DebuggerPlugin::pReceiveMassage_slot(QString id, QJsonObject obj)
{
    if (id == "ALL") {
        qDebug() << "[ALL] {Debugger} get obj" << obj;
        if (obj.contains("METHOD")) {
            QString method = obj.value("METHOD").toString();
            if (method == "EXIT") {
                qDebug() << "DebuggerPlugin will EXIT.";
                _stopProcess();
            }
        }
    } else if (id == PluginName) {

    }

}

void DebuggerPlugin::setLog_dir()
{
#ifdef Q_OS_WIN
    logDir.setPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    logDir.cd("AppData");
    logDir.cd("Roaming");
#elif defined (Q_OS_MAC)
    logDir.setPath(QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation));
#elif defined (Q_OS_LINUX)
    logDir.setPath("/var");
    logDir.cd("log");
#endif

    if (!logDir.exists("DobotLink")) {
        logDir.mkdir("DobotLink");

    }
    if (logDir.cd("DobotLink")) {
        QString log_path = logDir.absolutePath();
        QStringList param;
        param<<"--log_dir"<<log_path<<"--log_level"<<"debug";
        m_process->setArguments(param);
    }
}
