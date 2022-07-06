#ifndef MOOZAPI_H
#define MOOZAPI_H

#include <QObject>
#include <QJsonObject>
#include <QMap>
#include <QTimer>
#include "DobotType.h"
#include "ProtocolFramePacket.h"
extern "C" {
#include "DobotV3Func.h"
}

Q_DECLARE_METATYPE(QJsonObject);

class MoozApi : public QObject
{
    Q_OBJECT

public:
    MoozApi();

    //处理发送的数据
    void SendGcode(uint32_t seqNum, uint8_t *cmdArray, uint16_t arrayLength);
    void GetLevellingInfo(uint32_t seqNum);
    void GetCurrentPosition(uint32_t seqNum);
    void GetCurrentTemperature(uint32_t seqNum);
    void GetCurrentPower(uint32_t seqNum);
    void GetCurrentTerminalType(uint32_t seqNum);
    void GetCurrentRunningStatus(uint32_t seqNum);
    void SetAutoLevellingParams(uint32_t seqNum);
    void GetCurrentZeroDeviationValue(uint32_t seqNum);
    void GetCurrentMemoryStatus(uint32_t seqNum);
    void GetCurrentPrintPageData(uint32_t seqNum);
    void GetCurrentInfoPageData(uint32_t seqNum);
    void GetFractureStatus(uint32_t seqNum);
    void GetCurrentLanguage(uint32_t seqNum);
    void SetCurrentLanguage(uint32_t seqNum, uint8_t languageState);
    void SetAutoShutdown(uint32_t seqNum, uint8_t isAuto);
    void GetCurrentPageID(uint32_t seqNum, uint8_t pageId);
    void GetCurrentFileNum(uint32_t seqNum);
    void GetCurrentFileName(uint32_t seqNum, uint8_t begin, uint8_t end);
    void SetFileTransferConfig(uint32_t seqNum, uint8_t mode, const char *fileName, uint32_t fileSize, uint16_t packetSize, const char *downloadUrl);
    void SendFileTransferConfig(uint32_t seqNum, uint16_t currentPacketIndex, uint16_t totalPacketNum, uint8_t supplementFlag, uint8_t *data, uint8_t dataLength);
    void CheckFile(uint32_t seqNum, const char *fileName, uint32_t fileSize, uint8_t verityType, uint32_t verifyCode);
    void CheckPackage(uint32_t seqNum, const char *fileName, uint16_t beginPacketIndex, uint16_t endPacketIndex);
    void SetTCPClient(uint32_t seqNum, const char *severUrl, uint32_t port);
    void DisconnectWifi(uint32_t seqNum, uint8_t resver);

    //接受的回调函数
    static void SendGcode_CB(DobotV3CmdInterCBParm packet);
    static void GetLevellingInfo_CB(DobotV3CmdInterCBParm packet);
    static void GetCurrentPosition_CB(DobotV3CmdInterCBParm packet);
    static void GetCurrentTemperature_CB(DobotV3CmdInterCBParm packet);
    static void GetCurrentPower_CB(DobotV3CmdInterCBParm packet);
    static void GetCurrentTerminalType_CB(DobotV3CmdInterCBParm packet);
    static void GetCurrentRunningStatus_CB(DobotV3CmdInterCBParm packet);
    static void SetAutoLevellingParams_CB(DobotV3CmdInterCBParm packet);
    static void GetCurrentZeroDeviationValue_CB(DobotV3CmdInterCBParm packet);
    static void GetCurrentMemoryStatus_CB(DobotV3CmdInterCBParm packet);
    static void GetCurrentPrintPageData_CB(DobotV3CmdInterCBParm packet);
    static void GetCurrentInfoPageData_CB(DobotV3CmdInterCBParm packet);
    static void GetFractureStatus_CB(DobotV3CmdInterCBParm packet);
    static void GetCurrentLanguage_CB(DobotV3CmdInterCBParm packet);
    static void SetCurrentLanguage_CB(DobotV3CmdInterCBParm packet);
    static void SetAutoShutdown_CB(DobotV3CmdInterCBParm packet);
    static void GetCurrentPageID_CB(DobotV3CmdInterCBParm packet);
    static void GetCurrentFileNum_CB(DobotV3CmdInterCBParm packet);
    static void GetCurrentFileName_CB(DobotV3CmdInterCBParm packet);
    static void SetFileTransferConfig_CB(DobotV3CmdInterCBParm packet);
    static void SendFileTransferConfig_CB(DobotV3CmdInterCBParm packet);
    static void CheckFile_CB(DobotV3CmdInterCBParm packet);
    static void CheckPackage_CB(DobotV3CmdInterCBParm packet);
    static void SetTCPClient_CB(DobotV3CmdInterCBParm packet);
    static void DisconnectWifi_CB(DobotV3CmdInterCBParm packet);

public:
    void initProtocol();

public slots:
    void slots_registerLink(DobotV3_SendData SendData, DobotV3_ReadData ReadData);

signals:
    void signals_receiveData(bool isTimeOut, QJsonObject jsonObj, quint32 seqNum);

public:
    static MoozApi *m_staticMoozApi;
    QTimer *m_timer;
    bool m_isStop;
};

#endif // MOOZAPI_H
