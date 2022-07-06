#include "DownloadPlugin.h"

#include <QDebug>
#include "MessageCenter/DPacket.h"
#include "../version.h"
const QString DownloadPlugin::PluginName = "Download";
const QString DownloadPlugin::Version = VERSION_PLUGIN_DOWNLOAD;

DownloadPlugin::DownloadPlugin(QObject *parent) : DPluginInterface(parent)
{
    m_dfufile = new DDfufile(this);
    m_dmcuisp = new DMcuisp(this);
    m_dkflash = new DKflash(this);
    m_dmultifunc = new DMultiFunc(this);
    m_dgetversioninfo = new DGetVersion(this);
    connect(m_dfufile, &DDfufile::onProcessReadyRead_signal, this, [=](QString message){
        DNotificationPacket packet(m_handlingWsPort);
        QJsonObject messageObj;
        messageObj.insert("message", message);
        QJsonObject noticeObj = packet.getNotificationObj("dobotlink.download.process", messageObj);
        emit pSendMessage_signal(PluginName, noticeObj);
    });

    connect(m_dmcuisp, &DMcuisp::onProcessReadyRead_signal, this, [=](QString message){
        DNotificationPacket packet(m_handlingWsPort);
        QJsonObject messageObj;
        messageObj.insert("message", message);
        QJsonObject noticeObj = packet.getNotificationObj("dobotlink.download.process", messageObj);
        emit pSendMessage_signal(PluginName, noticeObj);
    });

    connect(m_dkflash, &DKflash::onProcessReadyRead_signal, this, [=](QString message){
        DNotificationPacket packet(m_handlingWsPort);
        QJsonObject messageObj;
        messageObj.insert("message", message);
        QJsonObject noticeObj = packet.getNotificationObj("dobotlink.download.process", messageObj);
        emit pSendMessage_signal(PluginName, noticeObj);
    });

    connect(m_dmultifunc, &DMultiFunc::onProcessReadyRead_signal, this, [=](QString message){
        DNotificationPacket packet(m_handlingWsPort);
        QJsonObject messageObj;
        messageObj.insert("message", message);
        QJsonObject noticeObj = packet.getNotificationObj("dobotlink.download.process", messageObj);
        emit pSendMessage_signal(PluginName, noticeObj);
    });

    connect(m_dfufile, &DDfufile::onFinish_signal,
            this, &DownloadPlugin::onFinish_slot);
    connect(m_dmcuisp, &DMcuisp::onFinish_signal,
            this, &DownloadPlugin::onFinish_slot);
    connect(m_dkflash, &DKflash::onFinish_signal,
            this, &DownloadPlugin::onFinish_slot);
    connect(m_dmultifunc, &DMultiFunc::onFinish_signal,
            this, &DownloadPlugin::onFinish_slot);

    connect(m_dfufile, &DDfufile::onStop_signal,
            this, &DownloadPlugin::onStop_slot);
    connect(m_dmcuisp, &DMcuisp::onStop_signal,
            this, &DownloadPlugin::onStop_slot);
    connect(m_dkflash, &DKflash::onStop_signal,
            this, &DownloadPlugin::onStop_slot);
    connect(m_dmultifunc, &DMultiFunc::onStop_signal,
            this, &DownloadPlugin::onStop_slot);

    connect(m_dgetversioninfo, &DGetVersion::on_getVersionInfo_signal,
            this, &DownloadPlugin::on_getVersionInfo_slot);
    connect(m_dgetversioninfo, &DGetVersion::on_getVersion_signal,
            this, &DownloadPlugin::on_getVersion_slot);

    connect(m_dfufile, &DDfufile::onProcessError_signal,
            this, &DownloadPlugin::onErrorHandle_slot);
    connect(m_dmcuisp, &DMcuisp::onProcessError_signal,
            this, &DownloadPlugin::onErrorHandle_slot);
    connect(m_dkflash, &DKflash::onProcessError_signal,
            this, &DownloadPlugin::onErrorHandle_slot);
    connect(m_dmultifunc, &DMultiFunc::onProcessError_signal,
            this, &DownloadPlugin::onErrorHandle_slot);
    connect(m_dgetversioninfo, &DGetVersion::on_getVersionErr_signal,
            this, &DownloadPlugin::onErrorHandle_slot);

}

DownloadPlugin::~DownloadPlugin()
{

}

QString DownloadPlugin::getVersion()
{
    return Version;
}

/* 收到消息 */
void DownloadPlugin::pReceiveMassage_slot(QString id, QJsonObject obj)
{
    if (id == "ALL") {
        qDebug() << "[ALL] {Download} get obj" << obj;
        if (obj.contains("METHOD")) {
            QString method = obj.value("METHOD").toString();
            if (method == "EXIT") {
                qDebug() << "DownloadPlugin will EXIT.";
                stopProcess();
            }
        }
    } else if (id == PluginName) {
        handleDownloadCmd(obj);
    }
}

void DownloadPlugin::stopProcess()
{
    m_dfufile->stopProcess();
    m_dmcuisp->stopProcess();
    m_dkflash->stopProcess();
    m_dmultifunc->stopProcess();
}

