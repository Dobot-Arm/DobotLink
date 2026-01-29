#include "DWebSocketServer.h"

#include <QWebSocket>
#include <QWebSocketServer>
#include <QJsonDocument>
#include <QDebug>

QAtomicInteger<qint64> DWebSocketServer::m_seqId;

DWebSocketServer *DWebSocketServer::getInstance()
{
    static DWebSocketServer *instance = nullptr;
    if (instance == nullptr) {
        instance = new DWebSocketServer();
    }
    return instance;
}

DWebSocketServer::DWebSocketServer(QObject *parent) : QObject(parent)
{
    m_WebSocketServer = new QWebSocketServer("DobotLinkServer", QWebSocketServer::NonSecureMode, this);
    m_WebSocketServer->setProxy(QNetworkProxy::NoProxy);
    connect(m_WebSocketServer, &QWebSocketServer::newConnection, this, &DWebSocketServer::onNewConnection_slot);
    connect(m_WebSocketServer, &QWebSocketServer::closed, this, &DWebSocketServer::onWebSocketServerClosed_slot);

    m_pTimer = new QTimer(this);
    m_pTimer->setSingleShot(false);
    m_pTimer->setInterval(1000);
    connect(m_pTimer, &QTimer::timeout, this, [this]{
        qDebug() << "DobotLink WebSocketServer(9090) start listening...." << endl;
        bool ok = m_WebSocketServer->listen(QHostAddress::AnyIPv4, 9090);
        if (ok == false) {
            qDebug() << "DobotLink WebSocketServer listen error." << m_WebSocketServer->errorString();
        } else {
            qDebug() << "DobotLink WebSocketServer PORT: [9090], enjoy it." << endl;
            m_pTimer->stop();
        }
    });
    m_pTimer->start();
}

//![1] Send message
/* 发送消息 */
void DWebSocketServer::sendMessage(quint16 port, QString message)
{
    if(port == 0)
        port = CLIENT_PORT;

    QWebSocket *pClient = nullptr;
    qint64 srcId = 0;

    qint64 innerId = GetJsonrpcId(message);
    auto itr = m_mapClient.find(innerId);
    if (itr != m_mapClient.end())
    {
        pClient = itr.value().second;
        srcId = itr.value().first;
        m_mapClient.erase(itr);
    }

    if (pClient && pClient->isValid()) {
        ExchangeJsonrpcId(message, innerId, srcId);
        pClient->sendTextMessage(message);
        qDebug().noquote() << QString("<<%1:innerId:%2[send]").arg(port).arg(innerId) << message;
    }
    else if (m_allClient.size()>0){
        //这种情况可能是各个插件向外发送通知消息，没有携带id，也不知道发给谁，所以只能群发
        //但实际上，这些websocket的客户端基本都是在本地电脑上，也就是说ws的客户端ip都一样，那就根据端口区分也可以的
        bool bFind = false;
        foreach(QWebSocket* pConn , m_allClient)
        {
            if (pConn->isValid() && port == pConn->peerPort())
            {
                bFind = true;

                RevertOriginJsonrpc(message);
                pConn->sendTextMessage(message);
                break;
            }
        }
        if (!bFind)
        {
            qDebug() << "[WebSocket-Server]: client is not found. port:" << port << message;
        }
    }
    else {
        qDebug() << "[WebSocket-Server]: client is not valid. port:" << port << message;
    }
}

void DWebSocketServer::sendMessageObj(QJsonObject obj)
{
    quint16 port = CLIENT_PORT;
    if (obj.contains("WSport")) {
        port = static_cast<quint16>(obj.value("WSport").toInt(0));
        obj.remove("WSport");
    }
    QJsonDocument document;
    document.setObject(obj);

    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    this->sendMessage(port, QString(byteArray));
}

void DWebSocketServer::closeServer()
{
    m_WebSocketServer->close();
    m_mapClient.clear();
    for (auto itr = m_allClient.begin(); itr != m_allClient.end(); ++itr)
    {
        (*itr)->deleteLater();
    }
    m_allClient.clear();
}

