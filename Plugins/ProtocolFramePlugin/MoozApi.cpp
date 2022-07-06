#include "MoozApi.h"
#include <QDebug>
#include <QJsonArray>

MoozApi *MoozApi::m_staticMoozApi = nullptr;

MoozApi::MoozApi() {
    m_staticMoozApi = this;
    m_isStop = false;

    m_timer = new QTimer;
    m_timer->start(10);
    connect(m_timer, &QTimer::timeout, [ = ]() {
        DobotV3_TimeCNT(10);
    });
}

void MoozApi::SendGcode(uint32_t seqNum, uint8_t *cmdArray, uint16_t arrayLength) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = SendGcodeId;
    memcpy(&packetProtocol.payloadAndCRC[0], &cmdArray, sizeof(uint8_t)*arrayLength);

    bool res = DobotV3_SendCmd(&packetProtocol, arrayLength);
    qDebug() << "res" << res;
}

void MoozApi::GetLevellingInfo(uint32_t seqNum) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetLevellingInfoId;
    uint16_t payloadLen = 0;

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::GetCurrentPosition(uint32_t seqNum) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetCurrentPositionId;
    uint16_t payloadLen = 0;

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
    qDebug() << "seqNum:" << seqNum;
}

void MoozApi::GetCurrentTemperature(uint32_t seqNum) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetCurrentTemperatureId;
    uint16_t payloadLen = 0;

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::GetCurrentPower(uint32_t seqNum) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetCurrentPowerId;
    uint16_t payloadLen = 0;

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::GetCurrentTerminalType(uint32_t seqNum) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetCurrentTerminalTypeId;
    uint16_t payloadLen = 0;

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::GetCurrentRunningStatus(uint32_t seqNum) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetCurrentRunningStatusId;
    uint16_t payloadLen = 0;

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::SetAutoLevellingParams(uint32_t seqNum) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = SetAutoLevellingParamsId;
    uint16_t payloadLen = 0;

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::GetCurrentZeroDeviationValue(uint32_t seqNum) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetCurrentZeroDeviationValueId;
    uint16_t payloadLen = 0;

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::GetCurrentMemoryStatus(uint32_t seqNum) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetCurrentMemoryStatusId;
    uint16_t payloadLen = 0;

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::GetCurrentPrintPageData(uint32_t seqNum) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetCurrentPrintPageDataId;
    uint16_t payloadLen = 0;

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::GetCurrentInfoPageData(uint32_t seqNum) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetCurrentInfoPageDataId;
    uint16_t payloadLen = 0;

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::GetFractureStatus(uint32_t seqNum) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetFractureStatusId;
    uint16_t payloadLen = 0;

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::GetCurrentLanguage(uint32_t seqNum) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetCurrentLanguageId;
    uint16_t payloadLen = 0;

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::SetCurrentLanguage(uint32_t seqNum, uint8_t languageState) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = SetCurrentLanguageId;
    uint16_t payloadLen = 1;
    memcpy(&packetProtocol.payloadAndCRC[0], &languageState, sizeof(uint8_t));

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::SetAutoShutdown(uint32_t seqNum, uint8_t isAuto) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = SetAutoShutdownId;
    uint16_t payloadLen = 1;
    memcpy(&packetProtocol.payloadAndCRC[0], &isAuto, sizeof(uint8_t));

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::GetCurrentPageID(uint32_t seqNum, uint8_t pageId) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetCurrentPageIDId;
    uint16_t payloadLen = 1;
    memcpy(&packetProtocol.payloadAndCRC[0], &pageId, sizeof(uint8_t));

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::GetCurrentFileNum(uint32_t seqNum) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetCurrentFileNumId;
    uint16_t payloadLen = 0;

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::GetCurrentFileName(uint32_t seqNum, uint8_t begin, uint8_t end) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = GetCurrentFileNameId;
    uint16_t payloadLen = 2;
    memcpy(&packetProtocol.payloadAndCRC[0], &begin, sizeof(uint8_t));
    memcpy(&packetProtocol.payloadAndCRC[1], &end, sizeof(uint8_t));

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::SetFileTransferConfig(uint32_t seqNum, uint8_t mode, const char *fileName, uint32_t fileSize, uint16_t packetSize, const char *downloadUrl) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = SetFileTransferConfigId;
    uint16_t payloadLen = 199;
    memcpy(&packetProtocol.payloadAndCRC[0], &mode, sizeof(uint8_t));
    memcpy(&packetProtocol.payloadAndCRC[1], &fileName, sizeof(char) * 64);
    memcpy(&packetProtocol.payloadAndCRC[65], &fileSize, sizeof(uint32_t));
    memcpy(&packetProtocol.payloadAndCRC[69], &packetSize, sizeof(uint16_t));
    memcpy(&packetProtocol.payloadAndCRC[71], &downloadUrl, sizeof(char) * 128);

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::SendFileTransferConfig(uint32_t seqNum, uint16_t currentPacketIndex, uint16_t totalPacketNum, uint8_t supplementFlag, uint8_t *data, uint8_t dataLength) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = SendFileTransferConfigId;
    uint16_t payloadLen = 5 + dataLength;
    memcpy(&packetProtocol.payloadAndCRC[0], &currentPacketIndex, sizeof(uint16_t));
    memcpy(&packetProtocol.payloadAndCRC[2], &totalPacketNum, sizeof(uint16_t));
    memcpy(&packetProtocol.payloadAndCRC[4], &supplementFlag, sizeof(uint8_t));
    memcpy(&packetProtocol.payloadAndCRC[5], &data, sizeof(uint8_t) * dataLength);

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::CheckFile(uint32_t seqNum, const char *fileName, uint32_t fileSize, uint8_t verityType, uint32_t verifyCode) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = CheckFileId;
    uint16_t payloadLen = 74;
    memcpy(&packetProtocol.payloadAndCRC[0], &fileName, sizeof(char) * 64);
    memcpy(&packetProtocol.payloadAndCRC[64], &fileSize, sizeof(uint32_t));
    memcpy(&packetProtocol.payloadAndCRC[68], &verityType, sizeof(uint8_t));
    memcpy(&packetProtocol.payloadAndCRC[69], &verifyCode, sizeof(uint32_t));

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::CheckPackage(uint32_t seqNum, const char *fileName, uint16_t beginPacketIndex, uint16_t endPacketIndex) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = CheckPackageId;
    uint16_t payloadLen = 68;
    memcpy(&packetProtocol.payloadAndCRC[0], &fileName, sizeof(char) * 64);
    memcpy(&packetProtocol.payloadAndCRC[64], &beginPacketIndex, sizeof(uint16_t));
    memcpy(&packetProtocol.payloadAndCRC[66], &endPacketIndex, sizeof(uint16_t));

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::SetTCPClient(uint32_t seqNum, const char *severUrl, uint32_t port) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = SetTCPClientId;
    uint16_t payloadLen = 132;
    memcpy(&packetProtocol.payloadAndCRC[0], &severUrl, sizeof(char) * 128);
    memcpy(&packetProtocol.payloadAndCRC[128], &port, sizeof(uint32_t));

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::DisconnectWifi(uint32_t seqNum, uint8_t resver) {
    DobotV3Format_Packet packetProtocol;
    memset(&packetProtocol, 0, sizeof(DobotV3Format_Packet));
    packetProtocol.version = 0x10;
    packetProtocol.needAck = 1;
    packetProtocol.isAck = 0;
    packetProtocol.rw = 1;
    packetProtocol.cmdType = 0;
    packetProtocol.encType = 0;
    packetProtocol.seqType = 1;
    packetProtocol.seqNum = seqNum;
    packetProtocol.src = 0x00;
    packetProtocol.des = 0x10;
    packetProtocol.cmdSet = 0x04;
    packetProtocol.cmdId = DisconnectWifiId;
    uint16_t payloadLen = 1;
    memcpy(&packetProtocol.payloadAndCRC[0], &resver, sizeof(uint8_t));

    bool res = DobotV3_SendCmd(&packetProtocol, payloadLen);
    qDebug() << "res" << res;
}

