#include "DAvrCompiler.h"

#include <QCoreApplication>
#include <QStandardPaths>
#include <QDebug>

DAvrCompiler::DAvrCompiler(QObject *parent) : QObject(parent)
{
    _processInit();
    _codefileInit();
    mDeviceType = ARDUINO_CTYPE_UNO;
}

DAvrCompiler::~DAvrCompiler()
{
    gppProcess->close();
    gccProcess->close();
    objcopyProcess->close();
    qDebug() << "DAvrCompiler destroyed.";
}

void DAvrCompiler::_processInit()
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
    workSpaceDir.cd("Resources");
    workSpaceDir.cd("arduino");
#endif
//    qDebug() << "work space:" << workSpaceDir;

    gppProcess = new QProcess(this);
    gppProcess->setProcessChannelMode(QProcess::MergedChannels);
    gppProcess->setWorkingDirectory(workSpaceDir.absolutePath());

    connect(gppProcess, &QProcess::readyRead, this, &DAvrCompiler::onProcessReadyRead_slot);
    connect(gppProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DAvrCompiler::onProcessFinished_slot);
    connect(gppProcess, &QProcess::errorOccurred, this, &DAvrCompiler::errorHandle_slot);

    gccProcess = new QProcess(this);
    gccProcess->setProcessChannelMode(QProcess::MergedChannels);
    gccProcess->setWorkingDirectory(workSpaceDir.absolutePath());

    connect(gccProcess, &QProcess::readyRead, this, &DAvrCompiler::onProcessReadyRead_slot);
    connect(gccProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DAvrCompiler::onProcessFinished_slot);
    connect(gccProcess, &QProcess::errorOccurred, this, &DAvrCompiler::errorHandle_slot);

    objcopyProcess = new QProcess(this);
    objcopyProcess->setProcessChannelMode(QProcess::MergedChannels);
    objcopyProcess->setWorkingDirectory(workSpaceDir.absolutePath());

    connect(objcopyProcess, &QProcess::readyRead, this, &DAvrCompiler::onProcessReadyRead_slot);
    connect(objcopyProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &DAvrCompiler::onProcessFinished_slot);
    connect(objcopyProcess, &QProcess::errorOccurred, this, &DAvrCompiler::errorHandle_slot);

    QDir dir(workSpaceDir);
    dir.cd("avr-toolchain");
    dir.cd("bin");

    if (dir.exists("avr-g++.exe")) {
        gppProcess->setProgram(dir.absoluteFilePath("avr-g++.exe"));
    } else if (dir.exists("avr-g++")) {
        gppProcess->setProgram(dir.absoluteFilePath("avr-g++"));
    } else {
        qDebug() << "DAvrCompiler:avr-g++.exe path is not correct.";
    }

    if (dir.exists("avr-gcc.exe")) {
        gccProcess->setProgram(dir.absoluteFilePath("avr-gcc.exe"));
    } else if (dir.exists("avr-gcc")) {
        gccProcess->setProgram(dir.absoluteFilePath("avr-gcc"));
    } else {
        qDebug() << "DAvrCompiler:avr-gcc.exe path is not correct.";
    }

    if (dir.exists("avr-objcopy.exe")) {
        objcopyProcess->setProgram(dir.absoluteFilePath("avr-objcopy.exe"));
    } else if (dir.exists("avr-objcopy")) {
        objcopyProcess->setProgram(dir.absoluteFilePath("avr-objcopy"));
    } else {
        qDebug() << "DAvrCompiler:avr-objcopy.exe path is not correct.";
    }
}

void DAvrCompiler::_codefileInit()
{
    codeDir.setPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    codeDir.cd("dobotlink-code");
    codeDir.cd("avr-build");
    codeFile.setFileName(codeDir.absoluteFilePath("code.cpp"));
}

QString DAvrCompiler::getDeviceName(DeviceType type)
{
    QString name;
    switch (type) {
    case ARDUINO_CTYPE_UNO:
        name = "Arduino Uno";
        break;
    case ARDUINO_CTYPE_MEAG2560:
        name = "Arduino Mega2560";
        break;
    case ARDUINO_CTYPE_AISTARTER:
        name = "Arduino AIStarter";
        break;
    case ARDUINO_CTYPE_MOBILEPLATFORM:
        name = "Arduino MobilePlatform";
        break;
    case ARDUINO_CTYPE_SMARTKIT:
        name = "Arduino SmartKit";
        break;
    }
    return name;
}

