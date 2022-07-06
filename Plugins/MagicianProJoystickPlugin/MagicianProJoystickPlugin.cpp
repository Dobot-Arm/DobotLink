#include "MagicianProJoystickPlugin.h"
#include <QCoreApplication>
#include <QFile>
#include <QThread>
#include <QDebug>
#include <QStandardPaths>
#include "../version.h"
const QString MagicianProJoystickPlugin::PluginName = "MagicianProJoystick";
const QString MagicianProJoystickPlugin::Version = "1.0.0";
const QString processName = "magicianprojoystick.exe";

MagicianProJoystickPlugin::MagicianProJoystickPlugin(QObject *parent):
    DPluginInterface (parent)
{
    m_process = nullptr;
    /*//暂时去掉
     * _processInit();
    setLog_dir();
    m_process->startDetached();*/
}

MagicianProJoystickPlugin::~MagicianProJoystickPlugin()
{
    //_stopProcess();
}


QString MagicianProJoystickPlugin::getVersion()
{
    return Version;
}

void MagicianProJoystickPlugin::_processInit()
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
        m_workSpaceDir.cd("magicianprojoystick");
#elif defined (Q_OS_MAC)
        m_workSpaceDir.cd("magicianprojoystick-mac");
#endif
    } else {
        qDebug() << "work space error:" << m_workSpaceDir;
    }

    m_process = new QProcess();
    // m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->setWorkingDirectory(m_workSpaceDir.absolutePath());
    QDir dir(m_workSpaceDir);

#ifdef Q_OS_WIN
    QString program = "magicianprojoystick.exe";
#elif defined (Q_OS_MAC)
    QString program = "magicianprojoystick";
#endif

    if (dir.exists(program)){
        m_processPath = dir.absoluteFilePath(program);
        m_process->setProgram(dir.absoluteFilePath(program));
    }else {
        qDebug() << "MagicianProJoystick: " << program << "path is not correct";

    }
}

void MagicianProJoystickPlugin::_stopProcess()
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
        QProcess p;
        QFileInfo fileInfo(processName);
        QString c = QString("pkill -f %1").arg(fileInfo.baseName());
        p.execute(c);
        p.waitForFinished();
        p.close();
#endif
}


void MagicianProJoystickPlugin::pReceiveMassage_slot(QString id,QJsonObject obj)
{
    if (id == "ALL") {
        qDebug() << "[ALL] {MagicianProJoystick} get obj" << obj;
        if (obj.contains("METHOD")) {
            QString method = obj.value("METHOD").toString();
            if (method == "EXIT") {
                qDebug() << "MagicianProJoystickPlugin will EXIT.";
                _stopProcess();
            }
        }
    } else if (id == PluginName) {

    }
}

void MagicianProJoystickPlugin::setLog_dir()
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
