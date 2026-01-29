#include "DKflash.h"

#include <QCoreApplication>
#include <QFile>
#include <QDebug>
const QString processName = "kflash.exe";
DKflash::DKflash(QObject *parent) : QObject(parent)
{
    m_process = nullptr;
}

DKflash::~DKflash()
{
    stopProcess();
}

void DKflash::_processInit()
{
    QString appPath = QCoreApplication::applicationDirPath().remove("_d");
    m_workSpaceDir.setPath(appPath);
#ifdef Q_OS_MAC
    m_workSpaceDir.cdUp();
    m_workSpaceDir.cd("Resources");
#endif
    if (m_workSpaceDir.exists("tool")) {
        m_workSpaceDir.cd("tool");
        m_workSpaceDir.cd("kflash");
    } else {
        qDebug() << "work space error:" << m_workSpaceDir;
    }

    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->setWorkingDirectory(m_workSpaceDir.absolutePath());

    connect(m_process, &QProcess::readyRead, this, &DKflash::onProcessReadyRead_slot);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DKflash::onProcessFinished_slot);
    connect(m_process, &QProcess::errorOccurred, this, &DKflash::errorHandle_slot);

    QDir dir(m_workSpaceDir);

#ifdef Q_OS_WIN
    QString program = "kflash.exe";
#else
    QString program = "kflash";
#endif

    if (dir.exists(program)) {
        m_process->setProgram(dir.absoluteFilePath(program));
    } else {
        qDebug() << "Kflash:" << program << "path is not correct.";
    }
}

void DKflash::setFileName(QString fileName, QString device)
{
    QString appPath = QCoreApplication::applicationDirPath().remove("_d");
    QDir dir(appPath);

#ifdef Q_OS_MAC
    dir.cdUp();
    dir.cd("Resources");
#endif

    if (!dir.exists("firmware")) {
        qDebug() << "k210 fileName error.";
        return;
    }
    dir.cd("firmware");

    // 如果fileName是空的，那么使用firmware下的固件
    if (fileName.isEmpty()) {

        QStringList nameFilters;
        nameFilters << "*.kfpkg";
        QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);

        QString typeName;
        if (device == "CameraArm") {
            typeName = "AicamEndstop";
        } else {
            typeName = "AicamCar";
        }

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

/* Command -p COM " ***.bin" */
void DKflash::startDownload(QString COM, quint64 start_id)
{
    this->start_id = start_id;
    _processInit();
    QFileInfo fileinfo(m_fileName);
    if (!fileinfo.exists()) {
        qDebug() << "k210 file not exists";
        return;
    }

    QStringList arguments;
#ifdef Q_OS_WIN
    arguments << "-p" << COM << m_fileName;
#elif defined (Q_OS_MAC)
    arguments << "-p" << QString("/dev/%1").arg(COM) << m_fileName;
#endif

    m_process->setArguments(arguments);

    qDebug() << "program:" << m_process->program();
    qDebug() << "arguments:" << m_process->arguments();
    m_process->start();
}

void DKflash::stopDownload(quint64 id)
{
    stopProcess();
    emit onStop_signal(id);
}

void DKflash::stopProcess()
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
void DKflash::onProcessReadyRead_slot()
{
    QString readStr = QString::fromLatin1(m_process->readAll());
    if (readStr.contains("error")) {
        emit onProcessError_signal(start_id, ERROR_DOWNLOAD_FIRMWARE, readStr);
    } else if (readStr.contains("fail")) {
        emit onProcessError_signal(start_id, ERROR_DOWNLOAD_FIRMWARE, readStr);
    } else {
        emit onProcessReadyRead_signal(readStr);
    }

}

void DKflash::onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus)
{
//    auto process = qobject_cast<QProcess *>(sender());

    if (exitStatus == QProcess::NormalExit) {
        emit onFinish_signal(true, start_id);
    } else {
        emit onFinish_signal(false, start_id);
        qDebug() << "error, exitCode:" << exitCode << "exitStatus:" << exitStatus;
    }
}

void DKflash::errorHandle_slot(QProcess::ProcessError error)
{
    qDebug() << "DKflash: QProcess ERROR:" << error;
    emit onProcessError_signal(start_id, error, "ProcessError");
}