void DownloadPlugin::onFinish_slot(bool ok, quint64 id)
{
    qDebug() << "download finish:" << ok;

    DRequestPacket packet = m_requestPacketMap.value(id);
    DResultPacket resPacket(packet);
    if (ok) {
        emit pSendMessage_signal(PluginName, resPacket.getResultObj());
    } else {
        QJsonObject errorObj;
        errorObj.insert("code", 300);
        errorObj.insert("message", "Firmware download failed.");
        emit pSendMessage_signal(PluginName, resPacket.setErrorObj(errorObj));
    }
}

void DownloadPlugin::onErrorHandle_slot(quint64 id, int code, QString message)
{
    DRequestPacket packet = m_requestPacketMap.value(id);
    DResultPacket errPacket(packet);
    emit pSendMessage_signal(PluginName, errPacket.getErrorObj(code, message));
}

void DownloadPlugin::on_getVersionInfo_slot(QString res)
{
    DRequestPacket packet = m_requestPacketMap.value(m_handlingId);
    DResultPacket resPacket(packet);
    QJsonObject paramsObj;
    paramsObj.insert("info", res);
    emit pSendMessage_signal(PluginName, resPacket.getResultObj(paramsObj));
}

void DownloadPlugin::on_getVersion_slot(QString ver, QString length, QString md5, int cameraNum)
{
    DRequestPacket packet = m_requestPacketMap.value(m_handlingId);
    DResultPacket resPacket(packet);
    QJsonObject paramsObj;
    paramsObj.insert("version", ver);
    paramsObj.insert("length", length);
    paramsObj.insert("md5", md5);
    paramsObj.insert("cameraNum", cameraNum);
    emit pSendMessage_signal(PluginName, resPacket.getResultObj(paramsObj));
}

void DownloadPlugin::onStop_slot(quint64 id)
{
    qDebug() << "Stop firmware update";

    DRequestPacket packet = m_requestPacketMap.value(id);
    DResultPacket resPacket(packet);

    emit pSendMessage_signal(PluginName, resPacket.getResultObj());

}

/* [!!!消息分发!!!] */
void DownloadPlugin::handleDownloadCmd(const QJsonObject &obj)
{
    DRequestPacket packet;
    packet.setPacketFromObj(obj);
    DResultPacket resPacket(packet);

    m_requestPacketMap.insert(packet.id, packet);
    m_handlingId = packet.id;
    m_handlingWsPort = packet.wsPort;

    QString device = packet.getParamValue("device").toString();
    if (packet.api == "GetVersionInfo") {
        QString version = packet.getParamValue("version").toString();
        m_dgetversioninfo->getVersionInfo(packet.id, device, version);
        return;
    } else if (packet.api == "GetLatestVersionInfo") {
        m_dgetversioninfo->getLatestVersionInfo(packet.id, device);
        return;
    } else if (packet.api == "GetLatestVersion") {
        m_dgetversioninfo->getLatestVersion(packet.id, device);
        return;
    }

    QString fileName = packet.getParamValue("fileName").toString();

    if (device == "MagicBox" or device == "MagicianLite") {
        if (device == "MagicBox") {
            m_dfufile->m_type = DDfufile::DFU_MAGICBOX;
        } else if (device == "MagicianLite") {
            m_dfufile->m_type = DDfufile::DFU_MAGICIANLITE;
        }
        if (packet.api == "StartFirmware"){
            m_dfufile->setFileName(fileName);
            m_dfufile->startDownload(m_handlingId);
        } else if (packet.api == "StopFirmware") {
            m_dfufile->stopDownload(m_handlingId);
        } else {
            m_requestPacketMap.remove(packet.id);
            emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_INVALID_COMMAND));
            return;
        }

    } else if (device == "Magician") {
        QString COM = packet.getParamValue("COM").toString();
        bool is3DPrinter = packet.getParamValue("is3DPrinter").toBool();
        if (packet.api == "StartFirmware"){
            m_dmcuisp->setFileName(fileName);
            m_dmcuisp->startDownload(COM, m_handlingId, is3DPrinter);
        } else if (packet.api == "StopFirmware") {
            m_dmcuisp->stopDowenload(m_handlingId);
        } else {
            m_requestPacketMap.remove(packet.id);
            emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_INVALID_COMMAND));
            return;
        }

    } else if (device == "CameraCar" or device == "CameraArm"){
        QString COM = packet.getParamValue("COM").toString();
        if (packet.api == "StartFirmware"){
            m_dkflash->setFileName(fileName, device);
            m_dkflash->startDownload(COM, m_handlingId);
        } else if (packet.api == "StopFirmware") {
            m_dkflash->stopDownload(m_handlingId);
        } else {
            m_requestPacketMap.remove(packet.id);
            emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_INVALID_COMMAND));
            return;
        }

    } else if (device == "MagicianGO"){
        QString COM = packet.getParamValue("COM").toString();
        QString version = packet.getParamValue("version").toString();
        if (packet.api == "StartFirmware"){
            m_dmultifunc->setFileName(fileName);
            m_dmultifunc->startDownload(COM, version, m_handlingId);
        } else if (packet.api == "StopFirmware") {
            m_dmultifunc->stopDownload(m_handlingId);
        } else {
            m_requestPacketMap.remove(packet.id);
            emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_INVALID_COMMAND));
            return;
        }
    } else {
        m_requestPacketMap.remove(packet.id);
        emit pSendMessage_signal(PluginName, resPacket.getErrorObjWithCode(ERROR_INVALID_DEVICE));
        return;
    }
}