void DAvrCompiler::startCompile()
{
    QStringList tempFiles = codeDir.entryList();
    foreach (const QString fileName, tempFiles) {
        if (fileName.endsWith(".o")
                || fileName.endsWith(".d")
                || fileName.endsWith(".elf")
                || fileName.endsWith(".hex")) {
            QFile file(codeDir.absoluteFilePath(fileName));
            file.remove();
        }
    }

    QDir dir(workSpaceDir);
    dir.cd("avr-toolchain");
    if (!dir.exists("bin")) {
        qDebug() << "DAvrCompiler: Arduino-tools not found.";
        emit compileFinished_signal(ARDUINO_ERROR_TOOL_NOTFOUND, "Arduino-tools not found.");
        return;
    }

    if (!codeFile.exists()) {
        qDebug() << "DAvrCompiler: code.cpp file is not exist.";
        emit compileFinished_signal(ARDUINO_ERROR_CODEFILE_NOTFOUND, "code.cpp file is not exist.");
        return;
    }

    qDebug() << "Start compile:" << getDeviceName(mDeviceType)
             << "Arduino work directory:" << workSpaceDir.absolutePath();

    QStringList arguments;

//![1] G++
    QString mcuStr;
    QString avrTypeStr;
    if (mDeviceType == ARDUINO_CTYPE_UNO) {
        mcuStr = "-mmcu=atmega328p";
        avrTypeStr = "-DARDUINO_AVR_UNO";
    } else {
        mcuStr = "-mmcu=atmega2560";
        avrTypeStr = "-DARDUINO_AVR_MEGA2560";
    }

    arguments << "-c" << "-g" << "-Os" << "-w" << "-fno-exceptions" << "-ffunction-sections" << "-fdata-sections";
    arguments << "-fno-threadsafe-statics" << "-MMD" << mcuStr << "-DF_CPU=16000000L" << "-DARDUINO=10605";
    arguments << avrTypeStr << "-DARDUINO_ARCH_AVR";
    arguments << "-Iavr-library/variants/standard";
    arguments << "-Iavr-library/variants/mega";
    arguments << "-Iavr-library/cores/arduino";

    switch (mDeviceType) {
    case ARDUINO_CTYPE_UNO:
        break;
    case ARDUINO_CTYPE_MEAG2560:
    case ARDUINO_CTYPE_AISTARTER:
        arguments << "-Iarduino-libraries/AIStarter/src";
        arguments << "-Iarduino-libraries/AIStarter/src/utility";
        arguments << "-Iarduino-libraries/AIStarter/src/utility/avr";
        break;
    case ARDUINO_CTYPE_MOBILEPLATFORM:
        arguments << "-Iarduino-libraries/MobilePlatform/src";
        arguments << "-Iarduino-libraries/MobilePlatform/src/CH375";
        arguments << "-Iarduino-libraries/MobilePlatform/src/LSM9DS1";
        arguments << "-Iarduino-libraries/MobilePlatform/src/MPU9250";
        arguments << "-Iarduino-libraries/MobilePlatform/src/utility";
        arguments << "-Iarduino-libraries/MobilePlatform/src/utility/avr";
        arguments << "-Iarduino-libraries/MobilePlatform/src/magician";
        arguments << "-Iarduino-libraries/MobilePlatform/src/pixy2";
        break;
    case ARDUINO_CTYPE_SMARTKIT:
        arguments << "-Iarduino-libraries/SmartKit/src";
        arguments << "-Iarduino-libraries/SmartKit/src/pixy2";
        arguments << "-Iarduino-libraries/SmartKit/src/magician";
        break;
    }

    arguments << "-Iavr-library/libraries/Wire/utility";
    arguments << "-Iavr-library/libraries/Wire";
    arguments << "-Iavr-library/libraries/SoftwareSerial";

    arguments << "-Iarduino-libraries/arduino/WiFi/src/";
    arguments << "-Iarduino-libraries/arduino/SD/src/";
    arguments << "-Iarduino-libraries/arduino/Bridge/src/";
    arguments << "-Iarduino-libraries/arduino/Temboo/src/";
    arguments << "-Iarduino-libraries/arduino/Servo/src/";
    arguments << "-Iarduino-libraries/arduino/Ethernet/src/";
    arguments << "-Iarduino-libraries/arduino/TFT/src/";
    arguments << "-Iarduino-libraries/arduino/SpacebrewYun/src/";
    arguments << "-Iarduino-libraries/arduino/LiquidCrystal/src/";
    arguments << "-Iarduino-libraries/arduino/GSM/src/";
    arguments << codeDir.absoluteFilePath("code.cpp");
    arguments << "-o";
    arguments << codeDir.absoluteFilePath("code.o");

    gppProcess->setArguments(arguments);
    qDebug()<<"start g++ compiler******************************";
    gppProcess->start();
    qDebug() << gppProcess->program();
    qDebug() << gppProcess->arguments();

    bool ok = gppProcess->waitForFinished(10000);
    if (ok == false) {
        qDebug() << "DAvrCompiler:gppProcess failed.";
        return;
    }

//![2] GCC
    arguments.clear();
    arguments << "-w" << "-Os" << mcuStr;
#ifdef Q_OS_WIN
    arguments << "-Wl,--gc-sections";
#elif defined (Q_OS_MAC)
    arguments << "-fdata-sections";
#endif
    arguments << "-o";
    arguments << codeDir.absoluteFilePath("out.elf");
    arguments << codeDir.absoluteFilePath("code.o");

    QDir dobotADir(workSpaceDir);
    QString libFileName;

    switch (mDeviceType) {
    case ARDUINO_CTYPE_UNO:
        dobotADir.cd("dobot-Uno");
        libFileName = QString("libUno.a");
        break;
    case ARDUINO_CTYPE_MEAG2560:
    case ARDUINO_CTYPE_AISTARTER:
        dobotADir.cd("dobot-aistarter");
        libFileName = QString("AIStarter.a");
        break;
    case ARDUINO_CTYPE_MOBILEPLATFORM:
        dobotADir.cd("dobot-mobileplatform");
        libFileName = QString("MobilePlatform.a");
        break;
    case ARDUINO_CTYPE_SMARTKIT:
        dobotADir.cd("dobot-smartkit");
        libFileName = QString("SmartKit.a");
        break;
    }

    if (!libFileName.isEmpty()) {
        arguments << dobotADir.absoluteFilePath(libFileName);
        arguments << "-lm";
    }

    gccProcess->setArguments(arguments);
    gccProcess->start();
    qDebug()<<"start gcc compiler******************************";
    qDebug() << gccProcess->program();
    qDebug() << gccProcess->arguments();

    bool ok1 = gccProcess->waitForFinished(10000);
    if (ok1 == false) {
        qDebug() << "DAvrCompiler:gccProcess failed.";
        return;
    }

//![3] OBJCOPY
    arguments.clear();
    arguments << "-O" << "ihex" << "-R" << ".eeprom";
    arguments << codeDir.absoluteFilePath("out.elf");
    arguments << codeDir.absoluteFilePath("out.hex");

    objcopyProcess->setArguments(arguments);
    objcopyProcess->start();
    qDebug()<<"start objcopy compiler******************************";
    qDebug() << objcopyProcess->program();
    qDebug() << objcopyProcess->arguments();

    bool ok2 = objcopyProcess->waitForFinished(10000);
    if (ok2 == false) {
        qDebug() << "DAvrCompiler:objcopyProcess failed.";
        return;
    }
}

