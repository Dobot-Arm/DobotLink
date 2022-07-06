#include "Module.h"
#include "DError/DError.h"
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <QEventLoop>
#include <QProcess>
#include <QFuture>
#include <QtConcurrent>
#ifdef USE_CURL_LIB
#include "curlnetwork/CurlNetworkManager.h"
#else
#include <QNetworkAccessManager>
#include <QTimer>
#endif

#ifndef __arm__
const QString MG400_DEFAULT_IP = "192.168.1.6";
#else
const QString MG400_DEFAULT_IP = "127.0.0.1";
#endif
const static QString MG400_WIRED_IP = "192.168.5.1";//有线连接的ip
const QString WifiDefaultIPAddress = "192.168.9.1";
const QString BASE_PORT = "22000";
const int SearchDefaultTimeout = 1; // s
const int CommDefaultTimeout = 2; //s

Module::Module(QString basePort, QObject *parent) : QObject(parent), BasePort(basePort)
{
#ifdef USE_CURL_LIB
    m_crulManager = new CurlNetworkManager(this);
#else
    m_manager = new QNetworkAccessManager(this);
#endif
    m_pThreadPool = new QThreadPool(this);

    m_iHttpRequestTryTimes = 0;
}

QString Module::getIpAddress()
{
    return m_ip;
}

bool Module::setIpAddress(QString ip)
{
    if (containsIPaddress(ip)) {
        qDebug() << "set target ip address:" << ip;
        m_ip = ip;
        return true;
    } else {
        qDebug() << "wrong ip address." << ip;
        return false;
    }
}

QString Module::ip()
{
    return m_ip;
}

void Module::SetSearchIP(const QString& strIP)
{
    m_strNewSearchIP = strIP;
}

int Module::_checkSearchFinish(QSharedPointer<QMap<QPair<QString, SearchType>,  QSharedPointer<SearchResult>>> searchTypeRes, quint64 id)
{
    QPair<QString, SearchType> key;
    foreach(key, searchTypeRes->keys()) {
        if (searchTypeRes->value(key)->state == SearchState::Searching) return 0;
    }

    QJsonArray searchDevices;
    QSet<QString> ipResultSet;//用于判断该ip结果是否已经包含searchDevices中
    ipResultSet.clear();

    foreach(key, searchTypeRes->keys()) {
        SearchResult *res = searchTypeRes->value(key).get();
        if (res->state != SearchState::Success) continue;

        if(ipResultSet.contains(key.first)){
            continue;
        }
        QJsonObject mgObj;
        mgObj.insert("portName", key.first);
        mgObj.insert("status", "unconnected");
        mgObj.insert("version", res->version);
        mgObj.insert("type", res->type);
        mgObj.insert("smbType", QJsonValue((int)(res->smbType)));
        searchDevices.append(mgObj);
        ipResultSet.insert(key.first);
    }

    emit onSearch_signal(searchDevices, id);

    return searchDevices.size();
}

bool Module::_handleSearchResult(QSharedPointer<QMap<QPair<QString, SearchType>,  QSharedPointer<SearchResult>>> searchTypeRes, const QString ip, const SearchType searchType, const QString &head, const QByteArray &receiveData)
{
    if (!head.contains("json")) {
        qWarning() << "Invalid Content Type Header:" << head;
        searchTypeRes->value(QPair<QString, SearchType>(ip, searchType))->state = SearchState::Failed;
    } else if (receiveData.size() == 0) {
        qWarning() << "Invalid Data:" << receiveData;
        searchTypeRes->value(QPair<QString, SearchType>(ip, searchType))->state = SearchState::Failed;
    } else {
        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson(receiveData, &jsonError);
        if (jsonError.error != QJsonParseError::NoError) {
            qWarning() << "QJsonParseError" << receiveData;
            searchTypeRes->value(QPair<QString, SearchType>(ip, searchType))->state = SearchState::Failed;
        } else {
            if (!document.isObject()) {
                qWarning() << "QJsonParseError" << receiveData;
                searchTypeRes->value(QPair<QString, SearchType>(ip, searchType))->state = SearchState::Failed;
            } else {
                QJsonObject json = document.object();
                qDebug() << QString("[%1] search success").arg(ip);
                searchTypeRes->value(QPair<QString, SearchType>(ip, searchType))->state = SearchState::Success;
                searchTypeRes->value(QPair<QString, SearchType>(ip, searchType))->version = json.value("version").toInt();
                searchTypeRes->value(QPair<QString, SearchType>(ip, searchType))->type = json.value("name").toString();
                return true;
            }
        }
    }
    return false;
}

