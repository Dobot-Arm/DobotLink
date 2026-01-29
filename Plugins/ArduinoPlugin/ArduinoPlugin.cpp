#include "ArduinoPlugin.h"

#include <QStandardPaths>
#include <QTimer>
#include <QDir>
#include <QDebug>

#include "../version.h"

const QString ArduinoPlugin::PluginName = "Arduino";
const QString ArduinoPlugin::Version = VERSION_PLUGIN_ARDUINO;

ArduinoPlugin::ArduinoPlugin(QObject *parent) : DPluginInterface(parent)
{
    m_isBusy = false;

    _codefileInit();

    m_serialPort = new QSerialPort(this);
    m_serialPort->setBaudRate(QSerialPort::Baud115200);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    m_compiler = new DAvrCompiler(this);
    connect(m_compiler, &DAvrCompiler::compileFinished_signal, this, &ArduinoPlugin::handleCompileFinish_slot);
    connect(m_compiler, &DAvrCompiler::onProcessPercent_signal, this, &ArduinoPlugin::handleProcessPercent_slot);

    m_uploader = new DAvrUploader(this);
    connect(m_uploader, &DAvrUploader::uploadFinished_signal, this, &ArduinoPlugin::handleUploadFinish_slot);
    connect(m_uploader, &DAvrUploader::onProcessPercent_signal, this, &ArduinoPlugin::handleProcessPercent_slot);
}

ArduinoPlugin::~ArduinoPlugin()
{

}

QString ArduinoPlugin::getVersion()
{
    return Version;
}

void ArduinoPlugin::_codefileInit()
{
    QDir codeDir;
    codeDir.setPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

    if (!codeDir.exists("dobotlink-code")) {
        codeDir.mkdir("dobotlink-code");
    }
    codeDir.cd("dobotlink-code");
    if (!codeDir.exists("avr-build")) {
        codeDir.mkdir("avr-build");
    }
    codeDir.cd("avr-build");
    codeFile.setFileName(codeDir.absoluteFilePath("code.cpp"));
}

/* 收到命令 */
void ArduinoPlugin::pReceiveMassage_slot(QString id, QJsonObject obj)
{
    if (id == "ALL") {
        qDebug() << "[ALL] {Arduino} get obj" << obj;
    } else if (id == PluginName) {
        ArduinoPacket packet;
        packet.setPacket(obj);

        if (packet.api == "ArduinoProgram") {
            if (handleProgram(packet) == true) {
                requestId = packet.id;
                originRequestId = packet.originRequestId;
                m_port = packet.port;
                m_isBusy = true;
                m_compiler->startCompile();
            }
        }
    }  
}

bool ArduinoPlugin::handleProgram(const ArduinoPacket &packet)
{
    ArduinoResPacket resPacket(packet.id, packet.originRequestId, m_port);

    m_serialPort->setPortName(packet.portName);
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        m_serialPort->close();
    } else {
        resPacket.setErrorObj(PortError, "portName is Occupied.");
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
        return false;
    }

    if (m_isBusy == true) {
        resPacket.setErrorObj(IsBusy, "ArduinoProgram is buzy.");
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    }

    if (!packet.data.isEmpty()) {
        if (writeIntoCodeFile(packet.data) == false) {
            return false;
        }
    } else {
        resPacket.setErrorObj(DataError, "ArduinoProgram's data is empty.");
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
        return false;
    }

    if (packet.type == "AIStarter") {
        m_compiler->mDeviceType = DAvrCompiler::ARDUINO_CTYPE_AISTARTER;
        m_uploader->mDeviceType = DAvrUploader::ARDUINO_UTYPE_MEAG2560;
    } else if (packet.type == "MobilePlatform") {
        m_compiler->mDeviceType = DAvrCompiler::ARDUINO_CTYPE_MOBILEPLATFORM;
        m_uploader->mDeviceType = DAvrUploader::ARDUINO_UTYPE_MEAG2560;
    } else if (packet.type == "Arduino Uno") {
        m_compiler->mDeviceType = DAvrCompiler::ARDUINO_CTYPE_UNO;
        m_uploader->mDeviceType = DAvrUploader::ARDUINO_UTYPE_UNO;
    } else if (packet.type == "Arduino Mega2560") {
        m_compiler->mDeviceType = DAvrCompiler::ARDUINO_CTYPE_MEAG2560;
        m_uploader->mDeviceType = DAvrUploader::ARDUINO_UTYPE_MEAG2560;
    } else if (packet.type == "SmartKit") {
        m_compiler->mDeviceType = DAvrCompiler::ARDUINO_CTYPE_SMARTKIT;
        m_uploader->mDeviceType = DAvrUploader::ARDUINO_UTYPE_MEAG2560;
    } else {
        resPacket.setErrorObj(TypeError, "Arduino Type error. type:" + packet.type);
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
        return false;
    }

    if (!packet.portName.isEmpty()) {
        m_uploader->portName = packet.portName;
    } else {
        resPacket.setErrorObj(PortError, "serial port is empty.");
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
        return false;
    }

    return true;
}

/* 写入本地文件 */
bool ArduinoPlugin::writeIntoCodeFile(QString text)
{
    if (!codeFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) { 

        ArduinoResPacket resPacket(requestId, originRequestId, m_port);
        resPacket.setErrorObj(FileOpenFail, "Open code file Failed. filePath:" + codeFile.fileName());
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
        return false;
    }

    QTextStream out(&codeFile);
    out << text;

    codeFile.close();
    return true;
}

/* SLOT */
void ArduinoPlugin::handleCompileFinish_slot(int type, QString massage)
{
    if (type == 0) {
        m_uploader->startUpload();
    } else {
        ArduinoResPacket resPacket(requestId, originRequestId, m_port);
        resPacket.setErrorObj(type, massage);
        m_isBusy = false;
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    }
}

void ArduinoPlugin::handleUploadFinish_slot(int type, QString massage)
{
    ArduinoResPacket resPacket(requestId, originRequestId, m_port);

    if (type == 0) {
        resPacket.setResultObj();
    } else {
        resPacket.setErrorObj(type, massage);
    }

    emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    m_isBusy = false;
}

void ArduinoPlugin::handleProcessPercent_slot(int percent, QString message)
{
    QJsonObject processObj;

    if (sender() == m_compiler) {
        processObj.insert("type", "Arduino Compile");
        processObj.insert("totalProgress", percent * 0.4);
    } else if (sender() == m_uploader) {
        processObj.insert("type", "Arduino Upload");
        double totalPercent = percent * 0.6 + 40;
        if (totalPercent > 98) {
            totalPercent = 100;
        }
        processObj.insert("totalProgress", totalPercent);
    }
    processObj.insert("progress", percent);
    processObj.insert("message", message);

    ArduinoResPacket resPacket(requestId, originRequestId, m_port);
    resPacket.setResultObj(processObj);

    emit pSendMessage_signal(PluginName, resPacket.getResultObj());
}



