#ifndef MAGICDEVICEPLUGIN_H
#define MAGICDEVICEPLUGIN_H

//#define __wasm__

#ifndef __wasm__
#include "DPluginInterface.h"
#else
#include "emscripten/bind.h"
#endif

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QMap>

#include "MessageCenter/DPacket.h"
#include "MagicDevice.h"
#include "DBoxDownload.h"
class DeviceInfo {
public:
    QString portName;
    QString description;
    QString status;
};

class MagicDevice;
class QUdpSocket;

#ifndef __wasm__
class MagicDevicePlugin : public DPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Dobot.plugin.interface")
    Q_INTERFACES(DPluginInterface)
#else
class MagicDevicePlugin : public QObject
{
    Q_OBJECT
#endif
public:
    static const QString PluginName;
    static const QString Version;

#ifndef __wasm__
    MagicDevicePlugin(QObject *parent = nullptr);
    ~MagicDevicePlugin();
    virtual QString getVersion() override;
#else
    MagicDevicePlugin();
    ~MagicDevicePlugin();
    QString getVersion();
#endif

private:
    QMap<quint64, DRequestPacket> m_requestPacketMap;
    QMap<QString, MagicDevice *> m_deviceMap;
    QMap<QString, MagicDevice *> m_checkDeviceMap;
    bool m_researchFileter;
    QStringList m_portNameList;
    quint64 m_handlingid;
#ifndef __wasm__
    QUdpSocket *m_udpSocket;
#endif
    QMap<QString, QString> m_ipAndBroadcastMap;
    QMap<QString, QString> m_m1deviceMap;
    QMap<QString, DeviceInfo> m_preDeviceMap;
    bool m_isSearchingDevices;
    QStringList m_handlingSearchDeviceList;
    QTimer *m_searchTimer;
    QJsonObject m_sourceObj;

    void _handleDobotLinkCommand(const QJsonObject &obj);
    void _handleMagicDeviceCommand(const QJsonObject &obj);

    void _getAvailableSerialPort(QStringList filter = QStringList());
    QJsonArray _getSearchResult();

    void pSearchDobot(const DRequestPacket &packet);
    void pConnectDobot(const DRequestPacket &packet);
    void pDisconnectDobot(MagicDevice *device, const DRequestPacket &packet);
    void pDisconnectDobot(quint16 wsport);

    bool _checkActionApi(QString api);
    int _getDefaultTimeoutValue(QString devideType, QString cmd);
    bool _handleActionCmd(MagicDevice *device, const DRequestPacket &packet);

    bool _checkQueueApi(QString api);
    bool _handleQueueCmd(MagicDevice *device, const DRequestPacket &packet);

    void _pQueuedCmdStop(MagicDevice *device, const DRequestPacket &packet);
    bool _pSendCommand(MagicDevice *device, const DRequestPacket &packet);
    void _pSetCommuTimeout(MagicDevice *device, const DRequestPacket &packet);

    void _handleDownloadCmd(MagicDevice *device, const DRequestPacket &packet);

    void _closeAllDevice();

    inline void _sendResMessage(const quint64 id, const QJsonValue resValue);
    inline void _sendResMessage(const DRequestPacket &request, const QJsonValue resValue);
    inline void _sendErrorMessage(const quint64 id, const ErrorType type);
    inline void _sendErrorMessage(const DRequestPacket &request, const ErrorType type);

    void _broadcastForSearchM1();
    QStringList _getHostIpList();

#ifndef __wasm__
public slots:
    virtual void pReceiveMassage_slot(QString id, QJsonObject obj) override;
#else
public slots:
    void pReceiveMassage_slot(QString id, QJsonObject obj);

signals:
    void pSendMessage_signal(QString id, QJsonObject obj);
#endif

private slots:
    void _onSearchTimeout_slot();

    /* message from Device */
    void _handleReceiveMessage_slot(quint64 id, QString cmd, int res, QJsonValue params);
    void _handleDownloadFinished_slot(quint64 id, bool isOk);
    void _onUdpReadyRead_slot();
};

#endif // MAGICDEVICEPLUGIN_H
