#include "DAvrUploader.h"

#include <QSerialPortInfo>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>

#define EXPORT_UPLOAD_PERCENT

DAvrUploader::DAvrUploader(QObject *parent) : QObject(parent)
{
    _processInit();
    _codeFileInit();
}

DAvrUploader::~DAvrUploader()
{  
    m_process->close();
    qDebug() << "DAvrUploader destroyed.";
}

void DAvrUploader::_processInit()
{
    workSpaceDir.setPath(QCoreApplication::applicationDirPath());
#ifdef Q_OS_WIN
    if (!workSpaceDir.exists("arduino")) {
        workSpaceDir.cdUp();
        workSpaceDir.cd("Plugins");
        workSpaceDir.cd("ArduinoPlugin");
    }
    workSpaceDir.cd("arduino");
#elif defined (Q_OS_MAC)
    workSpaceDir.cdUp();
    workSpaceDir.cd("Resources/arduino");
#endif
//    qDebug() << "work space:" << workSpaceDir;

    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_process->setWorkingDirectory(workSpaceDir.absolutePath());

    connect(m_process, &QProcess::started, this, &DAvrUploader::onStartedHandle_slot);
    connect(m_process, &QProcess::readyRead, this, &DAvrUploader::onProcessReadyRead_slot);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DAvrUploader::onProcessFinished_slot);
    connect(m_process, &QProcess::errorOccurred, this, &DAvrUploader::errorHandle_slot);

    QDir dir(workSpaceDir);
    dir.cd("avr-toolchain");
    dir.cd("bin");

    if (dir.exists("avrdude.exe")) {
        m_process->setProgram(dir.absoluteFilePath("avrdude.exe"));
    } else if (dir.exists("avrdude")) {
        m_process->setProgram(dir.absoluteFilePath("avrdude"));
    } else {
        qDebug() << "DAvrUploader:avrdude path is not correct.";
    }
}

void DAvrUploader::_codeFileInit()
{
    codeDir.setPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    codeDir.cd("dobotlink-code");
    codeDir.cd("avr-build");
    codeFile.setFileName(codeDir.absoluteFilePath("out.hex"));
}

/* Arduino Uno / Arduino Mega 2560 */
void DAvrUploader::startUpload()
{
    if (!codeFile.exists()) {
        qDebug() << "DAvrUploader: out.hex file is not exist.";
        emit uploadFinished_signal(ARDUINO_ERROR_HEX_FILE_NOTFOUND, "out.hex file is not exist.");
        return;
    }

    uploadLog.clear();

    QSerialPortInfo deviceInfo;

    if (!portName.isEmpty()) {
        foreach (const QSerialPortInfo &portInfo, QSerialPortInfo::availablePorts()) {
            if (portInfo.portName() == portName) {
                deviceInfo = portInfo;
                break;
            }
        }
    } else {
        foreach (const QSerialPortInfo &portInfo, QSerialPortInfo::availablePorts()) {
            deviceInfo = portInfo;
            break;
        }
    }

    if (deviceInfo.isNull()) {
        qDebug() << "DAvrUploader: no available device." << " port:" << portName;
        emit uploadFinished_signal(ARDUINO_ERROR_NO_AVAILABLE_DEVICE, "no available device.");
        return;
    }

    qDebug() << "DAvrUploader: ready upload. name:" << deviceInfo.description()
             << "portName:" << deviceInfo.portName();

    /* avrdude上传 */
    QDir confDir(workSpaceDir);
    confDir.cd("avr-toolchain");
    confDir.cd("etc");

    /* >avrdude.exe -Cavrdude.conf -u -carduino -pm328p -PCOM3 -b115200 -Uflash:w:out.hex:a */
    /* >avrdude.exe -Cavrdude.conf -u -cwiring -pm2560 -D -PCOM3 -b115200 -Uflash:w:out.hex:a */

    QStringList arguments;
    arguments << QString("-C%1").arg(confDir.absoluteFilePath("avrdude.conf"));
    arguments << "-u";
    arguments << "-v";      /* 详细 */

    if (mDeviceType == ARDUINO_UTYPE_UNO) {
        arguments << "-carduino" << "-pm328p";
    } else if (mDeviceType == ARDUINO_UTYPE_MEAG2560) {
        arguments << "-cwiring" <<"-pm2560" << "-D";
    } else {
        qDebug() << "DAvrUploader: Do not specify Arduino Type." << mDeviceType;
        emit uploadFinished_signal(ARDUINO_ERROR_NO_TYPE, "Do not specify Arduino Type.");
        return;
    }

    /* serial port */
#ifdef Q_OS_WIN
    arguments << QString("-P%1").arg(deviceInfo.portName());
#elif defined (Q_OS_MAC)
    arguments << QString("-P/dev/%1").arg(deviceInfo.portName());
#endif

    arguments << "-b115200";
    arguments << QString("-Uflash:w:%1:a").arg(codeFile.fileName());

    qDebug() << m_process->program();
    m_process->setArguments(arguments);
    m_process->start();
}

/* SLOT */
void DAvrUploader::onStartedHandle_slot()
{
    qDebug() << "Start upload";
    m_percent = -1;      //有标识"-v"会多打一个#
}

void DAvrUploader::onProcessReadyRead_slot()
{
    QString readStr = QString::fromLatin1(m_process->readAll());

    uploadLog.append(readStr);

    if (readStr.contains("#")) {
        m_percent += readStr.count("#");

#ifdef EXPORT_UPLOAD_PERCENT
        handleProcessPercentValue(m_percent);
#endif
    }
}

/* 发送信号，显示上传进度 */
void DAvrUploader::handleProcessPercentValue(int value)
{
    static int compareValue = 0;

    if ((compareValue != 50) && (value <= 52)) {
        compareValue = 50;
    } else if (value >= compareValue) {
        emit onProcessPercent_signal(value - 50, "uploading..");
        compareValue += 10;
    }
}

void DAvrUploader::onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)

    if (exitStatus == QProcess::NormalExit) {
        qDebug() << "DAvrUploader: Upload Finished OK.";
        qDebug() << "NormalExit LOG:" << uploadLog;

        if (uploadLog.contains("error")) {
            emit uploadFinished_signal(ARDUINO_ERROR_UNKNOWN, "error, please read log.");
        } else if (uploadLog.contains("timeout")) {
            emit uploadFinished_signal(ARDUINO_ERROR_TIMEOUT, "upload process timeout.");
        } else {
            emit uploadFinished_signal(ARDUINO_NOERROR);
        }
    } else {
        /* Process CrashExit */
        qDebug() << "DAvrUploader: Process Finished Error.";
        emit uploadFinished_signal(ARDUINO_ERROR_PROCESS_CRASH, "upload:" + m_process->errorString());
    }
}

void DAvrUploader::errorHandle_slot(QProcess::ProcessError error)
{
    qDebug() << "DAvrUploader: upload ERROR:" << error;

    QString message = QString("DAvrUploader: upload ERROR:%1").arg(error);
    emit uploadFinished_signal(ARDUINO_ERROR_PROCESS_CRASH, message);
}