/* SLOT */
void DAvrCompiler::onProcessReadyRead_slot()
{
    auto process = qobject_cast<QProcess *>(sender());
    QString str = process->readAllStandardOutput();
    QString strErr = process->readAllStandardOutput();
    qDebug()<<process<<",===============read:"<<str;
    qDebug()<<process<<",===============read:"<<strErr;
}

void DAvrCompiler::onProcessFinished_slot(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)

    auto process = qobject_cast<QProcess *>(sender());

    if (process == gppProcess) {
        if (exitStatus == QProcess::NormalExit) {
            emit onProcessPercent_signal(30, "avr-g++ finieshed OK");
//            qDebug() << gppProcess->program() << ":finished OK";
        } else {
            QString errorStr = "avr-g++:";
            errorStr.append(gppProcess->errorString());
            emit compileFinished_signal(ARDUINO_ERROR_GPP_FAIL, "avr-g++ compile Error:" + errorStr);
            return;
        }
    } else if (process == gccProcess) {
        if (exitStatus == QProcess::NormalExit) {
            emit onProcessPercent_signal(60, "avr-gcc finieshed OK");
//            qDebug() << gccProcess->program() << ":finished OK";
        } else {
            QString errorStr = "avr-gcc:";
            errorStr.append(gppProcess->errorString());
            emit compileFinished_signal(ARDUINO_ERROR_GCC_FAIL, "avr-gcc compile Error:" + errorStr);
            return;
        }
    } else if (process == objcopyProcess) {
        if (exitStatus == QProcess::NormalExit) {
            emit onProcessPercent_signal(100, "avr-objcopy finieshed OK");
//            qDebug() << gccProcess->program() << ":finished OK";
        } else {
            QString errorStr = "avr-objcopy:";
            errorStr.append(objcopyProcess->errorString());
            emit compileFinished_signal(ARDUINO_ERROR_OBJCOPY_FAIL, "avr-objcopy compile Error:" + errorStr);
            return;
        }

        emit compileFinished_signal(ARDUINO_NOERROR);
    }
}

void DAvrCompiler::errorHandle_slot(QProcess::ProcessError error)
{
    qDebug() << "DAvrCompiler: compile ERROR:" << error;

    QString message = QString("compile:Error:%1").arg(error);
    emit compileFinished_signal(ARDUINO_ERROR_PROCESS_CRASH, message);
}
