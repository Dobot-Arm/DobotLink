#include "DUpgrade.h"

#include <QDebug>

#include "Module/DNetworkManager.h"
#include "Module/DSettings.h"

DUpgrade::DUpgrade(QObject *parent) : QObject(parent)
{
    m_network = DNetworkManager::getInstance();
    connect(m_network, &DNetworkManager::onReplyMassage_signal,
            this, &DUpgrade::handleNetworkReplyMessage_slot);
    connect(m_network, &DNetworkManager::onReplyData_signal,
            this, &DUpgrade::handleNetworkReplyData_slot);

    settings = DSettings::getInstance();
}

void DUpgrade::checkNewVersion(QString user, quint64 id)
{
    if (user.isEmpty()) {
        user = "DUpgrade";
    }
    m_handlingID = id;
    m_network->getDobotRequest("DobotLink/version.json", user);
}

int DUpgrade::compareVersion(QString v1, QString v2)
{
    QStringList v1list = v1.split(".", QString::SkipEmptyParts);
    QStringList v2list = v2.split(".", QString::SkipEmptyParts);

    int len1 = v1list.count();
    int len2 = v2list.count();
    int i;
    for (i = 0; i < qMin(len1, len2); ++i) {
        if (v1list.at(i).toUInt() > v2list.at(i).toUInt()) {
            return 1;
        } else if (v1list.at(i).toUInt() < v2list.at(i).toUInt()) {
            return -1;
        }
    }
    if (v1list.count() > v2list.count()) {
        for (int j = i; j < len1; ++j) {
            if (v1list.at(j).toUInt() != 0) {
                return 1;
            }
        }
    } else if (v1list.count() < v2list.count()) {
        for (int j = i; j < len2; ++j) {
            if (v2list.at(j).toUInt() != 0) {
                return 1;
            }
        }
    }
    return 0;
}

QJsonObject DUpgrade::getLocalVersion()
{
    QJsonObject obj;

    QJsonObject mainObj;
    mainObj.insert("version", settings->getVersion("main"));
    obj.insert("main", mainObj);

    QJsonObject pluginObj;
    pluginObj.insert("MagicDevicePlugin", settings->getVersion("MagicDevicePlugin"));
    pluginObj.insert("ArduinoPlugin", settings->getVersion("ArduinoPlugin"));
    pluginObj.insert("DownloadPlugin", settings->getVersion("DownloadPlugin"));
    pluginObj.insert("MicrobitPlugin", settings->getVersion("MicrobitPlugin"));
    pluginObj.insert("PyImageOMPlugin",settings->getVersion("PyImageOMPlugin"));
    pluginObj.insert("DebuggerlitePlugin",settings->getVersion("DebuggerlitePlugin"));
    pluginObj.insert("RunCmdPlugin",settings->getVersion("RunCmdPlugin"));
    obj.insert("plugins", pluginObj);

    return obj;
}

void DUpgrade::handleNetworkReplyMessage_slot(QString url, QJsonObject resObj, QString user)
{
    if (user == "DUpgrade" and url == "DobotLink/version.json") {
        QJsonObject section_main = resObj.value("main").toObject();

        QString latestV = section_main.value("version").toString();
        QJsonObject urlObj = section_main.value("url").toObject();

#ifdef Q_OS_WIN
        QString url = urlObj.value("win").toString();
#elif defined(Q_OS_MAC)
        QString url = urlObj.value("mac").toString();
#endif

        QString localV = settings->getVersion("main");
        qDebug().noquote() << QString("check new version. local:V%1 latest:V%2").arg(localV).arg(latestV);
        int res = compareVersion(localV, latestV);
        qDebug() << "compare" << res;
    } else if (user == "DMessageCenter" and url == "DobotLink/version.json") {
        emit getNewVersionRes_signal(resObj, m_handlingID);
    }
}

void DUpgrade::handleNetworkReplyData_slot(QString url, QByteArray resBa)
{
    if (url == "x") {
        qDebug() << "res" << resBa;
    }
}
