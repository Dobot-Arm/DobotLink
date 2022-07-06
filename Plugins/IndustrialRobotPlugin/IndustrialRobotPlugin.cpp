#include "IndustrialRobotPlugin.h"

#include <QJsonArray>
#include <QList>
#include <QUdpSocket>
#include <QTime>
#include <QDebug>
#include <QtConcurrent>

#include "../version.h"

#define DEVICE_DLL_ERROR_BASE (120)

const QString IndustrialRobotPlugin::PluginName = "IndustrialRobot";
const QString IndustrialRobotPlugin::Version = VERSION_PLUGIN_INDUSTRIAL;

IndustrialRobotPlugin::IndustrialRobotPlugin(QObject *parent) : DPluginInterface(parent)
{
    m_device = new Device(this);
    connect(m_device, &Device::onReplyMessage_signal,
            this, &IndustrialRobotPlugin::handleReplyMessage_slot);
    connect(m_device, &Device::onErrorOccured_signal,
            this, &IndustrialRobotPlugin::handleErrorOccured_slot);
    connect(m_device, &Device::onNotifyMessage_signal,
            this, &IndustrialRobotPlugin::handleUDPNotify_slot);
    connect(m_device, &Device::onDebuggerNotify_signal,
            this, &IndustrialRobotPlugin::handleDebuggerNotify_slot);

    QtConcurrent::run([]{
        QProcess *p = new QProcess();
        p->setProcessChannelMode(QProcess::MergedChannels);
        connect(p, &QProcess::readyRead, [p]{
            QTextCodec *codec = QTextCodec::codecForName("GBK");
            qDebug().noquote().nospace()<<"QProcess::readyRead-->"<<codec->toUnicode(p->readAll());
        });
        connect(p, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),[](int code, QProcess::ExitStatus sta){
            qDebug().noquote().nospace()<<"QProcess::finished-->"<<code<<","<<sta;
        });
        connect(p, &QProcess::errorOccurred, [](QProcess::ProcessError error){
            qDebug().noquote().nospace()<<"QProcess::errorOccurred-->"<<error;
        });

        //先删除，防止太多重复的
        p->start("netsh advfirewall firewall delete rule name=\"dobotlink\"");
        p->waitForFinished(5000);
        p->start("netsh advfirewall firewall delete rule name=\"DobotLink\"");
        p->waitForFinished(5000);
        //删掉历史遗留的
        p->start("netsh advfirewall firewall delete rule name=\"DobotStudio2020UDPPort\"");
        p->waitForFinished(5000);
        p->start("netsh advfirewall firewall delete rule name=\"DobotStudio2020TCPPort\"");
        p->waitForFinished(5000);

        QThread::sleep(2);
        //再添加，防止前端没有主动调用OpenFireWall开启
        QString strUdpAdd = IndustrialRobotPlugin::GetFirewallInRuleCmdUdp();
        QString strTcpAdd = IndustrialRobotPlugin::GetFirewallInRuleCmdTcp();
        qDebug().noquote().nospace()<<strUdpAdd<<'\n';
        qDebug().noquote().nospace()<<strTcpAdd<<'\n';

        p->start(strUdpAdd);
        p->waitForFinished(5000);
        p->start(strTcpAdd);
        p->waitForFinished(5000);
        p->deleteLater();
    });
}

IndustrialRobotPlugin::~IndustrialRobotPlugin()
{

}

QString IndustrialRobotPlugin::GetFirewallInRuleCmdUdp()
{/*DobotStudio2020UDPPort
不要使用这个名字，因为当程序在用户电脑第一次使用时，因为"Windows安全中心警报"和"Windows防火墙"的原因，会提示你访问网络的权限，
结果用户没有勾选允许“专用网络”“公用网络”，此时在"Windows入栈规则"里面系统自动用该进程名生成入栈规则，并且是禁止访问的。
结果你使用 DobotStudio2020UDPPort 名字生成的规则又是可访问。最终被系统判定为禁止udp。
非常恶心的是：进程名明明是'DobotLink'，结果Windows全给你用小写的*/
    static QString strCmd = QString("netsh advfirewall firewall add rule name=\"dobotlink\" dir=in "
                            "action=allow program=\"%1\" enable=yes profile=any "
                            "protocol=UDP localport= 5000,5001,5002,5003,5004")
                            .arg(QDir::toNativeSeparators(qApp->applicationFilePath()));
    return strCmd;
}

