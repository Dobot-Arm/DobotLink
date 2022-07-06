#include "MicrobitPlugin.h"

#include <QCoreApplication>
#include <QStandardPaths>
#include <QDebug>

const QString MicrobitPlugin::PluginName = "Microbit";
const QString MicrobitPlugin::Version = "1.0.0";

MicrobitPlugin::MicrobitPlugin(QObject *parent) : DPluginInterface(parent)
{
    mSourceType = MICROBIT_PYTHON;

    _processInit();
    _codeFileInit();
}

MicrobitPlugin::~MicrobitPlugin()
{

}

QString MicrobitPlugin::getVersion()
{
    return Version;
}

void MicrobitPlugin::_processInit()
{
    workSpaceDir.setPath(QCoreApplication::applicationDirPath());
    workSpaceDir.cdUp();
    workSpaceDir.cd("Plugins");
    workSpaceDir.cd("MicrobitPlugin");
    workSpaceDir.cd("microbit");

    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->setWorkingDirectory(workSpaceDir.absolutePath());
    m_process->setProgram(workSpaceDir.absoluteFilePath("uflash.exe"));

    connect(m_process, &QProcess::readyRead, this, &MicrobitPlugin::onProcessReadyRead_slot);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MicrobitPlugin::onProcessFinished_slot);
    connect(m_process, &QProcess::errorOccurred, this, &MicrobitPlugin::errorHandle_slot);
}

void MicrobitPlugin::_codeFileInit()
{
    QDir codeDir;
    codeDir.setPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

    if (!codeDir.exists("dobotlink-code")) {
        codeDir.mkdir("dobotlink-code");
    }
    codeDir.cd("dobotlink-code");
    if (!codeDir.exists("mbit-build")) {
        codeDir.mkdir("mbit-build");
    }
    codeDir.cd("mbit-build");

    QString fileNameStr = "code";
    switch (mSourceType) {
    case MICROBIT_PYTHON:
        fileNameStr.append(".py");
        break;
    case MICROBIT_JAVASCRIPT:
        fileNameStr.append(".js");
        break;
    case MICROBIT_HEX:
        fileNameStr.append(".hex");
        break;
    }
    codeFile.setFileName(codeDir.absoluteFilePath(fileNameStr));
}

/* 收到命令 */
void MicrobitPlugin::pReceiveMassage_slot(QString id, QJsonObject obj)
{
    if (id == "ALL") {
        qDebug() << "[ALL] {Microbit} get obj" << obj;
    } else if (id == PluginName) {
        MicrobitPacket packet;
        packet.setPacket(obj);

        if (packet.api == "MicrobitProgram") {
            if (handleProgram(packet) == true) {
                requstPacket = packet;
                startUpload();
            }
        }
    }
}

/* 开始上传 */
void MicrobitPlugin::startUpload()
{
    if (!codeFile.exists()) {
        qDebug() << "DMicrobitManager: code.py file is not exist.";
        return;
    }

    QStringList arguments;
    arguments << codeFile.fileName();

    m_process->setArguments(arguments);
    m_process->start();
    m_process->waitForFinished(10000);
}

bool MicrobitPlugin::handleProgram(const MicrobitPacket &packet)
{
    if (!packet.data.isEmpty()) {
        bool ok = writeIntoCodeFile(packet);
        return ok;
    } else {
        MicrobitResPacket resPacket(packet.id, packet.port);
        resPacket.setErrorObj(DataError, "data is empty");
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
        return false;
    }
}

bool MicrobitPlugin::writeIntoCodeFile(const MicrobitPacket &packet)
{
    if (!codeFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {

        MicrobitResPacket resPacket(packet.id, packet.port);
        resPacket.setErrorObj(FileOpenFail, "Open code file Failed" + codeFile.fileName());
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
        return false;
    }

    QTextStream out(&codeFile);
    out << packet.data;

    codeFile.close();
    return true;
}

/* SLOT */
void MicrobitPlugin::onProcessReadyRead_slot()
{
    QString readStr = QString::fromLatin1(m_process->readAll());

//    uploadLog.append(readStr);
    qDebug() << readStr;
}

void MicrobitPlugin::onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)

    MicrobitResPacket resPacket(requstPacket.id, requstPacket.port);
    if (exitStatus == QProcess::NormalExit) {
        resPacket.setResultObj();
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    } else {
        resPacket.setErrorObj(FileOpenFail, m_process->errorString());
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    }
}

void MicrobitPlugin::errorHandle_slot(QProcess::ProcessError error)
{
    qDebug() << "Microbit: ERROR:" << error;
}
