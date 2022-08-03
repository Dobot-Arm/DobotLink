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
    /*//这个device只做设备的搜索操作
    connect(m_device, &Device::onNotifyMessage_signal,
            this, &IndustrialRobotPlugin::handleUDPNotify_slot);
    connect(m_device, &Device::onDebuggerNotify_signal,
            this, &IndustrialRobotPlugin::handleDebuggerNotify_slot);
    */

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

    if (packet.id == 0 || packet.wsPort == 0)
    {//之所以存在这种情况，是因为Mobdebug也会发这个消息给device，device然后转发给该插件，而它的id却是0
        Device* pDevice = qobject_cast<Device*>(sender());
        if (pDevice != nullptr)
        {
            packet.wsPort = pDevice->getWsPort();
        }
    }

    DResultPacket resPacket(packet);

    if (packet.api == "ConnectDobot")
    {//收到device的消息，而且是连接的消息，则保存device
        bool bSaved = false;
        Device* pDevice = qobject_cast<Device*>(sender());
        QString strPortName = packet.getParamValue("portName").toString();

        /* 坑。。。。。。
         * 之所以需要判断object和bool，是因为在Device::pConnectDobot里面也连接了Module::onReceiveData_signal的信号槽，结果该信号被发射时，
         * 2个槽函数都被执行，往外发送的消息是object，另一个却是bool。可能会存在先后执行的问题，所以只能这么处理
        */
        if (value.isObject())
        {
            QJsonObject jo = value.toObject();
            if (jo.contains("value") && jo.value("value").isString())
            {
                QString str = jo.value("value").toString();
                if ("connected" == str)
                {
                    //m_mapDevices[strPortName] = pDevice;
                    auto itr = m_mapDevices.find(strPortName);
                    if (itr == m_mapDevices.end())
                    {
                        m_mapDevices[strPortName] = QSet<Device*>({pDevice});
                    }
                    else
                    {
                        itr->insert(pDevice);
                    }
                    bSaved = true;
                }
            }
        }
        else if (value.isBool())
        {
            if (value.toBool())
            {
                //m_mapDevices[strPortName] = pDevice;
                auto itr = m_mapDevices.find(strPortName);
                if (itr == m_mapDevices.end())
                {
                    m_mapDevices[strPortName] = QSet<Device*>({pDevice});
                }
                else
                {
                    itr->insert(pDevice);
                }
                bSaved = true;
            }
        }

        if (!bSaved)
        {//没有连接成功，则删掉对象
            pDevice->deleteLater();
        }
        emit pSendMessage_signal(PluginName, resPacket.getResultObj(value));
    }
    else if (packet.api == "DisconnectDobot")
    {//收到device的消息，而且是断开连接的消息，则删掉device
        Device* pDevice = qobject_cast<Device*>(sender());
        QString strPortName = packet.getParamValue("portName").toString();

        auto itr = m_mapDevices.find(strPortName);
        if (itr != m_mapDevices.end())
        {
            auto deviceList = m_mapDevices[strPortName];
            deviceList.remove(pDevice);
            if (deviceList.isEmpty())
            {
                m_mapDevices.remove(strPortName);
            }
        }
        pDevice->deleteLater();
        emit pSendMessage_signal(PluginName, resPacket.getResultObj(value));
    }
    else if (packet.api == "SearchDobot")
    {
        while (!m_reqSearchPacket.empty())
        {
            auto itr = m_reqSearchPacket.begin();
            DResultPacket ressult(itr.value());
            emit pSendMessage_signal(PluginName, ressult.getResultObj(value));
            m_reqSearchPacket.erase(itr);
        }
    }
    else
    {
        emit pSendMessage_signal(PluginName, resPacket.getResultObj(value));
    }
}

void IndustrialRobotPlugin::handleDebuggerNotify_slot(QString msg)
{
    Device* pDevice = qobject_cast<Device*>(sender());
    quint16 port = 0;
    if (pDevice != nullptr)
    {
        port = pDevice->getWsPort();
    }

    DNotificationPacket packet(port);
    QJsonObject obj;
    obj.insert("msg", msg);
    emit pSendMessage_signal(PluginName, packet.getNotificationObj("IndustrialMsg", obj));
}

