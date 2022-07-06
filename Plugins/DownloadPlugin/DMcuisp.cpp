#include "DMcuisp.h"

#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QSerialPortInfo>
#include <QTimer>

const QString processName = "Mcuisp.exe";
DMcuisp::DMcuisp(QObject *parent) : QObject(parent)
{

}

DMcuisp::~DMcuisp()
{
    stopProcess();
}

void DMcuisp::_processInit()
{
    QString appPath = QCoreApplication::applicationDirPath().remove("_d");
    m_workSpaceDir.setPath(appPath);
#ifdef Q_OS_MAC
    m_workSpaceDir.cdUp();
    m_workSpaceDir.cd("Resources");
#endif
    if (m_workSpaceDir.exists("tool")) {
        m_workSpaceDir.cd("tool");
        m_workSpaceDir.cd("mcuisp");
#ifdef Q_OS_MAC
        m_workSpaceDir.cd("Mcuisp.app");
        m_workSpaceDir.cd("Contents");
        m_workSpaceDir.cd("MacOS");
#endif
    } else {
        qDebug() << "work space error:" << m_workSpaceDir;
    }

    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->setWorkingDirectory(m_workSpaceDir.absolutePath());

    connect(m_process, &QProcess::readyRead, this, &DMcuisp::onProcessReadyRead_slot);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DMcuisp::onProcessFinished_slot);
    connect(m_process, &QProcess::errorOccurred, this, &DMcuisp::errorHandle_slot);

    QDir dir(m_workSpaceDir);

#ifdef Q_OS_WIN
    QString program = "Mcuisp.exe";
#elif defined (Q_OS_MAC)
    QString program = "Mcuisp";
#elif defined (Q_OS_LINUX)
    QString program = "stm32flash";
#endif

    if (dir.exists(program)) {
        m_process->setProgram(dir.absoluteFilePath(program));
    } else {
        qDebug() << "Mcuispfile:" << program << "path is not correct.";
    }
}


void DMcuisp::setFileName(QString fileName)
{
    QString appPath = QCoreApplication::applicationDirPath().remove("_d");
    QDir dir(appPath);

#ifdef Q_OS_MAC
    dir.cdUp();
    dir.cd("Resources");
#endif

    if (!dir.exists("firmware")) {
        qDebug() << "mcuisp fileName error.";
        return;
    }
    dir.cd("firmware");

    // 如果fileName是空的，那么使用firmware下的固件
    if (fileName.isEmpty()) {
        QStringList nameFilters;
        nameFilters << "*.hex";
        QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);

        QString typeName = "DobotMCU";

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

/* Mcuisp.exe "Magician" "D:\aDL5.8.0\DobotLink\firmware\DobotMCU_V3.7.0.1.hex" "COM3" "AutoStart"  */
/*
when work in linux:
sudo ./stm32flash -i rts,-dtr,dtr:-rts,-dtr,dtr -w Dobot.hex /dev/ttyACM0
sudo ./stm32flash -g 0x08000000 /dev/ttyACM0
*/
void DMcuisp::startDownload(QString COM, quint64 start_id, bool is3DPrinter)
{
    this->start_id = start_id;
    _processInit();
    QFileInfo fileinfo(m_fileName);
    if(!fileinfo.exists()) {
        qDebug() << "MCU file not exists";
        return;
    }

    QStringList arguments;

    QString fwType = is3DPrinter ? "Marlin" : "Magician";
#if defined(Q_OS_LINUX)
    downloadFirwareForLinux(COM);
#else
    arguments << fwType << m_fileName << COM << "AutoStart";
    m_process->setArguments(arguments);
    qDebug() << "program:" << m_process->program();
    qDebug() << "arguments:" << m_process->arguments();
    m_process->start();
#endif
}

void DMcuisp::stopDowenload(quint64 id)
{
    stopProcess();
    emit onStop_signal(id);
}

void DMcuisp::stopProcess()
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
#elif defined(Q_OS_LINUX)
        QProcess p;
        QFileInfo fileInfo(processName);
        QString c = QString("pkill -9 %1").arg(fileInfo.baseName());
        p.execute(c);
        p.waitForFinished();
        p.close();
#endif
}

