#include "CurlNetworkRequest.h"

#include <QDebug>

CurlNetworkRequest::CurlNetworkRequest(quint64 id) : m_id(id)
{
    m_timeout = 0;
}

bool CurlNetworkRequest::setUrl(const QUrl &url)
{
    if (url == m_url) {
        return true;
    } else if (url.isValid()) {
        m_url = url;
        return true;
    }
    return false;
}

bool CurlNetworkRequest::setUrl(const QString &url_s)
{
    QUrl url(url_s);
    if (url == m_url) {
        return true;
    } else if (url.isValid()) {
        m_url = url;
        return true;
    }
    return false;
}

QUrl CurlNetworkRequest::url() const
{
    return m_url;
}

bool CurlNetworkRequest::hasRawHeader(const QByteArray &key) const
{
    return m_rawheaders.contains(key);
}

void CurlNetworkRequest::setRawHeader(const QByteArray &key, const QByteArray &value)
{
    m_rawheaders.insert(key, value);
}

QByteArray CurlNetworkRequest::rawHeaderValue(const QByteArray &key) const
{
    return m_rawheaders.value(key);
}

QList<QByteArray> CurlNetworkRequest::rawHeaderList() const
{
    QList<QByteArray> headers;
    foreach (const QByteArray &key, m_rawheaders.keys()){
        QByteArray value = m_rawheaders.value(key);
        if (!value.isEmpty()) {
            headers.append(key + ":" + value);
        }
    }
    return headers;
}

void CurlNetworkRequest::setBodyData(const QByteArray &data)
{
    m_bodydata = data;
}

QByteArray CurlNetworkRequest::bodyData() const
{
    return m_bodydata;
}

void CurlNetworkRequest::setTimeout(int timeout)
{
    m_timeout = timeout;
}

int CurlNetworkRequest::timeout() const
{
    return m_timeout;
}

quint64 CurlNetworkRequest::id()
{
    return m_id;
}
