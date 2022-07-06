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
    connect(m_WebSocketServer, &QWebSocketServer::newConnection, this, &DWebSocketServer::onNewConnection_slot);
    connect(m_WebSocketServer, &QWebSocketServer::closed, this, &DWebSocketServer::onWebSocketServerClosed_slot);

    bool ok = m_WebSocketServer->listen(QHostAddress::Any, 9090);
    if (ok == false) {
        qDebug() << "[WebSocket-Server] listen error." << m_WebSocketServer->errorString();
    } else {
        qDebug() << "DobotLink WebSocketServer PORT: [9090], enjoy it." << endl;
    }
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
        foreach(QWebSocket* pConn , m_allClient)
        {
            if (pConn->isValid())
            {
                pConn->sendTextMessage(message);
            }
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
        delete *itr;
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

