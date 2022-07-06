#include "PyImageOMPlugin.h"
#include <QCoreApplication>
#include <QFile>
#include <QThread>
#include <QDebug>
#include <QStandardPaths>
#include "../version.h"
const QString PyImageOMPlugin::PluginName = "PyImageOM";
const QString PyImageOMPlugin::Version = VERSION_PLUGIN_IMAGE;
const QString processName = "PyImageOM.exe";

PyImageOMPlugin::PyImageOMPlugin(QObject *parent) : 
    DPluginInterface(parent)
{
    _processInit();
    setLog_dir();
    m_process->startDetached();
}

PyImageOMPlugin::~PyImageOMPlugin()
{
    _stopProcess();
}


QString PyImageOMPlugin::getVersion()
{
    return Version;
}

void PyImageOMPlugin::_processInit()
{
    QString appPath = QCoreApplication::applicationDirPath().remove("_d");
    m_workSpaceDir.setPath(appPath);
#ifdef Q_OS_MAC
    m_workSpaceDir.cdUp();
    m_workSpaceDir.cd("Resources");
#endif
    if (m_workSpaceDir.exists("tool")) {
        m_workSpaceDir.cd("tool");
        m_workSpaceDir.cd("pyimageom");
    } else {
        qDebug() << "work space error:" << m_workSpaceDir;
    }

    m_process = new QProcess();
//    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->setWorkingDirectory(m_workSpaceDir.absolutePath());
    QDir dir(m_workSpaceDir);

#ifdef Q_OS_WIN
    QString program = "PyImageOM.exe";
#elif (defined (Q_OS_MAC) || defined (Q_OS_LINUX))
    QString program = "PyImageOM";
    QStringList envs = {
        "LC_ALL=en_US.utf-8",
        "LANG=en_US.utf-8"
    };
    m_process->setEnvironment(envs);
#endif

    if (dir.exists(program)) {
        m_processPath = dir.absoluteFilePath(program);
        m_process->setProgram(dir.absoluteFilePath(program));
    } else {
        qDebug() << "pyimageom:" << program << "path is not correct.";
    }
}

void PyImageOMPlugin::_stopProcess()
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
#elif (defined (Q_OS_MAC) || defined (Q_OS_LINUX))
        QProcess p;
        QFileInfo fileInfo(processName);
        QString c = QString("pkill -f %1").arg(fileInfo.baseName());
        p.execute(c);
        p.waitForFinished();
        p.close();
#endif
}

void PyImageOMPlugin::pReceiveMassage_slot(QString id, QJsonObject obj)
{
    if (id == "ALL") {
        qDebug() << "[ALL] {PyImageOM} get obj" << obj;
        if (obj.contains("METHOD")) {
            QString method = obj.value("METHOD").toString();
            if (method == "EXIT") {
                qDebug() << "PyImageOMPlugin will EXIT.";
                _stopProcess();
            }
        }
    } else if (id == PluginName) {

    }

}

void PyImageOMPlugin::setLog_dir()
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
