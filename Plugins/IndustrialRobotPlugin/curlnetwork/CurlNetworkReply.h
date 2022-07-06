#ifndef CURLNETWORKREPLY_H
#define CURLNETWORKREPLY_H

#include <QObject>
#include <QRunnable>

#include "CurlNetworkRequest.h"

enum RequestType {GET, POST, PUT, DOWNLOAD, UPLOAD};

class CurlNetworkReply : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit CurlNetworkReply(RequestType type,
                              const CurlNetworkRequest &request,
                              QObject *parent = nullptr);
    ~CurlNetworkReply();

    void run() override;

    QString urlstr();

signals:
    void finished_signal(quint64 id, QByteArray message);
    void onErrorOccured_signal(quint64 id, int code);

private:
    RequestType m_type;
    CurlNetworkRequest m_request;
    QString m_url;

    std::string m_receiveStr;

    void printRes(int res);
};

#endif // CURLNETWORKREPLY_H
