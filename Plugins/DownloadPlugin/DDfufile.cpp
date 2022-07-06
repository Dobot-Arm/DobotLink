#include "DDfufile.h"

#include <QCoreApplication>
#include <QFile>
#include <QDebug>
const QString processName = "dfucmd.exe";
DDfufile::DDfufile(QObject *parent) : QObject(parent)
{
//    _processInit();
}

DDfufile::~DDfufile()
{
    stopProcess();
}

void DDfufile::_processInit()
{
    QString appPath = QCoreApplication::applicationDirPath().remove("_d");
    m_workSpaceDir.setPath(appPath);
#ifdef Q_OS_MAC
    m_workSpaceDir.cdUp();
    m_workSpaceDir.cd("Resources");
#endif
    if (m_workSpaceDir.exists("tool")) {
        m_workSpaceDir.cd("tool");
        m_workSpaceDir.cd("dfu");
    } else {
        qDebug() << "work space error:" << m_workSpaceDir;
    }

    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->setWorkingDirectory(m_workSpaceDir.absolutePath());

    connect(m_process, &QProcess::readyRead, this, &DDfufile::onProcessReadyRead_slot);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DDfufile::onProcessFinished_slot);
    connect(m_process, &QProcess::errorOccurred, this, &DDfufile::errorHandle_slot);

    QDir dir(m_workSpaceDir);

#ifdef Q_OS_WIN
    QString program = "dfucmd.exe";
#else
    QString program = "dfucmd";
#endif

    if (dir.exists(program)) {
        m_process->setProgram(dir.absoluteFilePath(program));
    } else {
        qDebug() << "DDfufile:" << program << "path is not correct.";
    }
}

void DDfufile::setFileName(QString fileName)
{
    QString appPath = QCoreApplication::applicationDirPath().remove("_d");
    QDir dir(appPath);

#ifdef Q_OS_MAC
    dir.cdUp();
    dir.cd("Resources");
#endif

    if (!dir.exists("firmware")) {
        qDebug() << "dfu fileName error.";
        return;
    }
    dir.cd("firmware");

    // 如果fileName是空的，那么使用firmware下的固件
    if (fileName.isEmpty()) {
        QStringList nameFilters;
        nameFilters << "*.dfu";
        QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);

        QString typeName;
        if (m_type == DFU_MAGICBOX) {
            typeName = "MagicBox";
        } else if (m_type == DFU_MAGICIANLITE) {
            typeName = "MagicianLite";
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

/* DfuSeCommand -c -d --fn ***.dfu */
void DDfufile::startDownload(quint64 start_id)
{
    this->start_id = start_id;
    _processInit();
    QFileInfo fileinfo(m_fileName);
    if (!fileinfo.exists()) {
        qDebug() << "dfu file not exists";
        return;
    }

    QStringList arguments;
#ifdef Q_OS_WIN
    arguments << "-c" << "-d" << "--fn" << m_fileName;
#elif defined (Q_OS_MAC)
    arguments << "-a" << "0" << "-D" << m_fileName;
#endif

    m_process->setArguments(arguments);

    qDebug() << "program:" << m_process->program();
    qDebug() << "arguments:" << m_process->arguments();
    m_process->start();
}

void DDfufile::stopDownload(quint64 id)
{
    stopProcess();
    emit onStop_signal(id);
}

void DDfufile::stopProcess()
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
void DDfufile::onProcessReadyRead_slot()
{
    QString readStr = QString::fromLatin1(m_process->readAll());
    if (readStr.contains("0 Device(s) found")){
        qDebug() << "device is not in firmware mode";
        emit onProcessError_signal(start_id, ERROR_DOWNLOAD_FIRMWARE, readStr);
    } else if (readStr.contains("error")) {
        emit onProcessError_signal(start_id, ERROR_DOWNLOAD_FIRMWARE, readStr);
    } else if (readStr.contains("fail")) {
        emit onProcessError_signal(start_id, ERROR_DOWNLOAD_FIRMWARE, readStr);
    } else {
        emit onProcessReadyRead_signal(readStr);
    }
}

void DDfufile::onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus)
{
//    auto process = qobject_cast<QProcess *>(sender());

    if (exitStatus == QProcess::NormalExit) {
        emit onFinish_signal(true, start_id);
    } else {
        emit onFinish_signal(false, start_id);
        qDebug() << "error, exitCode:" << exitCode << "exitStatus:" << exitStatus;
    }
}

void DDfufile::errorHandle_slot(QProcess::ProcessError error)
{
    qDebug() << "DDfufile: QProcess ERROR:" << error << m_process->errorString();
    emit onProcessError_signal(start_id, error, "ProcessError");
}

