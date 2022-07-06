#include "DNetworkManager.h"

#include <QJsonDocument>
#include <QUrlQuery>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QTimer>
#include <QDebug>

const QString BaseUrl = "https://cn.dobot.cc/";

DNetworkManager *DNetworkManager::getInstance()
{
    static DNetworkManager *instance = nullptr;
    if (instance == nullptr) {
        instance = new DNetworkManager();
    }
    return instance;
}

DNetworkManager::DNetworkManager(QObject *parent) : QObject(parent)
{
#ifdef QT_DEBUG
    qDebug() << __FUNCTION__ << "create";
#endif

    m_handleID = 0;

    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &DNetworkManager::replyFinished_slot);

#ifdef Q_OS_WIN
    QString appPath = QCoreApplication::applicationDirPath().remove(QRegExp("_d$"));
    m_downloadPath.setPath(appPath);
    if (!m_downloadPath.exists("download")) {
        m_downloadPath.mkdir("download");
    }
    m_downloadPath.cd("download");
#elif defined (Q_OS_MAC)
    QString download = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    m_downloadPath.setPath(download);
#elif defined (Q_OS_ANDROID)
    QString download = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    m_downloadPath.setPath(download);
#endif
}

DNetworkManager::~DNetworkManager()
{
   qDebug() << "DNetwork destroyed.";
}

void DNetworkManager::showIPAddress()
{
    QString userName = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    userName = userName.section("/", -1);

   qDebug().noquote() << QString("Hello %1!").arg(userName);
   qDebug() << "Network base-url:" << BaseUrl;

    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces())
    {
        if (!interface.isValid()) {
            continue;
        }

        QNetworkInterface::InterfaceFlags flags = interface.flags();
        if (flags.testFlag(QNetworkInterface::IsRunning)
                && !flags.testFlag(QNetworkInterface::IsLoopBack))
        {
            if (interface.humanReadableName().contains("VMware")
                    || interface.humanReadableName().contains("Loopback")
                    || interface.humanReadableName().contains("VirtualBox"))
            {
                continue;
            }

            QList<QNetworkAddressEntry> entryList = interface.addressEntries();
            foreach (QNetworkAddressEntry entry, entryList)
            {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    qDebug().noquote() << QString("your IP address: %1").arg(entry.ip().toString());
                }
            }
        }
    }
}

void DNetworkManager::setDownloadFilePath(QString dir)
{
    QDir d(dir);
    if (d.exists()) {
        m_downloadPath = d;
    }
}

QString DNetworkManager::getDownloadPath()
{
    return m_downloadPath.path();
}

void DNetworkManager::downloadFile(quint64 id, QJsonObject obj, QString user)
{
//    qDebug() << __FUNCTION__ << id << obj;

    QString url = obj.value("url").toString();
    QString fileName = obj.value("fileName").toString();
    if (url.isEmpty()) {
       qDebug() << "url is empty.";
        return;
    }
    QNetworkReply *reply = downloadFile(url, fileName, id);
    if (user.isEmpty()) {
        reply->setProperty("user", "client");
    } else {
        reply->setProperty("user", user);
    }
}

QNetworkReply *DNetworkManager::downloadFile(QString urlstr, QString fileName, quint64 id)
{
    QFile *file = new QFile();
    if (fileName.isEmpty()) {
        QFileInfo info(urlstr);
        fileName = info.fileName();
    }
    file->setFileName(m_downloadPath.absoluteFilePath(fileName));

    if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        delete file;
       qDebug() << "download file open fail.";
        return nullptr;
    }

    QNetworkReply *reply = m_manager->get(QNetworkRequest(QUrl(urlstr)));
    reply->setProperty("type", "downloadFile");
    reply->setProperty("user", "main");
    connect(reply, &QNetworkReply::downloadProgress,
            this, &DNetworkManager::downloadProgress_signal);
    file->setParent(reply);
    reply->setProperty("requestId", id);

    m_downloadReplyMap.insert(reply, file);
    return reply;
}

QNetworkReply *DNetworkManager::getDobotRequest(QString urlstr, QString user, int timeoutMs)
{
    QUrl url(BaseUrl + urlstr);
    QNetworkReply *reply = getRequest(url, timeoutMs);
    reply->setProperty("user", user);
    return reply;
}

