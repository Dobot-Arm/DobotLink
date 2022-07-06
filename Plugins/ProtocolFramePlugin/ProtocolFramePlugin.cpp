#include "ProtocolFramePlugin.h"
#include <QDebug>
extern "C" {
#include "DobotV3Func.h"
}

const QString ProtocolFramePlugin::PluginName = "Mooz";
const QString ProtocolFramePlugin::Version = "1.0.0";

ProtocolFramePlugin::ProtocolFramePlugin(QObject *parent) : DPluginInterface(parent) {
    m_controller = new Controller;
    m_seqNum = 0;

    m_initflag = true;
    m_mythread = new MyThread;
    m_moozApi = new MoozApi;
    connect(m_moozApi, SIGNAL(signals_receiveData(bool, QJsonObject, quint32)), this, SLOT(slots_receiveData(bool, QJsonObject, quint32)));
}

ProtocolFramePlugin::~ProtocolFramePlugin() {
    m_mythread->quit();
    m_mythread->wait();
    m_mythread->deleteLater();
    m_moozApi->deleteLater();
    m_controller->deleteLater();
}

QString ProtocolFramePlugin::getVersion() {
    return Version;
}

/* 收到命令 */
void ProtocolFramePlugin::pReceiveMassage_slot(QString id, QJsonObject obj) {
//    qDebug() << Q_FUNC_INFO << obj;
    if (id == "ALL") {
        qDebug() << "[ALL] {ProtocolFrame} get obj !!!";
        if (obj.contains("METHOD")) {
            QString method = obj.value("METHOD").toString();
            if (method == "EXIT") {
                qDebug() << "demo will EXIT !!!";
            }
        }
    } else if (id == PluginName) {
        handleCommand(obj);
    }
}

/* [!!!消息分发!!!] */
void ProtocolFramePlugin::handleCommand(const QJsonObject &obj) {
    ProtocolFramePacket packet;
    packet.setPacketFromObj(obj);

    //search & dis/connect
    if (packet.api.endsWith("Port")) {
        handlePortCommand(packet);
    } else {
        handleProtocolFrameCommand(packet);
    }
}

void ProtocolFramePlugin::handlePortCommand(const ProtocolFramePacket &packet) {
    QJsonObject resObj;
    ProtocolFrameResPacket resPacket(packet.id, packet.wsPort);

    if (packet.api == "SearchPort") {
        resObj = m_controller->SearchPort(packet);
    } else if (packet.api == "ConnectPort") {
        resObj = m_controller->ConnectPort(packet);
        if (m_controller->getPortName() != "" && m_initflag) {
            m_moozApi->slots_registerLink(m_controller->writeData, m_controller->readData);
            m_moozApi->initProtocol();
            m_mythread->start();
            m_mythread->setThreadStart();
            m_initflag = false;
        }
    } else if (packet.api == "DisconnectPort") {
        resObj = m_controller->DisconnectPort(packet);
        if (m_controller->getPortName() == "" && !m_initflag) {
            m_mythread->setThreadStop();
            if (!m_mythread->isFinished()) {
                m_mythread->exit();
            }
            m_initflag = true;
        }
    } else {
        qDebug() << "no this api with : Device !!!";

        resPacket.setErrorObj(1, "no this api with : Device !!!");
        resObj = resPacket.getResultObj();
    }

    if (!resObj.isEmpty()) {
        emit pSendMessage_signal(PluginName, resObj);
    }
}