//![2] WebSocketServer
void DWebSocketServer::onNewConnection_slot()
{
    QWebSocket *pClient = m_WebSocketServer->nextPendingConnection();

    if (pClient) {
        CLIENT_PORT = pClient->peerPort();
        qDebug() << "DWebSocket: New Connection. address:"
                 << pClient->peerAddress().toString()
                 << "name:" << pClient->peerName()
                 << "port:" << pClient->peerPort();

        connect(pClient, &QWebSocket::textMessageReceived,
                this, &DWebSocketServer::onClientMessageReceived_slot);
        connect(pClient, &QWebSocket::disconnected, this,
                &DWebSocketServer::onClientDisconnection_slot);

        //m_clientMap.insert(pClient->peerPort(), pClient);
        m_allClient.insert(pClient);
    }
}

void DWebSocketServer::onWebSocketServerClosed_slot()
{
    qDebug() << "[WebSocket-Server] closed.";
}

bool DWebSocketServer::ExchangeJsonrpcId(QString& strMsg, qint64& srcId, qint64 replaceId)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(strMsg.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError)
    {
        return false;
    }
    if (!doc.isObject())
    {
        return false;
    }
    QJsonObject obj = doc.object();

    srcId = static_cast<qint64>(obj.value("id").toDouble());
    obj.insert("id", replaceId);

    doc.setObject(obj);
    strMsg = doc.toJson(QJsonDocument::Compact);

    return true;
}

qint64 DWebSocketServer::GetJsonrpcId(QString strMsg)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(strMsg.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError)
    {
        return 0;
    }
    if (!doc.isObject())
    {
        return 0;
    }
    QJsonObject obj = doc.object();
    if (obj.contains("id"))
    {
        return static_cast<qint64>(obj.value("id").toDouble());
    }
    return 0;
}

void DWebSocketServer::InsertOriginJsonrpcId(QString &strJsonRpc, qint64 originRequestId)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(strJsonRpc.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError)
    {
        return ;
    }
    if (!doc.isObject())
    {
        return ;
    }
    QJsonObject obj = doc.object();
    obj.insert("originRequestId", originRequestId);
    doc.setObject(obj);
    strJsonRpc = doc.toJson(QJsonDocument::Compact);
}

void DWebSocketServer::RevertOriginJsonrpc(QString &strJsonRpc)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(strJsonRpc.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError)
    {
        return ;
    }
    if (!doc.isObject())
    {
        return ;
    }
    QJsonObject obj = doc.object();
    if (obj.contains("originRequestId"))
    {
        obj.insert("id", obj.value("originRequestId"));
        obj.remove("originRequestId");
    }
    doc.setObject(obj);
    strJsonRpc = doc.toJson(QJsonDocument::Compact);
}
//![3] WebSocketClient
/* 收到信息*/
void DWebSocketServer::onClientMessageReceived_slot(QString message)
{
    auto pClient = qobject_cast<QWebSocket *>(sender());

    qint64 srcId = 0;
    qint64 innerId = ++m_seqId;

    qDebug().noquote() << QString(">>%1:innerId:%2[receive]").arg(pClient->peerPort()).arg(innerId) << message;

    if (ExchangeJsonrpcId(message, srcId, innerId))
    {//成功时把值保存起来，因为receiveMassage_signal里面成功时才把消息发给插件
        m_mapClient.insert(innerId, qMakePair(srcId,pClient));
    }
    InsertOriginJsonrpcId(message, srcId);
    emit receiveMassage_signal(pClient->peerPort(), message);
}

/* 客户端断开连接 */
void DWebSocketServer::onClientDisconnection_slot()
{
    auto pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        //m_clientMap.remove(pClient->peerPort());
        m_allClient.remove(pClient);
        auto itr = m_mapClient.begin();
        while (itr != m_mapClient.end())
        {
            if (itr.value().second == pClient)
            {
                itr = m_mapClient.erase(itr);
            }
            else
            {
                ++itr;
            }
        }
        qDebug() << "! DWebSocket client disconnected. port:"
                 << pClient->peerPort();
        emit clientClose_signal(pClient->peerPort());
    }
}