void Module::sendSearchRequest(const QString &url, const quint64 id)
{
    //创建以ip为key搜索的结构体
    QSharedPointer<QMap<QString, QSharedPointer<SearchResult>>> searchRes(new QMap<QString, QSharedPointer<SearchResult>>());
    searchRes.get()->insert(MG400_DEFAULT_IP, QSharedPointer<SearchResult>(new SearchResult()));
    searchRes.get()->insert(MG400_WIRED_IP, QSharedPointer<SearchResult>(new SearchResult()));
#ifndef __arm__
    searchRes.get()->insert(WifiDefaultIPAddress, QSharedPointer<SearchResult>(new SearchResult()));
#endif
    if (!m_strNewSearchIP.isEmpty())
    {
        searchRes.get()->insert(m_strNewSearchIP, QSharedPointer<SearchResult>(new SearchResult()));
    }

    if (m_pThreadPool->maxThreadCount() != searchRes->size())
    {
        m_pThreadPool->setMaxThreadCount(searchRes->size());
    }

    //创建以<ip,SearchType>为key存储结果的结构体
    QSharedPointer<QMap<QPair<QString, SearchType>, QSharedPointer<SearchResult>>> searchTypeRes(new QMap<QPair<QString, SearchType>, QSharedPointer<SearchResult>>());
    foreach (QString ip, searchRes->keys()){
        searchTypeRes->insert(QPair<QString, SearchType>(ip, SambaFile), QSharedPointer<SearchResult>(new SearchResult()));
        searchTypeRes->insert(QPair<QString, SearchType>(ip, HttpApi), QSharedPointer<SearchResult>(new SearchResult()));
    }
    if (m_manager->networkAccessible() != QNetworkAccessManager::Accessible)
    {
        m_manager->setNetworkAccessible(QNetworkAccessManager::Accessible);
    }
    foreach(QString ip, searchRes->keys()) {
        //先做http访问
        /***这个接口一开始就有，既然这个接口能够http成功，那就说明http-server起来了。可以兼容老设备没有/properties/controllerType接口的问题导致http-request失败****/
        const QString strConnectionState = "/connection/state";
        QString _url = QString("http://%1:%2%3").arg(ip).arg(BasePort).arg(strConnectionState);
        qDebug().noquote() << QString("<< [GET] (id:%1) url:%2").arg(id).arg(_url);
        QNetworkRequest request;
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setUrl(QUrl(_url));
        QNetworkReply *reply = m_manager->get(request);

        QTimer* pCheckHttpTimer = new QTimer(reply);
        pCheckHttpTimer->setInterval(2000);
        pCheckHttpTimer->setSingleShot(true);
        pCheckHttpTimer->start();
        connect(pCheckHttpTimer, &QTimer::timeout, [this, searchTypeRes, ip, id, reply]() {
            qDebug() << QString("[%1](id:%2) search timeout").arg(ip).arg(id);
            searchTypeRes->value(QPair<QString, SearchType>(ip, HttpApi))->state = SearchState::Failed;
            searchTypeRes->value(QPair<QString, SearchType>(ip, SambaFile))->state = SearchState::Failed;

            reply->disconnect();
            reply->abort();
            reply->deleteLater();

            if (this->_checkSearchFinish(searchTypeRes, id) <=0 )
            {//当发生超时时，清理一次网络缓存，否则可能出现再也无法正常http请求
                QPair<QString, SearchType> key;
                foreach(key, searchTypeRes->keys())
                {
                    if (searchTypeRes->value(key)->state == SearchState::Searching) return ;
                }
                m_manager->clearAccessCache();
            }
        });

        connect(reply, &QNetworkReply::finished, this, [this, searchTypeRes, ip, id, reply, pCheckHttpTimer]() {
            pCheckHttpTimer->stop();

            QByteArray receiveData = reply->readAll();
            qDebug()<< QString("[%1](id:%2) SearchDobot http receiveData: [%3]").arg(ip).arg(id).arg(QString(receiveData));

            QString head = reply->header(QNetworkRequest::ContentTypeHeader).toString();
            this->_handleSearchResult(searchTypeRes, ip, HttpApi, head, receiveData);

            //再controllerType的samba v2文件访问
            QString strSmbFile = "/project/properties/controllerType.json";
            FileControll* pSmb = new FileControllSmb(ip, reply);
            connect(pSmb, &FileControll::onFinish_signal, this, [this,searchTypeRes,ip,strSmbFile,reply](quint64 id, int code, QByteArray array){
                if (NOERROR != code)
                {
                    //smb v1访问
                    FileControll* pSmbV1 = new FileControll(ip, reply) ;
                    connect(pSmbV1, &FileControll::onFinish_signal, this, [=](quint64 idT, int codeT, QByteArray arrayT){
                        if (NOERROR != codeT){
                            qDebug() << QString("(id:%1)[%2] SearchDobot Samba file don't exist.  %3 ").arg(idT).arg(ip).arg(strSmbFile);
                            searchTypeRes->value(QPair<QString, SearchType>(ip, SambaFile))->state = SearchState::Failed;
                            searchTypeRes->value(QPair<QString, SearchType>(ip, HttpApi))->state = SearchState::Failed;
                        }else{
                            qDebug() << QString("(id:%1) [%2] SearchDobot Samba receive controllerType.json Data: [%3]").arg(idT).arg(ip).arg(QString(arrayT));
                            QString head = "application/json";
                            if (this->_handleSearchResult(searchTypeRes, ip, SambaFile, head, arrayT))
                            {
                                SMBType smbType = SMBType::SMBT_FILE_SMB;
                                searchTypeRes->value(QPair<QString, SearchType>(ip, SambaFile))->smbType = smbType;
                                searchTypeRes->value(QPair<QString, SearchType>(ip, HttpApi))->smbType = smbType;
                            }
                        }
                        this->_checkSearchFinish(searchTypeRes, idT);
                        reply->deleteLater();
                    });
                    pSmbV1->readFile(id, strSmbFile, 2000);
                }
                else
                {
                    qDebug() << QString("(id:%1) [%2] SearchDobot SambaV2 receive controllerType.json Data: [%3]").arg(id).arg(ip).arg(QString(array));
                    QString head = "application/json";
                    if (this->_handleSearchResult(searchTypeRes, ip, SambaFile, head, array))
                    {
                        SMBType smbType = SMBType::SMBT_V2_SMB;
                        searchTypeRes->value(QPair<QString, SearchType>(ip, SambaFile))->smbType = smbType;
                        searchTypeRes->value(QPair<QString, SearchType>(ip, HttpApi))->smbType = smbType;
                    }
                    this->_checkSearchFinish(searchTypeRes, id);
                    reply->deleteLater();
                }
            });
            qDebug().noquote() << QString("<<SearchDobot Samba(id:%1) url:%2").arg(id).arg(strSmbFile);
            pSmb->readFile(id, strSmbFile, 2000);
        });
        connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
                [this, searchTypeRes, ip, id, reply](QNetworkReply::NetworkError err) {

            //错误信号发射出来后，finished信号也可能会被发送
            reply->disconnect();

            qDebug() << QString("[%1](id:%2) SearchDobot http search error: (%3)%4").arg(ip).arg(id).arg(err).arg(reply->errorString());

            searchTypeRes->value(QPair<QString, SearchType>(ip, SambaFile))->state = SearchState::Failed;
            searchTypeRes->value(QPair<QString, SearchType>(ip, HttpApi))->state = SearchState::Failed;

            this->_checkSearchFinish(searchTypeRes, id);

            reply->deleteLater();
        });
    }
}

