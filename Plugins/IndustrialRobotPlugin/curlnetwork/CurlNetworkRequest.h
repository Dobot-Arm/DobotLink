#ifndef CURLNETWORKREQUEST_H
#define CURLNETWORKREQUEST_H

#include <QUrl>
#include <QMap>

class CurlNetworkRequest
{
public:
    CurlNetworkRequest(quint64 id = 0);

    bool setUrl(const QUrl &url);
    bool setUrl(const QString &url_s);
    QUrl url() const;

    bool hasRawHeader(const QByteArray &key) const;
    void setRawHeader(const QByteArray &key, const QByteArray &value);
    QByteArray rawHeaderValue(const QByteArray &key) const;
    QList<QByteArray> rawHeaderList() const;

    void setBodyData(const QByteArray &data);
    QByteArray bodyData() const;

    void setTimeout(int timeout);
    int timeout() const;

    quint64 id();

private:
    quint64 m_id;
    QUrl m_url;
    int m_timeout;
    QByteArray m_bodydata;

    QMap<QByteArray, QByteArray> m_rawheaders;
};

#endif // CURLNETWORKREQUEST_H
