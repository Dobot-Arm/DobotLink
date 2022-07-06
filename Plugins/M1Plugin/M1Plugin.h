#ifndef M1PLUGIN_H
#define M1PLUGIN_H

#include "DPluginInterface.h"

#include <QObject>
#include <QJsonObject>
#include "M1Packet.h"
#include "M1Controller.h"

class M1Plugin : public DPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Dobot.plugin.interface")
    Q_INTERFACES(DPluginInterface)

public:
    static const QString PluginName;
    static const QString Version;

    M1Plugin(QObject *parent = nullptr);

    QString getVersion() override;

private:
    M1Controller *m_controller;

    void handleDobotLinkCommand(const QJsonObject &obj);
    void handleM1Command(const QJsonObject &obj);

    bool checkPacket(const M1Packet &packet);
    void handleGetPoseCmd(const M1Packet &packet);
    void handleActionCmd(const M1Packet &packet);
    void handleStateCmd(const M1Packet &packet);
    void handleParamsCmd(const M1Packet &packet);
    void handleDobotCmd(const M1Packet &packet);
    void handleIOCmd(const M1Packet &packet);
    void handleDeviceCmd(const M1Packet &packet);
    void handleSensorCmd(const M1Packet &packet);
    void handleHHTTrigCmd(const M1Packet &packet);
    void handleFirmwareCmd(const M1Packet &packet);
    void handleOtherCmd(const M1Packet &packet);

signals:
    void closeAllDevice_signal();

private slots:
    virtual void pReceiveMassage_slot(QString id, QJsonObject obj) override;
    void handleDeviceDisconnected_slot(QString portName);
    void sendResPacket_slot(QJsonObject resObj);
};

#endif // M1PLUGIN_H
