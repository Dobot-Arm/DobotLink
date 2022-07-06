#include "DMultiFunc.h"

#include <QCoreApplication>
#include <QFile>
#include <QDebug>
const QString processName = "MultiFuncTool.exe";
DMultiFunc::DMultiFunc(QObject *parent) : QObject(parent)
{

}

DMultiFunc::~DMultiFunc()
{
    stopProcess();
}

void DMultiFunc::_processInit()
{
    QString appPath = QCoreApplication::applicationDirPath().remove("_d");
    m_workSpaceDir.setPath(appPath);
#ifdef Q_OS_MAC
    m_workSpaceDir.cdUp();
    m_workSpaceDir.cd("Resources");
#endif
    if (m_workSpaceDir.exists("tool")) {
        m_workSpaceDir.cd("tool");
        m_workSpaceDir.cd("multifunc");
#ifdef Q_OS_MAC
        m_workSpaceDir.cd("MultiFuncTool.app");
        m_workSpaceDir.cd("Contents");
        m_workSpaceDir.cd("MacOS");
#endif
    } else {
        qDebug() << "work space error:" << m_workSpaceDir;
    }

    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->setWorkingDirectory(m_workSpaceDir.absolutePath());

    connect(m_process, &QProcess::readyRead, this, &DMultiFunc::onProcessReadyRead_slot);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DMultiFunc::onProcessFinished_slot);
    connect(m_process, &QProcess::errorOccurred, this, &DMultiFunc::errorHandle_slot);

    QDir dir(m_workSpaceDir);

#ifdef Q_OS_WIN
    QString program = "MultiFuncTool.exe";
#else
    QString program = "MultiFuncTool";
#endif

    if (dir.exists(program)) {
        m_process->setProgram(dir.absoluteFilePath(program));
    } else {
        qDebug() << "MultiFuncTool:" << program << "path is not correct.";
    }
}

void DMultiFunc::setFileName(QString fileName)
{
    QString appPath = QCoreApplication::applicationDirPath().remove("_d");
    QDir dir(appPath);

#ifdef Q_OS_MAC
    dir.cdUp();
    dir.cd("Resources");
#endif

    if (!dir.exists("firmware")) {
        qDebug() << "firmware error.";
        return;
    }
    dir.cd("firmware");

    // 如果fileName是空的，那么使用firmware下的固件
    if (fileName.isEmpty()) {
        QStringList nameFilters;
        nameFilters << "*.bin";
        QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);

        QString typeName = "Car";

        foreach (QString file, files) {
            if (file.startsWith(typeName)) {
                m_fileName = dir.absoluteFilePath(file);
            }
        }
    } else {
        m_fileName = fileName;
    }
    qDebug() << m_fileName;
}

/* Command: "url" "COM" "version"  */
void DMultiFunc::startDownload(QString COM, QString version, quint64 start_id)
{
    this->start_id = start_id;
    _processInit();
    QFileInfo fileinfo(m_fileName);
    if (!fileinfo.exists()) {
        qDebug() << "Car file not exists";
        return;
    }

    QStringList arguments;
#ifdef Q_OS_WIN
    arguments << m_fileName << COM << version;
#elif defined (Q_OS_MAC)
    arguments << m_fileName << COM << version;
#endif

    m_process->setArguments(arguments);

    qDebug() << "program:" << m_process->program();
    qDebug() << "arguments:" << m_process->arguments();
    m_process->start();
}

void DMultiFunc::stopDownload(quint64 id)
{
    stopProcess();
    emit onStop_signal(id);
}

void DMultiFunc::stopProcess()
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

/* SLOT */
void DMultiFunc::onProcessReadyRead_slot()
{
    QString readStr = QString::fromLatin1(m_process->readAll());
    // 进度条到达100，返回给前端
    if (readStr == "100\r\n") {
        emit onProcessReadyRead_signal(readStr);
        emit onFinish_signal(true, start_id);
    } else if (readStr.contains("error")) {
        emit onProcessError_signal(start_id, ERROR_DOWNLOAD_FIRMWARE, readStr);
    } else {
        emit onProcessReadyRead_signal(readStr);
    }
}

void DMultiFunc::onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus)
{
//    auto process = qobject_cast<QProcess *>(sender());

    if (exitStatus == QProcess::NormalExit) {
        emit onFinish_signal(true, start_id);
    } else {
        emit onFinish_signal(false, start_id);
        qDebug() << "error, exitCode:" << exitCode << "exitStatus:" << exitStatus;
    }
}

void DMultiFunc::errorHandle_slot(QProcess::ProcessError error)
{
    qDebug() << "MultiFuncTool: QProcess ERROR:" << error;
    emit onProcessError_signal(start_id, error, "ProcessError");
}
