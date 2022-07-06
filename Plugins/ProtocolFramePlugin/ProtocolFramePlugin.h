#ifndef PROTOCOLFRAMEPLUGIN_H
#define PROTOCOLFRAMEPLUGIN_H

#include "DPluginInterface.h"
#include "ProtocolFramePacket.h"
#include "DobotType.h"
#include "Controller.h"
#include "MyThread.h"
#include "MoozApi.h"
#include <QObject>
#include <QJsonObject>
#include <QMap>

class IdPortPacket {
public:
    quint64 id;
    quint16 wsPort;
    IdPortPacket();
    IdPortPacket(quint64 packetId, quint16 packetWsPort) {
        id = packetId;
        wsPort = packetWsPort;
    }
};

class ProtocolFramePlugin : public DPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Dobot.plugin.interface")
    Q_INTERFACES(DPluginInterface)

public:
    ProtocolFramePlugin(QObject *parent = nullptr);
    virtual ~ProtocolFramePlugin() override;
    virtual QString getVersion() override;

private:
    void handleCommand(const QJsonObject &obj);
    void handlePortCommand(const ProtocolFramePacket &packet);
    void handleProtocolFrameCommand(const ProtocolFramePacket &packet);

    void handleSendGcode(const ProtocolFramePacket &packet);
    void handleGetLevellingInfo(const ProtocolFramePacket &packet);
    void handleGetCurrentPosition(const ProtocolFramePacket &packet);
    void handleGetCurrentTemperature(const ProtocolFramePacket &packet);
    void handleGetCurrentPower(const ProtocolFramePacket &packet);
    void handleGetCurrentTerminalType(const ProtocolFramePacket &packet);
    void handleGetCurrentRunningStatus(const ProtocolFramePacket &packet);
    void handleSetAutoLevellingParams(const ProtocolFramePacket &packet);
    void handleGetCurrentZeroDeviationValue(const ProtocolFramePacket &packet);
    void handleGetCurrentMemoryStatus(const ProtocolFramePacket &packet);
    void handleGetCurrentPrintPageData(const ProtocolFramePacket &packet);
    void handleGetCurrentInfoPageData(const ProtocolFramePacket &packet);
    void handleGetFractureStatus(const ProtocolFramePacket &packet);
    void handleGetCurrentLanguage(const ProtocolFramePacket &packet);
    void handleSetCurrentLanguage(const ProtocolFramePacket &packet);
    void handleSetAutoShutdown(const ProtocolFramePacket &packet);
    void handleGetCurrentPageID(const ProtocolFramePacket &packet);
    void handleGetCurrentFileNum(const ProtocolFramePacket &packet);
    void handleGetCurrentFileName(const ProtocolFramePacket &packet);
    void handleSetFileTransferConfig(const ProtocolFramePacket &packet);
    void handleSendFileTransferConfig(const ProtocolFramePacket &packet);
    void handleCheckFile(const ProtocolFramePacket &packet);
    void handleCheckPackage(const ProtocolFramePacket &packet);
    void handleSetTCPClient(const ProtocolFramePacket &packet);
    void handleDisconnectWifi(const ProtocolFramePacket &packet);

public slots:
    virtual void pReceiveMassage_slot(QString id, QJsonObject obj) override;
    void slots_receiveData(bool isTimeOut, QJsonObject jsonObj, quint32 seqNum);

public:
    static const QString PluginName;
    static const QString Version;
    Controller *m_controller;
    MyThread *m_mythread;
    MoozApi *m_moozApi;
    uint32_t m_seqNum;
    bool m_initflag;
    QMap<uint32_t, IdPortPacket> m_idPortSeqMap;
};

#endif // PROTOCOLFRAMEPLUGIN_H
