#ifndef DWEBSOCKETSERVER_H
#define DWEBSOCKETSERVER_H

#include <QObject>
#include <QJsonObject>
#include <QMap>
#include <QSet>
#include <QAtomicInteger>
#include <QPair>

class QWebSocket;
class QWebSocketServer;
class DWebSocketServer : public QObject
{
    Q_OBJECT
public:
    static DWebSocketServer *getInstance();

    void sendMessage(quint16 port, QString message);
    void sendMessageObj(QJsonObject obj);
    void closeServer();

signals:
    void receiveMassage_signal(quint16 port, QString message);
    void clientClose_signal(quint16 port);

private:
    Q_DISABLE_COPY(DWebSocketServer)
    explicit DWebSocketServer(QObject *parent = nullptr);

    /*
     * 功能：为了能够兼容多个用户出现相同id的情况，这里拦截jsonrpc的id，并替换为自己的
    * 参数：strMsg-jsonrpc字符串，输入输出参数。
    *       srcId-strMsg字符串中的id字段，输出参数
    *       replaceId-替换的id，输入参数
    * 返回：成功true，否则false
    */
    bool ExchangeJsonrpcId(QString& strMsg, qint64& srcId, qint64 replaceId);
    qint64 GetJsonrpcId(QString strMsg);

    static QAtomicInteger<qint64> m_seqId;

    QWebSocketServer *m_WebSocketServer;
    //QMap<quint16, QWebSocket *> m_clientMap;
    QHash<qint64, //内部生成的jsonrpc-id
        QPair<qint64, //jsonrpc原始id
        QWebSocket*> //ws对象
    > m_mapClient;
    QSet<QWebSocket*> m_allClient;

    quint16 CLIENT_PORT;

private slots:
//![WebSocketServer]
    void onNewConnection_slot();
    void onWebSocketServerClosed_slot();

//![WebSocketClient]
    void onClientMessageReceived_slot(QString message);
    void onClientDisconnection_slot();
};

#endif // DWEBSOCKETSERVER_H
