#include "CurlNetworkReply.h"

#include <QDebug>

#include "curl/curl.h"

static char errorBuffer[1024];
static size_t write_callback(char *data, size_t size, size_t nmemb, std::string *writeData);
static size_t write_file(void *ptr, size_t size, size_t nmemb, void *stream);
int my_progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);

CurlNetworkReply::CurlNetworkReply(RequestType type, const CurlNetworkRequest &request, QObject *parent)
    : QObject(parent)
    , m_type(type)
    , m_request(request)
{
    this->setAutoDelete(false);
}

CurlNetworkReply::~CurlNetworkReply()
{

}

void CurlNetworkReply::run()
{
    CURL *curl = curl_easy_init();
    CURLcode res = CURLE_OK;

    Q_ASSERT_X(curl, __FUNCTION__, "curl init error");

    //![error buffer]
    res = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);

    //![settings]
    /* Switch on full protocol/debug output while testing */
    res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    res = curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    res = curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    res = curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, NULL);
    res = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

    //![SSL Options]
    res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    //![Get / Post]
    if (m_type == GET) {
        res = curl_easy_setopt(curl, CURLOPT_POST, 0L);
    } else if (m_type == POST) {
        res = curl_easy_setopt(curl, CURLOPT_POST, 1L);
    } else if (m_type == DOWNLOAD) {    // TODO
        /* resume TODO */
        curl_easy_setopt(curl, CURLOPT_RANGE, "..-");

        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, my_progress_callback);

        /* send all data to this function  */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file);

        /* write the page body to this file handle */
        FILE *_file = fopen("address...", "ab");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, _file);
    }

    //![Timeout]
    int timeout = m_request.timeout();
    if (timeout > 0) {
        res = curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)timeout);
    } else {
        res = curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);    //2s
    }

    //![Url]
    QString url_str = m_request.url().toString();
    m_url = url_str;
    QByteArray url_ba = url_str.toLatin1();
    curl_easy_setopt(curl, CURLOPT_URL, url_ba.data());

    //![Header]
    QList<QByteArray> headerList = m_request.rawHeaderList();
    if (!headerList.isEmpty()) {
        struct curl_slist *headers = NULL;
        foreach (const QByteArray &header, headerList){
            headers = curl_slist_append(headers, header.data());
        }

        res =  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    //![Callback]
    res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_receiveStr);


    //![Body Data]
//    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"ratio\":50}");
    QByteArray body = m_request.bodyData();
    if (!body.isEmpty()) {
        res = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.data());
    }

    //![Send Request]
    qDebug() << "#[Request](origin) id:" << m_request.id() << "data:" << url_ba << body;
    res = curl_easy_perform(curl);

    //![Result Handle]
    if (res == CURLE_OK) {
        long code = 0;
        res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

        QByteArray resData = QByteArray::fromStdString(m_receiveStr);

//        qDebug() << "#[REPLY](origin) id:" << m_request.id() << "data:" << code << resData;
        emit finished_signal(m_request.id(), resData);
    } else {
        printRes(res);
//        qDebug() << "#[REPLY]: ERROR:" << res << errorBuffer;
        emit onErrorOccured_signal(m_request.id(), res);
    }

    curl_easy_cleanup(curl);
}

QString CurlNetworkReply::urlstr()
{
    return m_url;
}

void CurlNetworkReply::printRes(int res)
{
    switch (res) {
    case CURLE_UNSUPPORTED_PROTOCOL:
        qDebug() << "不支持的协议, 由URL的头部指定";
        break;
    case CURLE_COULDNT_CONNECT:
        qDebug() << "不能连接到 remote 主机或者代理";
        break;
    case CURLE_HTTP_RETURNED_ERROR:
        qDebug() << "http返回错误";
        break;
    case CURLE_READ_ERROR:
        qDebug() << "读本地文件错误";
        break;
    default:
        qDebug() << "返回值:" << res;
        break;
    }
}

static size_t write_callback(char *data, size_t size, size_t nmemb, std::string *writeData)
{
    if (writeData == NULL) {
        return 0;
    }

    qulonglong sizes = size * nmemb;
    writeData->append(data, sizes);
    return sizes;
}

static size_t write_file(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

int my_progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
    if (clientp != NULL) {
        //...
    }
    double totalKb = dltotal / 1024;
    double nowKb = dlnow / 1024;
    qDebug() << "download progress"
             << (QString::number(totalKb,'f',3) + "kb")
             << (QString::number(nowKb,'f',3) + "kb");
    qDebug() << ultotal << ulnow;
    return 0;
}

