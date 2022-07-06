#ifndef DTCPSOCKETSERVER_H
#define DTCPSOCKETSERVER_H

#include <QObject>

class QTcpServer;
class QTcpSocket;
class DTcpSocketServer : public QObject
{
    Q_OBJECT
public:
    static DTcpSocketServer *getInstance();

    void sendMessage(QByteArray data);
    void closeServer();

private:
    Q_DISABLE_COPY(DTcpSocketServer)
    explicit DTcpSocketServer(QObject *parent = nullptr);

    QTcpServer *m_server;
    QList<QTcpSocket *> m_clients;

private slots:
    /* TCP SERVER */
    void onNewClient_slot();

    /* TCP CLIENT */
    void onClientReadData_slot();
    void onClientDisconnect_slot();
};

#endif // DTCPSOCKETSERVER_H