//![1] get request(1)
QNetworkReply * DNetworkManager::getRequest(QUrl url, int timeoutMs)
{
   qDebug() << "[DNetworkManager] send GET Request:" << url.toString();

    QNetworkReply *reply = m_manager->get(QNetworkRequest(url));

    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, &DNetworkManager::onReplyError_slot);
#ifndef QT_NO_SSL
    connect(reply, &QNetworkReply::sslErrors, this, &DNetworkManager::onSslErrors_slot);
#endif
    setTimeoutRequest(reply, timeoutMs);
    return reply;
}

//![2] get request(2)
QNetworkReply * DNetworkManager::getRequest(QUrl url, QJsonObject queryObj, int timeoutMs)
{
   qDebug() << "[DNetworkManager] send GET Request:" << url.toString() << queryObj;

    QUrlQuery query;
    foreach (const auto &key, queryObj.keys()) {
        query.addQueryItem(key, queryObj.value(key).toString());
    }
    url.setQuery(query);

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(url);

    QNetworkReply *reply = m_manager->get(request);

    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, &DNetworkManager::onReplyError_slot);
#ifndef QT_NO_SSL
    connect(reply, &QNetworkReply::sslErrors, this, &DNetworkManager::onSslErrors_slot);
#endif
    setTimeoutRequest(reply, timeoutMs);
    return reply;
}

//![3] post request
QNetworkReply * DNetworkManager::postRequest(QString url, QJsonObject queryObj, int timeoutMs)
{
   qDebug() << "[DNetworkManager] send POST Request:" << url << queryObj;

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setUrl(QUrl(BaseUrl + url));

    QJsonDocument doc(queryObj);

    QNetworkReply *reply = m_manager->post(request, doc.toJson());
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this, &DNetworkManager::onReplyError_slot);
#ifndef QT_NO_SSL
    connect(reply, &QNetworkReply::sslErrors, this, &DNetworkManager::onSslErrors_slot);
#endif
    setTimeoutRequest(reply, timeoutMs);
    return reply;
}

//![4] set timeout
void DNetworkManager::setTimeoutRequest(QNetworkReply *reply, int msec)
{
    if (msec == 0) {
        return;
    }
    QTimer *timer = new QTimer(reply);
    timer->setObjectName(reply->url().toString() + " timer");
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [=](){
       qDebug() << "[DNetworkManager] request timeout, url:" << reply->url();
        reply->abort();
    });
    timer->start(msec);
}

//![5] finish Slot
void DNetworkManager::replyFinished_slot(QNetworkReply *reply)
{
    QString urlStr = reply->url().toString();
   qDebug() << "[DNetworkManager Reply]" << urlStr;

    if (m_downloadReplyMap.contains(reply)) {
        QFile *file = m_downloadReplyMap.take(reply);
        file->write(reply->readAll());
        file->close();
        quint64 id = static_cast<quint64>(reply->property("requestId").toDouble());
       qDebug() << "download finished. request id:" << id;

        emit downloadFinished_signal(urlStr, id);
    } else {
        if (urlStr.startsWith(BaseUrl)) {
            urlStr = urlStr.remove(BaseUrl);
        }

        if (urlStr.endsWith("DobotLink/version.json")) {
            QJsonObject resObj = parseJsonData(reply->readAll());
            emit onReplyMassage_signal(urlStr, resObj, reply->property("user").toString());
        } else {
            emit onReplyData_signal(urlStr, reply->readAll());
        }
    }

    reply->deleteLater();
}

QJsonObject DNetworkManager::parseJsonData(QByteArray byteArray)
{
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(byteArray, &jsonError);
    QJsonObject resObject;

    if (jsonError.error == QJsonParseError::NoError) {
        if (document.isObject()) {
            resObject = document.object();
        }
    } else {
       qDebug() << "[DNetworkManager] handle ReplyData error: QJsonParseError";
    }

    return resObject;
}

//![6] reply Slot
void DNetworkManager::onReplyError_slot(QNetworkReply::NetworkError code)
{
   qDebug() << "[DNetworkManager] error:" << code;
}

#ifndef QT_NO_SSL
void DNetworkManager::onSslErrors_slot(const QList<QSslError> &errors)
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    qDebug()<< "[DNetworkManager] error, url:" << reply->url() << "  ssl error:" << errors;
}
#endif
