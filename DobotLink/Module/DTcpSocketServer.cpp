#include "DTcpSocketServer.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

DTcpSocketServer *DTcpSocketServer::getInstance()
{
    static DTcpSocketServer *instance = nullptr;
    if (instance == nullptr) {
        instance = new DTcpSocketServer();
    }
    return instance;
}

DTcpSocketServer::DTcpSocketServer(QObject *parent) : QObject(parent)
{
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &DTcpSocketServer::onNewClient_slot);

    if (!m_server->listen(QHostAddress::Any, 9091)) {
        qDebug() << "[TCP-server] listen error.";
    }
}

void DTcpSocketServer::sendMessage(QByteArray data)
{
    for (int i = 0; i < m_clients.count(); ++i) {
        m_clients.at(i)->write(data);
    }
}

void DTcpSocketServer::closeServer()
{
    m_server->close();
    qDeleteAll(m_clients);
}

void DTcpSocketServer::onNewClient_slot()
{
    qDebug() << "[TCP-server] new client connected.";
    QTcpSocket *client = m_server->nextPendingConnection();

    connect(client, &QTcpSocket::disconnected, this, &DTcpSocketServer::onClientDisconnect_slot);
    connect(client, &QTcpSocket::readyRead, this, &DTcpSocketServer::onClientReadData_slot);

    m_clients.append(client);
}

void DTcpSocketServer::onClientReadData_slot()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    QString msg = client->readAll();
    qDebug() << "[TCP-server] get message:" << msg;
}

void DTcpSocketServer::onClientDisconnect_slot()
{
    qDebug() << "[TCP-server] client disconnect.";
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());

    m_clients.removeOne(client);
}
