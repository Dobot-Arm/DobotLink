#ifndef INDUSTRIALROBOTPLUGIN_H
#define INDUSTRIALROBOTPLUGIN_H

#include "DPluginInterface.h"

#include <QObject>
#include <QJsonObject>
#include <QMap>
#include <QSet>
#ifdef Q_OS_ANDROID
#include <luamobile.h>
#endif
#include "MessageCenter/DPacket.h"
#include "Device.h"

class IndustrialRobotPlugin : public DPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Dobot.plugin.interface")
    Q_INTERFACES(DPluginInterface)

public:
    static const QString PluginName;
    static const QString Version;
    IndustrialRobotPlugin(QObject *parent = nullptr);
    virtual ~IndustrialRobotPlugin() override;
    virtual QString getVersion() override;

    //获取udp和tcp防火墙入栈规则命令,以及删除命令
    static QString GetFirewallInRuleCmdUdp();
    static QString GetFirewallInRuleCmdTcp();

private:
    QMap<quint64, DRequestPacket> m_requestPacketMap;

    //多人搜索时，临时缓存
    QMap<quint64, DRequestPacket> m_reqSearchPacket;

    Device *m_device;


    //wsport就是ws客户端连接的端口，因为这些客户端与dobotlink都是处在同一个电脑上，ip一样的，那么port肯定不一样
    //这样，通过port也能够区分并将消息发给对应的客户端。
    /*支持连接多台控制器，因为在同一个局域网内每一台控制器的ip是唯一的，所以可以做映射,
     * 特殊情况：一台MG400设备，可以被多个Pro连接，这样，当同一台电脑，开了多个pro，然后都连接了dobotlink，结果就凉凉了，所以一个ip对应多个设备
    */
    QMap<QString, //控制器ip，也就是portName
        QSet<Device*>  //控制器业务对象
    > m_mapDevices;

    Device* GetDeviceByPortNameAndWsPort(QString strPortName,quint16 wsPort);

    void disconnectDobot(quint16 port = 0);
    void handleDobotLinkCommand(const QJsonObject &obj);
    void handleIndustrialRobotCommand(const QJsonObject &obj);

private slots:
    virtual void pReceiveMassage_slot(QString id, QJsonObject obj) override;

    void handleReplyMessage_slot(quint64 id, QJsonValue value);
    void handleErrorOccured_slot(quint64 id, int errCode, QString errStr);

    void handleUDPNotify_slot(quint64 id, QJsonObject obj);
    void handleDebuggerNotify_slot(QString msg);
};

#endif // INDUSTRIALROBOTPLUGIN_H
