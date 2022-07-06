#include "DemoPlugin.h"

#include <QDebug>

const QString DemoPlugin::PluginID = "Demo";

DemoPlugin::DemoPlugin(QObject *parent) : DPluginInterface(parent)
{

}

/* 收到消息 */
void DemoPlugin::pReceiveMassage_slot(QString id, QJsonObject obj)
{
    if (id == "ALL") {
        qDebug() << "[ALL] {Demo} get obj" << obj;
        if (obj.contains("METHOD")) {
            QString method = obj.value("METHOD").toString();
            if (method == "EXIT") {
                qDebug() << "demo will EXIT";
            }
        }
    } else if (id == PluginID) {
        handleDemoCommand(obj);
    }
}

/* [!!!消息分发!!!] */
void DemoPlugin::handleDemoCommand(const QJsonObject &obj)
{
//    qDebug() << "DemoPlugin" << obj;

    DemoRequestPacket packet;
    packet.setPacketFromObj(obj);

    if (packet.api.contains("your api")) {
        /* handleYourAPI(packet) */
    }

    DemoResultPacket resPacket(packet);

    /* 回复消息 */
    emit pSendMessage_signal(PluginID, resPacket.getResultObj(true));
}