/* SLOT */
void DMcuisp::onProcessReadyRead_slot()
{
    QString readStr = QString::fromLatin1(m_process->readAll());

    if (readStr.contains("error")) {
        emit onProcessError_signal(start_id, ERROR_DOWNLOAD_FIRMWARE, readStr);
    } else {
        emit onProcessReadyRead_signal(readStr);
    }
}

void DMcuisp::onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus)
{
//    auto process = qobject_cast<QProcess *>(sender());

    if (exitStatus == QProcess::NormalExit) {
#if defined(Q_OS_LINUX)
        //嵌入式那边说下载后最好延迟50ms后再启动
        QTimer::singleShot(100, [this]{
            restartSTM32FlashForLinux();//linux中是由2条命令完成固件更新下载的，所以第一次下载成功后再发送启动命令
        });
#else
        emit onFinish_signal(true, start_id);
#endif
    } else {
        emit onFinish_signal(false, start_id);
        qDebug() << "error, exitCode:" << exitCode << "exitStatus:" << exitStatus;
    }
}

void DMcuisp::errorHandle_slot(QProcess::ProcessError error)
{
    qDebug() << "DDfufile: QProcess ERROR:" << error;
    emit onProcessError_signal(start_id, error, "ProcessError");
}

void DMcuisp::downloadFirwareForLinux(const QString& strCOM)
{
    QDir dir(m_workSpaceDir);
    QString strExeFile = dir.absoluteFilePath("stm32flash");
    strExeFile.replace(' ',"\\ ");
    QString strHexFile = m_fileName;
    strHexFile.replace(' ',"\\ ");
    m_strSerialPort = strCOM;
    if (!m_strSerialPort.startsWith("/dev/"))
    {
        QSerialPortInfo info(m_strSerialPort);
        m_strSerialPort = info.systemLocation();
    }
    QString strCmd;
    strCmd.append("sudo ")
            .append(strExeFile)
            .append(" -i")
            .append(" rts,-dtr,dtr:-rts,-dtr,dtr")
            .append(" -w ")
            .append(strHexFile)
            .append(' ')
            .append(m_strSerialPort);
    qDebug()<<strCmd;
    m_process->start(strCmd);
}

void DMcuisp::restartSTM32FlashForLinux()
{
    auto pProc = new QProcess(this);
    pProc->setProcessChannelMode(QProcess::MergedChannels);
    connect(pProc, &QProcess::errorOccurred, [this, pProc](QProcess::ProcessError error){
        emit onProcessError_signal(start_id, error, "ProcessError");
        pProc->kill();
        pProc->deleteLater();
    });
    connect(pProc, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished), [this, pProc](int exitCode, QProcess::ExitStatus exitStatus){
        if (exitStatus == QProcess::NormalExit)
        {
            emit onFinish_signal(true, start_id);
        }
        else
        {
            emit onFinish_signal(false, start_id);
            qDebug() << "error, exitCode:" << exitCode << "exitStatus:" << exitStatus;
        }
        pProc->kill();
        pProc->deleteLater();
    });
    connect(pProc, &QProcess::readyRead, [this, pProc]{
        QString readStr = QString::fromLatin1(pProc->readAll());
        emit onProcessReadyRead_signal(readStr);
    });
    QDir dir(m_workSpaceDir);
    QString strExeFile = dir.absoluteFilePath("stm32flash");
    strExeFile.replace(' ',"\\ ");
    QString strCmd;
    strCmd.append("sudo ")
            .append(strExeFile)
            .append(" -g")
            .append(" 0x08000000 ")
            .append(m_strSerialPort);
    qDebug()<<strCmd;
    pProc->start(strCmd);
}