void ProtocolFramePlugin::handleProtocolFrameCommand(const ProtocolFramePacket &packet) {
    QJsonObject resObj;
    ProtocolFrameResPacket resPacket(packet.id, packet.wsPort);

    qDebug() << "send:" << packet.id << packet.wsPort;
    if (packet.api == "SendGcode") {
        handleSendGcode(packet);
    } else if (packet.api == "GetLevellingInfo") {
        handleGetLevellingInfo(packet);
    } else if (packet.api == "GetCurrentPosition") {
        handleGetCurrentPosition(packet);
    } else if (packet.api == "GetCurrentTemperature") {
        handleGetCurrentTemperature(packet);
    } else if (packet.api == "GetCurrentPower") {
        handleGetCurrentPower(packet);
    } else if (packet.api == "GetCurrentTerminalType") {
        handleGetCurrentTerminalType(packet);
    } else if (packet.api == "GetCurrentRunningStatus") {
        handleGetCurrentRunningStatus(packet);
    } else if (packet.api == "SetAutoLevellingParams") {
        handleSetAutoLevellingParams(packet);
    } else if (packet.api == "GetCurrentZeroDeviationValue") {
        handleGetCurrentZeroDeviationValue(packet);
    } else if (packet.api == "GetCurrentMemoryStatus") {
        handleGetCurrentMemoryStatus(packet);
    } else if (packet.api == "GetCurrentPrintPageData") {
        handleGetCurrentPrintPageData(packet);
    } else if (packet.api == "GetCurrentInfoPageData") {
        handleGetCurrentInfoPageData(packet);
    } else if (packet.api == "GetFractureStatus") {
        handleGetFractureStatus(packet);
    } else if (packet.api == "GetCurrentLanguage") {
        handleGetCurrentLanguage(packet);
    } else if (packet.api == "SetCurrentLanguage") {
        handleSetCurrentLanguage(packet);
    } else if (packet.api == "SetAutoShutdown") {
        handleSetAutoShutdown(packet);
    } else if (packet.api == "GetCurrentPageID") {
        handleGetCurrentPageID(packet);
    } else if (packet.api == "GetCurrentFileNum") {
        handleGetCurrentFileNum(packet);
    } else if (packet.api == "GetCurrentFileName") {
        handleGetCurrentFileName(packet);
    } else if (packet.api == "SetFileTransferConfig") {
        handleSetFileTransferConfig(packet);
    } else if (packet.api == "SendFileTransferConfig") {
        handleSendFileTransferConfig(packet);
    } else if (packet.api == "CheckFile") {
        handleCheckFile(packet);
    } else if (packet.api == "CheckPackage") {
        handleCheckPackage(packet);
    } else if (packet.api == "SetTCPClient") {
        handleSetTCPClient(packet);
    } else if (packet.api == "DisconnectWifi") {
        handleDisconnectWifi(packet);
    } else {
        qDebug() << "no this protocol api !!!";

        resPacket.setErrorObj(2, "no this protocol api !!!");
        resObj = resPacket.getResultObj();
        emit pSendMessage_signal(PluginName, resObj);
    }

}