QString IndustrialRobotPlugin::GetFirewallInRuleCmdTcp()
{//DobotStudio2020TCPPort
    static QString strCmd = QString("netsh advfirewall firewall add rule name=\"dobotlink\" dir=in "
                            "action=allow program=\"%1\" enable=yes profile=any "
                            "protocol=TCP localport= 8172")
                            .arg(QDir::toNativeSeparators(qApp->applicationFilePath()));
    return strCmd;
}

QString IndustrialRobotPlugin::getVersion()
{
    return Version;
}

/* 收到消息 */
void IndustrialRobotPlugin::pReceiveMassage_slot(QString id, QJsonObject obj)
{
    if (id == "ALL") {
        handleDobotLinkCommand(obj);
    } else if (id.contains(PluginName)) {
        handleIndustrialRobotCommand(obj);
    }
}

void IndustrialRobotPlugin::handleReplyMessage_slot(quint64 id, QJsonValue value)
{
    DRequestPacket packet = m_requestPacketMap.take(id);
    DResultPacket resPacket(packet);

    emit pSendMessage_signal(PluginName, resPacket.getResultObj(value));
}

void IndustrialRobotPlugin::handleDebuggerNotify_slot(QString msg)
{
    DNotificationPacket packet(0);
    QJsonObject obj;
    obj.insert("msg", msg);
    emit pSendMessage_signal(PluginName, packet.getNotificationObj("IndustrialMsg", obj));
}

void IndustrialRobotPlugin::handleUDPNotify_slot(quint64 id, QJsonObject obj)
{
    QString method;
    DNotificationPacket packet(m_device->getWsPort());
    if(id==5000){
        method = "GetClientMsg";
    }
    if(id==5001){
        method = "Get1stCursorMsg";
    }
    if(id==5002){
        method = "Get2ndCursorMsg";
    }
    if(id==5003){
        method = "GetSpecialMsg";
    }
    if(id==5004){
        method = "GetBlockHighlightId";
    }


    emit pSendMessage_signal(PluginName, packet.getNotificationObj(method, obj));
}

void IndustrialRobotPlugin::handleErrorOccured_slot(quint64 id, int errCode, QString errStr)
{
    DRequestPacket packet = m_requestPacketMap.take(id);
    DResultPacket resPacket(packet);

    if (errStr.size() == 0) {
        emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(errCode));
    } else {
        emit pSendMessage_signal(PluginName, resPacket.getErrorObj(errCode, errStr));
    }
}

/* [!!!消息分发!!!] */
void IndustrialRobotPlugin::handleDobotLinkCommand(const QJsonObject &obj)
{
    qDebug() << "[ALL] {IndustrialRobotPlugin} get obj" << obj;

    if (obj.contains("METHOD")) {
        QString method = obj.value("METHOD").toString();

        if (method == "EXIT") {
            //... close all device...
            disconnectDobot();
        } else if (method == "CloseWebSocket") {
            QJsonObject params = obj.value("params").toObject();
            quint16 port = static_cast<quint16>(params.value("WSport").toInt());
            disconnectDobot(port);
        }
    }
}

void IndustrialRobotPlugin::disconnectDobot(quint16 port)
{
    qDebug() << "close robot device, port:" << port;
    m_device->pDisconnectDobot();
}

void IndustrialRobotPlugin::handleIndustrialRobotCommand(const QJsonObject &obj)
{
    DRequestPacket packet;
    packet.setPacketFromObj(obj);
    DResultPacket resPacket(packet);

    m_requestPacketMap.insert(packet.id, packet);

    if (packet.api == "ConnectDobot") {
        if (packet.getParamValue("portName").toString().isEmpty()) {
            m_requestPacketMap.remove(packet.id);

            emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_INVALID_PORTNAME));
            return;
        }
        m_device->setWsPort(packet.wsPort);
    }

    m_device->sendCommand(packet.api, packet.id, packet.paramsObj);
}

