#include "CameraCalibration.h"
#include <QDebug>
#include <QJsonArray>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QStorageInfo>

const QString CameraCalibration::processName = "fit_homography.exe";

CameraCalibration *CameraCalibration::m_staticCameraCalibration = new CameraCalibration;
CameraCalibration *CameraCalibration::instance(){
    return m_staticCameraCalibration;
}

CameraCalibration::CameraCalibration()
{

}

void CameraCalibration::GOManualCalibration(uint32_t seqNum, QString april_list, QString device_list)
{
    if(april_list.isEmpty() or device_list.isEmpty()){
        qDebug() << __FUNCTION__ << "april_list or device_list is empty";
        return;
    } else {
        m_seqNum = seqNum;
        _processInit();
        _processSetArgument(april_list, device_list);
#if (defined (Q_OS_MAC) || defined (Q_OS_LINUX))
        QStringList envs = {
            "LC_ALL=en_US.utf-8",
            "LANG=en_US.utf-8"
        };
        m_process->setEnvironment(envs);
#endif
        m_process->start();
        qDebug().noquote() << "program:" << m_process->program();
        qDebug().noquote() << "arguments:" << m_process->arguments();
        qDebug().noquote() << "working directory:" << m_process->workingDirectory();
        qDebug().noquote() << "environment:" << m_process->environment();
    }
}

CameraCalibration::~CameraCalibration()
{
    _stopProcess();
}

void CameraCalibration::_processInit()
{
    QString appPath = QCoreApplication::applicationDirPath().remove("_d");
    m_workSpaceDir.setPath(appPath);
#ifdef Q_OS_MAC
    m_workSpaceDir.cdUp();
    m_workSpaceDir.cd("Resources");
#endif
    if (m_workSpaceDir.exists("tool")) {
        m_workSpaceDir.cd("tool");
        m_workSpaceDir.cd("fit_homography");
    } else {
        qDebug() << "work space error:" << m_workSpaceDir;
    }

    m_process = new QProcess();
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->setWorkingDirectory(m_workSpaceDir.absolutePath());
    connect(m_process, &QProcess::errorOccurred, this, [&](QProcess::ProcessError err){
        qWarning() << "error:" << err << m_process->errorString();
    });
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, [&](int exitCode, QProcess::ExitStatus exitStatus){
        qWarning() << "finish:" << exitCode << exitStatus;
    });
    QDir dir(m_workSpaceDir);
    connect(m_process, &QProcess::readyRead, this, &CameraCalibration::onProcessReadyRead_slot);
#ifdef Q_OS_WIN
    QString program = "fit_homography.exe";
#elif defined (Q_OS_MAC)
    QString program = "fit_homography";
#elif defined (Q_OS_LINUX)
    QString program = "fit_homography";
#elif defined (__wasm__)
    QString program = "fit_homography";
#endif
    if (dir.exists(program)) {
        m_processPath = dir.absoluteFilePath(program);
        m_process->setProgram(dir.absoluteFilePath(program));
        qDebug() << dir.absoluteFilePath(program);
    } else {
        qDebug() << "fit_homography:" << program << "path is not correct.";
    }
}

void CameraCalibration::_processSetArgument(QString april_list, QString device_list)
{
    QStringList param;
    param << "--april_list" << april_list << "--device_list" << device_list;
    m_process->setArguments(param);
}

void CameraCalibration::onProcessReadyRead_slot()
{
    QByteArray data;
    QString result;
    bool isTimeout = false;
    data = m_process->readAll();
    result = QString::fromLocal8Bit(data);
    QStringList list = result.split("aa");
    QString H = list[0];
    QString err = list[1];
    qDebug() << H <<  "----" <<err;
    QString BoxPath = "";

    foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady()) {
              if (storage.name().contains("PYBFLASH")) {
                  qDebug() << "contains PYBFLASH" << BoxPath;
                  BoxPath = storage.rootPath();
              }
        }
    }
    QDir dir(BoxPath);
    if (!dir.exists("K210")){
        dir.mkdir("K210");
        dir.cd("K210");
    }

    dir.cd("K210");
    if (!dir.exists("__init__.py")){
        QFile init(QString("%1/K210/__init__.py").arg(BoxPath));
        if (init.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            init.write(" ");
            init.close();
        }
    }
    QFile file(QString("%1/K210/data.py").arg(BoxPath));
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(H.toUtf8());
        file.close();
        qDebug("success data");
    }
    QJsonObject resObj;
    resObj.insert("data", err);
    emit m_staticCameraCalibration->receiveData_signal(isTimeout, resObj, m_seqNum);

}

void CameraCalibration::_stopProcess()
{
    if(m_process){
#ifdef Q_OS_WIN
        QProcess p;
        QString c = QString("taskkill /im %1 /f").arg(processName);
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
        m_process->kill();
        m_process->terminate();
        m_process->deleteLater();
        m_process = nullptr;
    }
}