void ProtocolFramePlugin::handleSendGcode(const ProtocolFramePacket &packet) {
    QJsonArray cmdArrayJson = packet.paramsObj.value("cmdArray").toArray();
    uint16_t arrayLen = static_cast<uint16_t>(cmdArrayJson.size());
    uint8_t cmdArray[arrayLen];
    memset(&cmdArray, 0, sizeof(uint8_t) * arrayLen);
    for (int i = 0; i < arrayLen; i++) {
        cmdArray[i] = static_cast<uint8_t>(cmdArrayJson.at(i).toInt());
    }
    m_moozApi->SendGcode(m_seqNum, cmdArray, arrayLen);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetLevellingInfo(const ProtocolFramePacket &packet) {
    m_moozApi->GetLevellingInfo(m_seqNum);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetCurrentPosition(const ProtocolFramePacket &packet) {
    m_moozApi->GetCurrentPosition(m_seqNum);
    qDebug() << "now:" << packet.id << packet.wsPort << "seqNum" << m_seqNum;
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetCurrentTemperature(const ProtocolFramePacket &packet) {
    m_moozApi->GetCurrentTemperature(m_seqNum);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetCurrentPower(const ProtocolFramePacket &packet) {
    m_moozApi->GetCurrentPower(m_seqNum);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetCurrentTerminalType(const ProtocolFramePacket &packet) {
    m_moozApi->GetCurrentTerminalType(m_seqNum);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetCurrentRunningStatus(const ProtocolFramePacket &packet) {
    m_moozApi->GetCurrentRunningStatus(m_seqNum);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleSetAutoLevellingParams(const ProtocolFramePacket &packet) {
    m_moozApi->SetAutoLevellingParams(m_seqNum);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetCurrentZeroDeviationValue(const ProtocolFramePacket &packet) {
    m_moozApi->GetCurrentZeroDeviationValue(m_seqNum);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetCurrentMemoryStatus(const ProtocolFramePacket &packet) {
    m_moozApi->GetCurrentMemoryStatus(m_seqNum);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetCurrentPrintPageData(const ProtocolFramePacket &packet) {
    m_moozApi->GetCurrentPrintPageData(m_seqNum);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetCurrentInfoPageData(const ProtocolFramePacket &packet) {
    m_moozApi->GetCurrentInfoPageData(m_seqNum);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetFractureStatus(const ProtocolFramePacket &packet) {
    m_moozApi->GetFractureStatus(m_seqNum);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetCurrentLanguage(const ProtocolFramePacket &packet) {
    m_moozApi->GetCurrentLanguage(m_seqNum);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleSetCurrentLanguage(const ProtocolFramePacket &packet) {
    uint8_t languageState = static_cast<uint8_t>(packet.paramsObj.value("languageState").toInt());
    m_moozApi->SetCurrentLanguage(m_seqNum, languageState);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleSetAutoShutdown(const ProtocolFramePacket &packet) {
    uint8_t isAuto = static_cast<uint8_t>(packet.paramsObj.value("isAuto").toInt());
    m_moozApi->SetAutoShutdown(m_seqNum, isAuto);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetCurrentPageID(const ProtocolFramePacket &packet) {
    uint8_t pageId = static_cast<uint8_t>(packet.paramsObj.value("pageId").toInt());
    m_moozApi->GetCurrentPageID(m_seqNum, pageId);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetCurrentFileNum(const ProtocolFramePacket &packet) {
    m_moozApi->GetCurrentFileNum(m_seqNum);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleGetCurrentFileName(const ProtocolFramePacket &packet) {
    uint8_t begin = static_cast<uint8_t>(packet.paramsObj.value("begin").toInt());
    uint8_t end = static_cast<uint8_t>(packet.paramsObj.value("end").toInt());
    m_moozApi->GetCurrentFileName(m_seqNum, begin, end);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleSetFileTransferConfig(const ProtocolFramePacket &packet) {
    uint8_t mode = static_cast<uint8_t>(packet.paramsObj.value("mode").toInt());
    uint32_t fileSize = static_cast<uint32_t>(packet.paramsObj.value("fileSize").toInt());
    uint16_t packetSize = static_cast<uint16_t>(packet.paramsObj.value("packetSize").toInt());

    QString fileNameString = packet.paramsObj.value("fileName").toString();
    QString downloadUrlString = packet.paramsObj.value("downloadUrl").toString();
    const char *fileName;
    const char *downloadUrl;
    std::string  fileNamestr = fileNameString.toStdString();
    fileName = fileNamestr.c_str();
    std::string  downloadUrlstr = downloadUrlString.toStdString();
    downloadUrl = downloadUrlstr.c_str();

    m_moozApi->SetFileTransferConfig(m_seqNum, mode, fileName, fileSize, packetSize, downloadUrl);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleSendFileTransferConfig(const ProtocolFramePacket &packet) {
    uint16_t currentPacketIndex = static_cast<uint16_t>(packet.paramsObj.value("currentPacketIndex").toInt());
    uint16_t totalPacketNum = static_cast<uint16_t>(packet.paramsObj.value("totalPacketNum").toInt());
    uint8_t supplementFlag = static_cast<uint8_t>(packet.paramsObj.value("supplementFlag").toInt());

    QJsonArray dataArrayJson = packet.paramsObj.value("dataArray").toArray();
    uint8_t arrayLen = static_cast<uint8_t>(dataArrayJson.size());
    uint8_t dataArray[arrayLen];
    memset(&dataArray, 0, sizeof(uint8_t) * arrayLen);
    for (int i = 0; i <  arrayLen; i++) {
        dataArray[i] = static_cast<uint8_t>(dataArrayJson.at(i).toInt());
    }

    m_moozApi->SendFileTransferConfig(m_seqNum, currentPacketIndex, totalPacketNum, supplementFlag, dataArray, arrayLen);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleCheckFile(const ProtocolFramePacket &packet) {
    uint32_t fileSize = static_cast<uint32_t>(packet.paramsObj.value("fileSize").toInt());
    uint8_t verityType = static_cast<uint8_t>(packet.paramsObj.value("verityType").toInt());
    uint32_t verifyCode = static_cast<uint32_t>(packet.paramsObj.value("verifyCode").toInt());

    QString fileNameString = packet.paramsObj.value("fileName").toString();
    const char *fileName;
    std::string  fileNamestr = fileNameString.toStdString();
    fileName = fileNamestr.c_str();

    m_moozApi->CheckFile(m_seqNum, fileName, fileSize, verityType, verifyCode);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleCheckPackage(const ProtocolFramePacket &packet) {
    uint16_t beginPacketIndex = static_cast<uint16_t>(packet.paramsObj.value("beginPacketIndex").toInt());
    uint16_t endPacketIndex = static_cast<uint16_t>(packet.paramsObj.value("endPacketIndex").toInt());

    QString fileNameString = packet.paramsObj.value("fileName").toString();
    const char *fileName;
    std::string  fileNamestr = fileNameString.toStdString();
    fileName = fileNamestr.c_str();

    m_moozApi->CheckPackage(m_seqNum, fileName, beginPacketIndex, endPacketIndex);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleSetTCPClient(const ProtocolFramePacket &packet) {
    uint32_t port = static_cast<uint32_t>(packet.paramsObj.value("port").toInt());

    QString downloadUrlString = packet.paramsObj.value("downloadUrl").toString();
    const char *downloadUrl;
    std::string  downloadUrlstr = downloadUrlString.toStdString();
    downloadUrl = downloadUrlstr.c_str();

    m_moozApi->SetTCPClient(m_seqNum, downloadUrl, port);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::handleDisconnectWifi(const ProtocolFramePacket &packet) {
    uint8_t resver = static_cast<uint8_t>(packet.paramsObj.value("resver").toInt());
    m_moozApi->DisconnectWifi(m_seqNum, resver);
    m_idPortSeqMap.insert(m_seqNum, IdPortPacket(packet.id, packet.wsPort));
    m_seqNum++;
}

void ProtocolFramePlugin::slots_receiveData(bool isTimeOut, QJsonObject jsonObj, quint32 seqNum) {
    qDebug() << "receive seqNum:" << seqNum;
    //超时的处理
    if (isTimeOut) {
        ProtocolFrameResPacket resPacket(m_idPortSeqMap.find(seqNum).value().id, m_idPortSeqMap.find(seqNum).value().wsPort);
        resPacket.setErrorObj(1, QString("the cmd receive timeout !   "));
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
        return;
    }

    QMap<uint32_t, IdPortPacket>::const_iterator i;
    for (i = m_idPortSeqMap.constBegin(); i != m_idPortSeqMap.constEnd(); ++i) {
        qDebug() << "first" << "key" << i.key() << "value" << i.value().id << i.value().wsPort;
    }

    qDebug() << "receive:" << m_idPortSeqMap.find(seqNum).value().id << m_idPortSeqMap.find(seqNum).value().wsPort;
    if (!jsonObj.isEmpty()) {
        ProtocolFrameResPacket resPacket(m_idPortSeqMap.find(seqNum).value().id, m_idPortSeqMap.find(seqNum).value().wsPort);
        resPacket.setResultObj(jsonObj);
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    }
    m_idPortSeqMap.remove(seqNum);

    QMap<uint32_t, IdPortPacket>::const_iterator it;
    for (it = m_idPortSeqMap.constBegin(); it != m_idPortSeqMap.constEnd(); ++it) {
        qDebug() << "second:" << "key" << it.key() << "value" << it.value().id << it.value().wsPort;
    }
}