void Module::sendGetRequest(const QString &url, const quint64 id, const QString api,
                            const std::function<void(const QJsonValue&, QString strUrl)>& successCallback/* = nullptr*/,
                            const std::function<void(int iErrCode, QString strErrMsg)>& errorCallback/* = nullptr*/)
{
    QString req_url = QString("http://%1:%2%3").arg(m_ip).arg(BasePort).arg(url);
    qDebug().noquote() << QString("<< [GET] (id:%1) url:%2").arg(id).arg(req_url);

#ifdef USE_CURL_LIB
    CurlNetworkReply *reply = m_crulManager->getJson(QUrl(req_url), id, CommDefaultTimeout);
    connect(reply, &CurlNetworkReply::finished_signal, this, &Module::receiveCurlData_slot);
    connect(reply, &CurlNetworkReply::onErrorOccured_signal, this, &Module::receiveCurlError_slot);
#else
    if (m_manager->networkAccessible() != QNetworkAccessManager::Accessible)
    {
        m_manager->setNetworkAccessible(QNetworkAccessManager::Accessible);
    }
    QNetworkReply *reply = m_manager->get(QNetworkRequest(req_url));
    QTimer *timer = new QTimer(reply);
    timer->setSingleShot(true);
    timer->setInterval(CommDefaultTimeout * 1000);
    timer->start();

    connect(timer, &QTimer::timeout, this, [=](){
        quint64 id = reply->property("id").toDouble();
        QString api = reply->property("api").toString();
        Q_UNUSED(api);

        if (reply->isRunning()) {
            reply->disconnect();
            reply->abort();

            /*reply->disconnect()后，receiveError_slot槽函数不再接收处理导致断开网络连接时无法判断设备已经失去连接。
             *正常情况下，GetConnectionState接口响应很快的，可以理解为设备连接的心跳包。
            */
            if ("GetConnectionState" == api || "GetDobotStatus" == api)
            {
                ++m_iHttpRequestTryTimes;
                if (m_iHttpRequestTryTimes >= 3)
                {
                    emit onGetConnectionStateOccuredError_signal(id);
                    m_iHttpRequestTryTimes = 0;
                }
            }

            int errCode = ERROR_HTTP_QT_TIMEOUT;
            QString errMsg = DError::getErrorMessage(errCode);
            qDebug() << __FUNCTION__ << errMsg;

            if (nullptr == errorCallback)
            {
                emit onErrorOccured_signal(id, errCode, errMsg);
            }
            else
            {
                errorCallback(errCode, errMsg);
            }

            reply->deleteLater();
        }
    });
    connect(reply, &QNetworkReply::finished, this, [this,reply,successCallback,errorCallback]{
        receiveData_slot(reply,successCallback,errorCallback);
    });
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, [this,reply,errorCallback](QNetworkReply::NetworkError err){
        receiveError_slot(err, reply,errorCallback);
    });
