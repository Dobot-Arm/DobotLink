#include "DelayToQuit.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

const QString HOSTADDRESS = "ws://localhost";
const QString PORT = "9090";

DelayToQuit::DelayToQuit(QObject *parent) : QObject(parent)
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &DelayToQuit::timeOutAndQuit_slot);

    m_websocket = new QWebSocket();
    m_websocket->setParent(this);
    connect(m_websocket, &QWebSocket::connected, this, &DelayToQuit::onConnected_slot);
    connect(m_websocket, &QWebSocket::textMessageReceived, this, &DelayToQuit::onMessageReceive_slot);
}

DelayToQuit::~DelayToQuit()
{
    qDebug() << "fail to start, because another dobotlink is still running.";
}

void DelayToQuit::start()
{
    m_timer->start(2000);
    m_websocket->open(QUrl(HOSTADDRESS + ":" + PORT));
}

void DelayToQuit::timeOutAndQuit_slot()
{
    m_websocket->abort();
    qApp->quit();
}

void DelayToQuit::onConnected_slot()
{
    QJsonObject sendObj;
    sendObj.insert("method", "dobotlink.api.ShowMessage");
    QJsonObject paramsObj;
    paramsObj.insert("message", tr("DobotLink is running. Another dobotlink is Closed."));
    sendObj.insert("params", paramsObj);

    QJsonDocument doc(sendObj);
    QByteArray byteArray = doc.toJson(QJsonDocument::Compact);

    m_websocket->sendTextMessage(byteArray);
}

void DelayToQuit::onMessageReceive_slot(QString message)
{
    qDebug() << "DelayToQuit received:" << message;

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &jsonError);
    if (jsonError.error == QJsonParseError::NoError) {
        QJsonObject msgObj = doc.object();
        if (msgObj.contains("result")) {
            QJsonValue resultValue = msgObj.value("result");
            if (resultValue.isBool()) {
                if (resultValue.toBool() == true) {
                    m_timer->stop();
                    timeOutAndQuit_slot();
                }
            }
        }
    }
}

