#include "CurlNetworkManager.h"

#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include "curl/curl.h"

CurlNetworkManager::CurlNetworkManager(QObject *parent) : QObject(parent)
{
    m_pool.setMaxThreadCount(10);

    curl_global_init(CURL_GLOBAL_ALL);
}

CurlNetworkManager::~CurlNetworkManager()
{
    curl_global_cleanup();
}

CurlNetworkReply *CurlNetworkManager::getJson(QUrl url, quint64 id, int timeout)
{
    CurlNetworkRequest request(id);
    request.setUrl(url);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Accept", "*/*");
    request.setTimeout(timeout);

    CurlNetworkReply *reply = new CurlNetworkReply(GET, request);

//    qDebug() << "#[GET]:" << url.toString();

    m_pool.start(reply);
    // 当次请求完后，timeout恢复默认值
    request.setTimeout(0);
    return reply;
}

CurlNetworkReply *CurlNetworkManager::postJson(QUrl url, const QJsonValue &value, quint64 id, int timeout)
{
    CurlNetworkRequest request(id);
    request.setUrl(url);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Accept", "*/*");
    request.setTimeout(timeout);

    if (!value.isNull() and !value.isUndefined()) {

        // 设置超时时间
        QJsonObject obj = value.toObject();
        int timeout = obj.value("timeout").toInt();
        qDebug() << __FUNCTION__ << "timeout: " << timeout;
        request.setTimeout(timeout);

        QJsonDocument doc;
        if (value.isArray()) {
            doc.setArray(value.toArray());
        } else {
            doc.setObject(value.toObject());
        }

        QString doc_str = doc.toJson();
        //qDebug() << "#[POST]:" << url.toString() << doc_str;
        QByteArray doc_ba = doc_str.simplified().toLatin1();
        request.setBodyData(doc_ba);
    }

    CurlNetworkReply *reply = new CurlNetworkReply(POST, request);

//    qDebug() << "#[POST]:" << url.toString() << doc_ba;

    m_pool.start(reply);
    // 当次请求完后，timeout恢复默认值
    request.setTimeout(0);
    return reply;
}