void IndustrialRobotPlugin::handleUDPNotify_slot(quint64 id, QJsonObject obj)
{
    Device* pDevice = qobject_cast<Device*>(sender());
    quint16 port = 0;
    if (pDevice != nullptr)
    {
        port = pDevice->getWsPort();
    }

    QString method;
    DNotificationPacket packet(port);
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

    if (packet.id == 0 || packet.wsPort == 0)
    {//之所以存在这种情况，是因为Mobdebug也会发这个消息给device，device然后转发给该插件，而它的id却是0
        Device* pDevice = qobject_cast<Device*>(sender());
        if (pDevice != nullptr)
        {
            packet.wsPort = pDevice->getWsPort();
        }
    }

    DResultPacket resPacket(packet);

    if (packet.api == "ConnectDobot")
    {//连接设备失败了，那么就要删掉
        sender()->deleteLater();

        if (errStr.size() == 0) {
            emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(errCode));
        } else {
            emit pSendMessage_signal(PluginName, resPacket.getErrorObj(errCode, errStr));
        }
    }
    else if (packet.api == "SearchDobot")
    {
        while (!m_reqSearchPacket.empty())
        {
            auto itr = m_reqSearchPacket.begin();
            DResultPacket ressult(itr.value());
            if (errStr.size() == 0) {
                emit pSendMessage_signal(PluginName, ressult.getErrorObjWithCode(errCode));
            } else {
                emit pSendMessage_signal(PluginName, ressult.getErrorObj(errCode, errStr));
            }
            m_reqSearchPacket.erase(itr);
        }
    }
    else
    {
        if (errStr.size() == 0) {
            emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(errCode));
        } else {
            emit pSendMessage_signal(PluginName, resPacket.getErrorObj(errCode, errStr));
        }
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
    if (0 == port)
    {//关闭所有
        auto itr = m_mapDevices.begin();
        while (itr != m_mapDevices.end())
        {
            foreach(Device* pDevice, itr.value())
            {
                pDevice->disconnect();
                pDevice->pDisconnectDobot();
                pDevice->deleteLater();
            }
            ++itr;
        }
        m_mapDevices.clear();
    }
    else
    {//只关闭port的
        auto itr = m_mapDevices.begin();
        while (itr != m_mapDevices.end())
        {
            foreach(Device* pDevice, itr.value())
            {
                if (pDevice->getWsPort() == port)
                {
                    pDevice->disconnect();
                    pDevice->pDisconnectDobot();
                    pDevice->deleteLater();

                    itr.value().remove(pDevice);
                    break;
                }
            }
            if (itr.value().isEmpty())
            {
                m_mapDevices.erase(itr);
                break;
            }
            ++itr;
        }
    }
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

        //请求连接时，先创建device
        Device* pDevice = new Device(this);
        pDevice->setWsPort(packet.wsPort);
        connect(pDevice, &Device::onReplyMessage_signal,
                this, &IndustrialRobotPlugin::handleReplyMessage_slot);
        connect(pDevice, &Device::onErrorOccured_signal,
                this, &IndustrialRobotPlugin::handleErrorOccured_slot);
        connect(pDevice, &Device::onNotifyMessage_signal,
                this, &IndustrialRobotPlugin::handleUDPNotify_slot);
        connect(pDevice, &Device::onDebuggerNotify_signal,
                this, &IndustrialRobotPlugin::handleDebuggerNotify_slot);
        pDevice->sendCommand(packet.api, packet.id, packet.paramsObj);
    }
    else if (packet.api == "SearchDobot" || packet.api == "AddSearchIP"
             || packet.api == "OpenNetUse" || packet.api == "OpenFireWall"
             || packet.api == "CheckSamba" || packet.api == "OpenSamba"
             || packet.api == "RestartComputer")
    {
        /*
        **搜索优化：
        **多个人同时搜索时，只做一次请求，
        **即：当某一个人在搜索时，没有返回结果之前，后面来的搜索请求，都不处理，而是等待第一个
        **搜索的结果，一次性分发给所有人
        */
        if (packet.api == "SearchDobot")
        {
            if (!m_reqSearchPacket.isEmpty())
            {//不为空，表示当前已经有人正在搜索，所以本次不要再搜索了
                m_reqSearchPacket.insert(packet.id, packet);
                return ;
            }
            //当前没有搜索，也要保存起来
            m_reqSearchPacket.insert(packet.id, packet);
        }
        m_device->sendCommand(packet.api, packet.id, packet.paramsObj);
    }
    else
    {
        QString strPortName = packet.getParamValue("portName").toString();
        if (strPortName.isEmpty()) {
            m_requestPacketMap.remove(packet.id);
            emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_INVALID_PORTNAME));
            return;
        }
        Device* pDevice = GetDeviceByPortNameAndWsPort(strPortName, packet.wsPort);
        if (!pDevice)
        {
            m_requestPacketMap.remove(packet.id);
            emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_INVALID_DEVICE));
            return;
        }
        pDevice->sendCommand(packet.api, packet.id, packet.paramsObj);
    }
}

Device* IndustrialRobotPlugin::GetDeviceByPortNameAndWsPort(QString strPortName,quint16 wsPort)
{
    if (m_mapDevices.contains(strPortName))
    {
        const auto& keys = m_mapDevices[strPortName];
        foreach(Device* p,keys)
        {
            if (wsPort == p->getWsPort())
            {
                return p;
            }
        }
    }
    return nullptr;
}
