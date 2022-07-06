#include "CodingAgentplugin.h"
#include <QCoreApplication>
#include <QFile>
#include <QThread>
#include <QDebug>
#include <QStandardPaths>

#include "../version.h"

const QString CodingAgentPlugin::PluginName = "CodingAgentPlugin";
const QString CodingAgentPlugin::Version = VERSION_PLUGIN_CODING;
const QString processName = "CodingAgent.exe";

CodingAgentPlugin::CodingAgentPlugin(QObject *parent):
    DPluginInterface (parent)
{
    if(isCodingAgent()){
        qDebug() << "start CodingAgent server";
        _processInit();
        setPort(getPort());
        m_process->startDetached();
    }
}

CodingAgentPlugin::~CodingAgentPlugin()
{
    qDebug() << "close codingagentplugin";
    _stopProcess();
}

bool CodingAgentPlugin::isCodingAgent()
{
    QStringList argument = QCoreApplication::arguments();
    if(argument.count()==2 && argument.at(1).startsWith("-codingport")){
        return true;
    }
    return false;
}

QString CodingAgentPlugin::getPort()
{    
    QStringList argument = QCoreApplication::arguments();
    return argument.at(1);
}

QString CodingAgentPlugin::getVersion()
{
    return  Version;
}

void CodingAgentPlugin::_processInit()
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
        m_workSpaceDir.cd("codingagent");
#elif defined (Q_OS_MAC)
        m_workSpaceDir.cd("dynamical-mac");
#endif
    } else {
        qDebug() << "work space error:" << m_workSpaceDir;
    }

    m_process = new QProcess();
//    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->setWorkingDirectory(m_workSpaceDir.absolutePath());
    QDir dir(m_workSpaceDir);

#ifdef Q_OS_WIN
    QString program = "CodingAgent.exe";
#elif defined (Q_OS_MAC)
    QString program = "dynamicAlgorithm";
#endif

    if (dir.exists(program)) {
        m_processPath = dir.absoluteFilePath(program);
        m_process->setProgram(dir.absoluteFilePath(program));
    } else {
        qDebug() << "codingagent:" << program << "path is not correct.";
    }
}


void CodingAgentPlugin::_stopProcess()
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

void CodingAgentPlugin::pReceiveMassage_slot(QString id,QJsonObject obj)
{
    if (id == "ALL") {
        qDebug() << "[ALL] {CodingAent} get obj" << obj;
        if (obj.contains("METHOD")) {
            QString method = obj.value("METHOD").toString();
            if (method == "EXIT") {
                qDebug() << "CodingAentPlugin will EXIT.";
                _stopProcess();
            }
        }
    } else if (id == PluginName) {

    }
}

void CodingAgentPlugin::setPort(QString port)
{
    Q_UNUSED(port)
    QStringList params;
    params << getPort();
    m_process->setArguments(params);
}