#endif

    if (reply) {
        reply->setProperty("id", id);
        if (!api.isEmpty()) {
            reply->setProperty("api", api);
        }
    }
}

void Module::sendPostRequest(const QString &url, const QJsonValue &value, const quint64 id, const QString api,
                             const std::function<void(const QJsonValue&, QString strUrl)>& successCallback/* = nullptr*/,
                             const std::function<void(int iErrCode, QString strErrMsg)>& errorCallback/* = nullptr*/)
{
    QString req_url = QString("http://%1:%2%3").arg(m_ip).arg(BasePort).arg(url);
    qDebug().noquote() << QString("<< [POST] (id:%1) url:%2").arg(id).arg(req_url) << value;

#ifdef USE_CURL_LIB
    CurlNetworkReply *reply = m_crulManager->postJson(QUrl(req_url), value, id, CommDefaultTimeout);
    connect(reply, &CurlNetworkReply::finished_signal, this, &Module::receiveCurlData_slot);
    connect(reply, &CurlNetworkReply::onErrorOccured_signal, this, &Module::receiveCurlError_slot);
#else
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(QUrl(req_url));

    QByteArray data_send;
    int realTimeout = CommDefaultTimeout * 1000;

    if (!value.isNull() and !value.isUndefined()) {
        QJsonDocument doc;
        if (value.isObject()) {
            QJsonObject jsonObj = value.toObject();
            if(jsonObj.contains("timeout")){
                realTimeout = jsonObj.value("timeout").toInt();
                jsonObj.remove("timeout");
            }
            doc.setObject(jsonObj);
        } else if (value.isArray()) {
            doc.setArray(value.toArray());
        }
        data_send = doc.toJson();

        request.setHeader(QNetworkRequest::ContentLengthHeader, data_send.size());
    }

    if (m_manager->networkAccessible() != QNetworkAccessManager::Accessible)
    {
        m_manager->setNetworkAccessible(QNetworkAccessManager::Accessible);
    }

    QNetworkReply *reply = m_manager->post(request, data_send);
    QTimer *timer = new QTimer(reply);
    timer->setSingleShot(true);
    timer->setInterval(realTimeout);
    timer->start();

    connect(timer, &QTimer::timeout, this, [=](){
        quint64 id = reply->property("id").toDouble();
        QString api = reply->property("api").toString();
        Q_UNUSED(api);

        if (reply->isRunning()) {
            reply->disconnect();
            reply->abort();

            int errCode = ERROR_HTTP_QT_TIMEOUT;
            QString errMsg = DError::getErrorMessage(errCode);
            qDebug() << __FUNCTION__ << errMsg;
            if (nullptr == errorCallback)
            {
                emit onErrorOccured_signal(id, errCode, errMsg);
            }
            else
            {
                errorCallback(errCode, errMsg);
            }

            reply->deleteLater();
        }
    });
    connect(reply, &QNetworkReply::finished, this, [this,reply,successCallback,errorCallback]{
        receiveData_slot(reply,successCallback,errorCallback);
    });
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, [this,reply,errorCallback](QNetworkReply::NetworkError err){
        receiveError_slot(err, reply,errorCallback);
    });