void MoozApi::SendGcode_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t state;
        uint8_t excuteRes[sizeof(packet.packet.payloadAndCRC) / 8 - 3];

        memcpy(&state, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        memcpy(&excuteRes, &packet.packet.payloadAndCRC[1], sizeof(packet.packet.payloadAndCRC - 3 * sizeof(uint8_t)));
        resObj.insert("state", state);
        resObj.insert("excuteRes", reinterpret_cast<char *>(excuteRes));
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetLevellingInfo_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        int pos[3];
        memcpy(&pos[0], &packet.packet.payloadAndCRC[0], sizeof(int));
        memcpy(&pos[1], &packet.packet.payloadAndCRC[1], sizeof(int));
        memcpy(&pos[2], &packet.packet.payloadAndCRC[2], sizeof(int));
        resObj.insert("pos1", pos[0]);
        resObj.insert("pos2", pos[1]);
        resObj.insert("pos3", pos[2]);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetCurrentPosition_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        float result[4];
        memcpy(&result[0], &packet.packet.payloadAndCRC[0], sizeof(float));
        memcpy(&result[1], &packet.packet.payloadAndCRC[4], sizeof(float));
        memcpy(&result[2], &packet.packet.payloadAndCRC[8], sizeof(float));
        memcpy(&result[3], &packet.packet.payloadAndCRC[12], sizeof(float));
        resObj.insert("x", static_cast<double>(result[0]));
        resObj.insert("y", static_cast<double>(result[1]));
        resObj.insert("z", static_cast<double>(result[2]));
        resObj.insert("e", static_cast<double>(result[3]));
    }

    qDebug() << "CB seqNum:" << packet.packet.seqNum;
    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetCurrentTemperature_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        float tempBedTarget;
        float tempBedNow;
        float tempHotendTarget;
        float tempHotendNow;
        memcpy(&tempBedTarget, &packet.packet.payloadAndCRC[0], sizeof(float));
        memcpy(&tempBedNow, &packet.packet.payloadAndCRC[4], sizeof(float));
        memcpy(&tempHotendTarget, &packet.packet.payloadAndCRC[8], sizeof(float));
        memcpy(&tempHotendNow, &packet.packet.payloadAndCRC[12], sizeof(float));
        resObj.insert("x", static_cast<double>(tempBedTarget));
        resObj.insert("y", static_cast<double>(tempBedNow));
        resObj.insert("z", static_cast<double>(tempHotendTarget));
        resObj.insert("e", static_cast<double>(tempHotendNow));
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetCurrentPower_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint32_t fan_value;
        memcpy(&fan_value, &packet.packet.payloadAndCRC[0], sizeof(uint32_t));
        resObj.insert("fan_value", static_cast<int>(fan_value));
    }
    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetCurrentTerminalType_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t endstopType;
        memcpy(&endstopType, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        resObj.insert("endstopType", endstopType);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetCurrentRunningStatus_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t isRunning;
        memcpy(&isRunning, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        resObj.insert("isRunning", isRunning);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::SetAutoLevellingParams_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        QJsonObject resObj;
        uint8_t state;
        uint8_t pointNum;
        float point[3][3];
        memcpy(&state, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        memcpy(&pointNum, &packet.packet.payloadAndCRC[1], sizeof(uint8_t));
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                memcpy(&point[i][j], &packet.packet.payloadAndCRC[2 + i * 3 + j], sizeof(float));
            }
        }
        resObj.insert("state", state);
        resObj.insert("pointNum", pointNum);
        QJsonArray pointArrayJson;
        for (int i = 0; i < 3; i++) {
            QJsonArray tempArrayJson;
            tempArrayJson.insert(0, static_cast<double>(point[i][0]));
            tempArrayJson.insert(1, static_cast<double>(point[i][1]));
            tempArrayJson.insert(2, static_cast<double>(point[i][2]));
            pointArrayJson.insert(i, tempArrayJson);
        }
        resObj.insert("pointArray", pointArrayJson);
    }
    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetCurrentZeroDeviationValue_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        float x_offset;
        float y_offset;
        float z_offset;
        float dx;
        float dy;
        memcpy(&x_offset, &packet.packet.payloadAndCRC[0], sizeof(float));
        memcpy(&y_offset, &packet.packet.payloadAndCRC[4], sizeof(float));
        memcpy(&z_offset, &packet.packet.payloadAndCRC[8], sizeof(float));
        memcpy(&dx, &packet.packet.payloadAndCRC[12], sizeof(float));
        memcpy(&dy, &packet.packet.payloadAndCRC[16], sizeof(float));
        resObj.insert("x_offset", static_cast<double>(x_offset));
        resObj.insert("y_offset", static_cast<double>(y_offset));
        resObj.insert("z_offset", static_cast<double>(z_offset));
        resObj.insert("z_offset", static_cast<double>(dx));
        resObj.insert("z_offset", static_cast<double>(dy));
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetCurrentMemoryStatus_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t memoryState;
        memcpy(&memoryState, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        resObj.insert("memoryState", memoryState);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetCurrentPrintPageData_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        float       pose[4];                      //当前运动坐标
        float       tempBedTarget;         //热床目标温度
        float       tempBedNow;           //热床当前温度
        float       tempHotendTarget;   // 加热头目标温度
        float       tempHotendNow;      // 加热头当前温度
        uint64_t printTime;                 //当前打印时间
        float       printProcess;            //当前打印进度
        uint8_t       printFilename[64];    //当前打印文件名称
        uint8_t   filaState;                   //耗材状态
        uint8_t   isAutoPoweroff;        //自动关机状态
        uint8_t   printingState;            //打印状态
        int          feedMultiply;             //打印速率

        memcpy(&pose[0], &packet.packet.payloadAndCRC[0], sizeof(float));
        memcpy(&pose[1], &packet.packet.payloadAndCRC[4], sizeof(float));
        memcpy(&pose[2], &packet.packet.payloadAndCRC[8], sizeof(float));
        memcpy(&pose[3], &packet.packet.payloadAndCRC[12], sizeof(float));
        memcpy(&tempBedTarget, &packet.packet.payloadAndCRC[16], sizeof(float));
        memcpy(&tempBedNow, &packet.packet.payloadAndCRC[20], sizeof(float));
        memcpy(&tempHotendTarget, &packet.packet.payloadAndCRC[24], sizeof(float));
        memcpy(&tempHotendNow, &packet.packet.payloadAndCRC[28], sizeof(float));
        memcpy(&printTime, &packet.packet.payloadAndCRC[32], sizeof(uint64_t));
        memcpy(&printProcess, &packet.packet.payloadAndCRC[40], sizeof(float));
        memcpy(&printFilename, &packet.packet.payloadAndCRC[44], sizeof(char) * 64);
        memcpy(&filaState, &packet.packet.payloadAndCRC[108], sizeof(uint8_t));
        memcpy(&isAutoPoweroff, &packet.packet.payloadAndCRC[109], sizeof(uint8_t));
        memcpy(&printingState, &packet.packet.payloadAndCRC[110], sizeof(uint8_t));
        memcpy(&feedMultiply, &packet.packet.payloadAndCRC[111], sizeof(int));

        QJsonArray poseArrayJson;
        poseArrayJson.insert(0, static_cast<double>(pose[0]));
        poseArrayJson.insert(1, static_cast<double>(pose[1]));
        poseArrayJson.insert(2, static_cast<double>(pose[2]));
        poseArrayJson.insert(3, static_cast<double>(pose[3]));
        resObj.insert("poseArray", poseArrayJson);
        resObj.insert("tempBedTarget", static_cast<double>(tempBedTarget));
        resObj.insert("tempBedNow", static_cast<double>(tempBedNow));
        resObj.insert("tempHotendTarget", static_cast<double>(tempHotendTarget));
        resObj.insert("tempHotendNow", static_cast<double>(tempHotendNow));
        resObj.insert("printTime", static_cast<double>(printTime));
        resObj.insert("printProcess", static_cast<double>(printProcess));
        resObj.insert("printFilename", reinterpret_cast<char *>(printFilename));
        resObj.insert("filaState", filaState);
        resObj.insert("isAutoPoweroff", isAutoPoweroff);
        resObj.insert("printingState", printingState);
        resObj.insert("feedMultiply", feedMultiply);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetCurrentInfoPageData_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t   moduleType;    //机型
        uint8_t  mac[6];              //机器MAC地址
        uint8_t  ip[4];                 //机器IP地址
        uint8_t  version[4];        //最后一位测试版本号为0时，不显示最后一位，否则显示最后一位以便于区分版本；
        uint16_t  size[3];             //机器尺寸
        uint64_t  runTime;          //运行时间

        memcpy(&moduleType, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        QJsonArray macArrayJson;
        for (int i = 0; i < 6; i++) {
            memcpy(&mac[i], &packet.packet.payloadAndCRC[1 + i], sizeof(uint8_t));
            macArrayJson.insert(i, mac[i]);
        }
        QJsonArray ipArrayJson;
        for (int i = 0; i < 4; i++) {
            memcpy(&ip[i], &packet.packet.payloadAndCRC[7 + i], sizeof(uint8_t));
            ipArrayJson.insert(i, ip[i]);
        }
        QJsonArray versionArrayJson;
        for (int i = 0; i < 4; i++) {
            memcpy(&version[i], &packet.packet.payloadAndCRC[11 + i], sizeof(uint8_t));
            versionArrayJson.insert(i, version[i]);
        }
        QJsonArray sizeArrayJson;
        for (int i = 0; i < 3; i++) {
            memcpy(&size[i], &packet.packet.payloadAndCRC[15 + 2 * i], sizeof(uint16_t));
            sizeArrayJson.insert(i, size[i]);
        }
        memcpy(&runTime, &packet.packet.payloadAndCRC[21], sizeof(uint64_t));
        resObj.insert("moduleType", moduleType);
        resObj.insert("macArray", macArrayJson);
        resObj.insert("ipArray", ipArrayJson);
        resObj.insert("versionArray", versionArrayJson);
        resObj.insert("sizeArray", sizeArrayJson);
        resObj.insert("runTime", static_cast<double>(runTime));
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetFractureStatus_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t filamentState;
        memcpy(&filamentState, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        resObj.insert("filamentState", filamentState);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetCurrentLanguage_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t languageState;

        memcpy(&languageState, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        resObj.insert("languageState", languageState);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::SetCurrentLanguage_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;
        resObj.insert("result", "cmd set success!");
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::SetAutoShutdown_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;
        resObj.insert("result", "cmd set success!");
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetCurrentPageID_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t state;

        memcpy(&state, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        resObj.insert("state", state);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetCurrentFileNum_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t count;

        memcpy(&count, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        resObj.insert("count", count);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::GetCurrentFileName_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t begin;
        uint8_t end;
        uint8_t filename[3][64];

        memcpy(&begin, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        memcpy(&end, &packet.packet.payloadAndCRC[1], sizeof(uint8_t));
        int result = end - begin + 1;
        switch (result) {
            case 1:
                memcpy(&filename[0], &packet.packet.payloadAndCRC[2], sizeof(uint8_t) * 64);
                resObj.insert("filename1", reinterpret_cast<char *>(filename[0]));
                break;
            case 2:
                memcpy(&filename[0], &packet.packet.payloadAndCRC[2], sizeof(uint8_t) * 64);
                memcpy(&filename[1], &packet.packet.payloadAndCRC[66], sizeof(uint8_t) * 64);
                resObj.insert("filename1", reinterpret_cast<char *>(filename[0]));
                resObj.insert("filename2", reinterpret_cast<char *>(filename[1]));
                break;
            case 3:
                memcpy(&filename[0], &packet.packet.payloadAndCRC[2], sizeof(uint8_t) * 64);
                memcpy(&filename[1], &packet.packet.payloadAndCRC[66], sizeof(uint8_t) * 64);
                memcpy(&filename[2], &packet.packet.payloadAndCRC[130], sizeof(uint8_t) * 64);
                resObj.insert("filename1", reinterpret_cast<char *>(filename[0]));
                resObj.insert("filename2", reinterpret_cast<char *>(filename[1]));
                resObj.insert("filename3", reinterpret_cast<char *>(filename[2]));
                break;
        }
        resObj.insert("begin", begin);
        resObj.insert("end", end);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::SetFileTransferConfig_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t state;

        memcpy(&state, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        resObj.insert("state", state);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::SendFileTransferConfig_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t state;
        uint16_t writeTime;
        uint16_t currentWriteIndex;

        memcpy(&state, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        memcpy(&writeTime, &packet.packet.payloadAndCRC[1], sizeof(uint16_t));
        memcpy(&currentWriteIndex, &packet.packet.payloadAndCRC[3], sizeof(uint16_t));
        resObj.insert("state", state);
        resObj.insert("writeTime", writeTime);
        resObj.insert("currentWriteIndex", currentWriteIndex);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::CheckFile_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t state;

        memcpy(&state, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        resObj.insert("state", state);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::CheckPackage_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint16_t      endPacketIndex;
        uint16_t      errorPacketNum = 0;
        uint16_t      errorPacketIndex[errorPacketNum];

        memcpy(&endPacketIndex, &packet.packet.payloadAndCRC[0], sizeof(uint16_t));
        memcpy(&errorPacketNum, &packet.packet.payloadAndCRC[2], sizeof(uint16_t));

        resObj.insert("endPacketIndex", endPacketIndex);
        resObj.insert("errorPacketNum", errorPacketNum);
        QJsonArray errorArrayJson;
        for (int i = 0; i < errorPacketNum; i++) {
            memcpy(&errorPacketIndex[i], &packet.packet.payloadAndCRC[4 + 2 * errorPacketNum], sizeof(uint16_t));
            errorArrayJson.insert(i, errorPacketIndex[i]);
        }
        resObj.insert("endPacketIndexArray", errorArrayJson);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::SetTCPClient_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t state;

        memcpy(&state, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        resObj.insert("state", state);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}

void MoozApi::DisconnectWifi_CB(DobotV3CmdInterCBParm packet) {
    bool isTimeOut = false;
    QJsonObject resObj;
    if (packet.error == DobotV3CmdInter_TIMEOUT) {
        qDebug() << "TimeOut    ";
        isTimeOut = true;
    } else {
        qDebug() << "Success    ";
        isTimeOut = false;

        uint8_t state;

        memcpy(&state, &packet.packet.payloadAndCRC[0], sizeof(uint8_t));
        resObj.insert("state", state);
    }

    emit m_staticMoozApi->signals_receiveData(isTimeOut, resObj, packet.packet.seqNum);
}


void MoozApi::initProtocol() {
    DobotV3_SetSendParm(3, 100);  //设置用户默认重发次数和超时时间

    DobotV3_RegisterGRCB(CBSendGcodeId, SendGcode_CB);
    DobotV3_RegisterGRCB(CBGetLevellingInfoId, GetLevellingInfo_CB);
    DobotV3_RegisterGRCB(CBGetCurrentPositionId, GetCurrentPosition_CB);
    DobotV3_RegisterGRCB(CBGetCurrentTemperatureId, GetCurrentTemperature_CB);
    DobotV3_RegisterGRCB(CBGetCurrentPowerId, GetCurrentPower_CB);
    DobotV3_RegisterGRCB(CBGetCurrentTerminalTypeId, GetCurrentTerminalType_CB);
    DobotV3_RegisterGRCB(CBGetCurrentRunningStatusId, GetCurrentRunningStatus_CB);
    DobotV3_RegisterGRCB(CBSetAutoLevellingParamsId, SetAutoLevellingParams_CB);
    DobotV3_RegisterGRCB(CBGetCurrentZeroDeviationValueId, GetCurrentZeroDeviationValue_CB);
    DobotV3_RegisterGRCB(CBGetCurrentMemoryStatusId, GetCurrentMemoryStatus_CB);
    DobotV3_RegisterGRCB(CBGetCurrentPrintPageDataId, GetCurrentPrintPageData_CB);
    DobotV3_RegisterGRCB(CBGetCurrentInfoPageDataId, GetCurrentInfoPageData_CB);
    DobotV3_RegisterGRCB(CBGetFractureStatusId, GetFractureStatus_CB);
    DobotV3_RegisterGRCB(CBGetCurrentLanguageId, GetCurrentLanguage_CB);
    DobotV3_RegisterGRCB(CBSetCurrentLanguageId, SetCurrentLanguage_CB);
    DobotV3_RegisterGRCB(CBSetAutoShutdownId, SetAutoShutdown_CB);
    DobotV3_RegisterGRCB(CBGetCurrentPageIDId, GetCurrentPageID_CB);
    DobotV3_RegisterGRCB(CBGetCurrentFileNumId, GetCurrentFileNum_CB);
    DobotV3_RegisterGRCB(CBGetCurrentFileNameId, GetCurrentFileName_CB);
    DobotV3_RegisterGRCB(CBSetFileTransferConfigId, SetFileTransferConfig_CB);
    DobotV3_RegisterGRCB(CBSendFileTransferConfigId, SendFileTransferConfig_CB);
    DobotV3_RegisterGRCB(CBCheckFileId, CheckFile_CB);
    DobotV3_RegisterGRCB(CBCheckPackageId, CheckPackage_CB);
    DobotV3_RegisterGRCB(CBSetTCPClientId, SetTCPClient_CB);
    DobotV3_RegisterGRCB(CBDisconnectWifiId, DisconnectWifi_CB);

    DobotV3_Init();
}


void MoozApi::slots_registerLink(DobotV3_SendData SendData, DobotV3_ReadData ReadData) {
    DobotV3_RegisterLink(SendData, ReadData);
}

