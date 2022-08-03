#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>

//#ifndef __arm__
//#define USE_CURL_LIB
//#endif

#ifdef USE_CURL_LIB
class CurlNetworkManager;
#else
class QNetworkAccessManager;
#include <QNetworkReply>
#endif

#include "FileControll.h"
#include "FileControllSmb.h"

class QThreadPool;

enum SearchType {
    SambaFile,
    HttpApi
};

enum SearchState {
    Searching,
    Success,
    Failed
};

enum SMBType
{
    SMBT_UNKNOW,
    SMBT_FILE_SMB,
    SMBT_V2_SMB
};

struct SearchResult
{
    SearchResult():
        version(0),
        type(""),
        state(SearchState::Searching),
        smbType(SMBType::SMBT_UNKNOW)
    {}
    int version;
    QString type;
    SearchState state;
    SMBType smbType;
};

class Module : public QObject
{
    Q_OBJECT
public:
    explicit Module(QString basePort, QObject *parent = nullptr);

    const QString BasePort;

    bool setIpAddress(QString ip);
    QString ip();
    QString getIpAddress();
    void SetSearchIP(const QStringList& strIP);

    /* get post */
    void sendGetRequest(const QString &url, const quint64 id = 0, const QString api = QString(),
                        const std::function<void(const QJsonValue&, QString strUrl)>& successCallback = nullptr,
                        const std::function<void(int iErrCode, QString strErrMsg)>& errorCallback = nullptr);
    void sendPostRequest(const QString &url, const QJsonValue &value, const quint64 id = 0, const QString api = QString(),
                         const std::function<void(const QJsonValue&, QString strUrl)>& successCallback = nullptr,
                         const std::function<void(int iErrCode, QString strErrMsg)>& errorCallback = nullptr);
    void sendSearchRequest(const QString &url, const quint64 id = 0);

    bool containsIPaddress(const QString &address);
    QJsonValue parseJsonData(QByteArray byteArray);

signals:
    void onReceiveData_signal(QJsonValue value, QString url, quint64 id, QString api);
    void onErrorOccured_signal(quint64 id, int errCode, QString errStr = "");
    void onSearch_signal(QJsonArray res, quint64 id);
    void onIOErrorOccured_signal(quint64 id, int code);
    void onGetConnectionStateOccuredError_signal(quint64 id);

private:
    inline int _checkSearchFinish(QSharedPointer<QMap<QPair<QString, SearchType>, QSharedPointer<SearchResult>>> searchTypeRes, quint64 id);
    inline bool _handleSearchResult(QSharedPointer<QMap<QPair<QString, SearchType>,  QSharedPointer<SearchResult>>> searchTypeRes, const QString ip, const SearchType searchType, const QString &head, const QByteArray &receiveData);
#ifdef USE_CURL_LIB
    CurlNetworkManager *m_crulManager;
#else
    QNetworkAccessManager *m_manager;
#endif

    QString m_ip;
    QStringList m_newSearchIp;
    int m_iHttpRequestTryTimes; //http请求响应超时时重复的次数

#ifdef USE_CURL_LIB
protected slots:
    void receiveCurlData_slot(const quint64 id, const QByteArray message);
    void receiveCurlError_slot(const quint64 id, const int code);
#else
protected:
    void receiveData_slot(QNetworkReply *reply,const std::function<void(const QJsonValue&, QString strUrl)>& successCallback = nullptr,
                          const std::function<void(int iErrCode, QString strErrMsg)>& errorCallback = nullptr);
    void receiveError_slot(QNetworkReply::NetworkError err,QNetworkReply *reply,
                           const std::function<void(int iErrCode, QString strErrMsg)>& errorCallback = nullptr);
#endif

};

#endif // MODULE_H