#endif

    if (reply) {
        reply->setProperty("id", id);
        if (!api.isEmpty()) {
            reply->setProperty("api", api);
        }
    }
}

bool Module::containsIPaddress(const QString &address)
{
    QRegExp rx("((25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)\\.){3}(25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)");
    if (rx.indexIn(address) > -1) {
        return true;
    }
    return false;
}

QJsonValue Module::parseJsonData(QByteArray byteArray)
{
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(byteArray, &jsonError);

    if (jsonError.error == QJsonParseError::NoError) {
        if (document.isObject()) {
            return document.object();
        } else if (document.isArray()) {
            return document.array();
        }
    } else {
        qDebug() << "error: QJsonParseError";
    }

    return QJsonValue();
}

#ifdef USE_CURL_LIB
void Module::receiveCurlData_slot(const quint64 id, const QByteArray message)
{
    auto reply = qobject_cast<CurlNetworkReply *>(sender());
    if (reply == nullptr) {
        qDebug() << "There is an error with your operation.";
        return;
    }

    QString api = reply->property("api").toString();

    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(message, &jsonError);

    if (jsonError.error == QJsonParseError::NoError) {
        if (document.isObject()) {
            QJsonObject obj = document.object();

            qDebug().noquote() << QString(">> [REPLY] (id:%1)").arg(id) << obj;
            emit onReceiveData_signal(obj, reply->urlstr(), id, api);
        } else {
            QJsonArray arr = document.array();
            qDebug().noquote() << QString(">> [REPLY] (id:%1)").arg(id) << arr << "todo todo todo";
        }
    } else {
        qDebug() << "error: QJsonParseError" << message;
        emit onErrorOccured_signal(id, ERROR_INDUSTRY_JSONOBJECT);
    }

    reply->deleteLater();
}

void Module::receiveCurlError_slot(const quint64 id, const int code) {
    auto reply = qobject_cast<CurlNetworkReply *>(sender());
    if (reply == nullptr) {
        qDebug() << "There is an error with your operation.";
        return;
    }

    qDebug() << "error code:" << code;
    int err_code = code + ERROR_HTTP_CURL;
    emit onErrorOccured_signal(id, err_code);

    reply->deleteLater();
}
#else
/* [Receive] */
void Module::receiveData_slot(QNetworkReply *reply,const std::function<void(const QJsonValue&, QString strUrl)>& successCallback,
                              const std::function<void(int iErrCode, QString strErrMsg)>& errorCallback)
{
    QByteArray receiveData = reply->readAll();
    QString head = reply->header(QNetworkRequest::ContentTypeHeader).toString();
    QString url = reply->url().toString();
    quint64 id = reply->property("id").toDouble();
    QString api = reply->property("api").toString();

    reply->deleteLater();

    if (!head.contains("json")) {
        qWarning() << "Invalid Content Type Header:" << head;
        qWarning() << "Invalid Data:" << receiveData;
        return;
    }

    if ("GetConnectionState" == api || "GetDobotStatus" == api)
    {//对应的接口请求成功，则复位
        m_iHttpRequestTryTimes = 0;
    }

    //后台请求成功了，但是有时候接口返回的数据是空的，为了兼容，添加一个补救的方法
    if (reply->operation() != QNetworkAccessManager::GetOperation)
    {
        if (receiveData.size() == 0)
        {
            qDebug().noquote()<<QString(">>%1(id:%2)%3:has no response data!!").arg(api).arg(id).arg(url);
            receiveData = "{\"status\":true}";
        }
    }

    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(receiveData, &jsonError);

    if (jsonError.error == QJsonParseError::NoError) {
        QJsonValue resValue;
        if (document.isObject()) {
            resValue = document.object();
        } else if (document.isArray()) {
            resValue = document.array();
        }

        qDebug().noquote() << QString(">> [REPLY] (id:%1)").arg(id) << resValue;

        if (!resValue.isNull() && !resValue.isUndefined()) {
            if (nullptr == successCallback)
            {
                emit onReceiveData_signal(resValue, url, id, api);
            }
            else
            {
                successCallback(resValue,url);
            }
        }
    } else {
        qDebug().noquote() << QString(">> [REPLY] (id:%1)error,QJsonParseError:").arg(id) << receiveData;
        if ("SetDebuggerStepIn" == api || "SetDebuggerSetb" == api || "SetDebuggerStepOver" == api)
        {//这几个接口，只要有数据返回，无论内容是什么都认为成功
            QJsonObject jo;
            jo.insert("status",true);
            if (nullptr == successCallback)
            {
                emit onReceiveData_signal(jo, url, id, api);
            }
            else
            {
                successCallback(jo,url);
            }
        }
        else
        {
            if (nullptr == errorCallback)
            {
                emit onErrorOccured_signal(id, ERROR_INDUSTRY_JSONOBJECT);
            }
            else
            {
                errorCallback(ERROR_INDUSTRY_JSONOBJECT,QString(""));
            }
        }
    }
}

void Module::receiveError_slot(QNetworkReply::NetworkError err,QNetworkReply *reply,
                               const std::function<void(int iErrCode, QString strErrMsg)>& errorCallback)
{
    quint64 id = reply->property("id").toDouble();
    QString api = reply->property("api").toString();
    Q_UNUSED(api);

    //fix by lf:当请求发生错误时，比如断网了，就需要通知与设备连接断开了，解决断线后重新SearchDobot时返回的状态为伪connected的bug。
    if ("GetConnectionState" == api || "GetDobotStatus" == api)
    {
        ++m_iHttpRequestTryTimes;
        if (m_iHttpRequestTryTimes >= 3)
        {
            emit onGetConnectionStateOccuredError_signal(id);
            m_iHttpRequestTryTimes = 0;
        }
    }

    qDebug() << __FUNCTION__ << err << reply->errorString();
    int errCode = err + ERROR_HTTP_QT;
    if (nullptr == errorCallback)
    {
        emit onErrorOccured_signal(id, errCode, reply->errorString());
    }
    else
    {
        errorCallback(ERROR_INDUSTRY_JSONOBJECT,reply->errorString());
    }

    reply->deleteLater();
}
#endif


