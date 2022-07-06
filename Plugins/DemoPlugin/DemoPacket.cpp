#include "DemoPacket.h"

#include <QStringList>

const QString JsonRPCVersion = "2.0";

DemoRequestPacket::DemoRequestPacket()
{
    wsPort = 0;
    id = 0;
}

bool DemoRequestPacket::setPacketFromObj(QJsonObject obj)
{
    if (obj.contains("WSport")) {
        wsPort = static_cast<quint16>(obj.value("WSport").toInt());
    }

    id = static_cast<quint64>(obj.value("id").toDouble());

    if (obj.contains("method")) {
        QJsonValue value = obj.value("method");
        if (value.isString()) {
            method = value.toString();
            if (method.startsWith("dobotlink")) {
                QStringList methodList = method.split(".", QString::SkipEmptyParts);
                if (methodList.count() >= 2) {
                    target = methodList.at(1);
                }
                if (methodList.count() >= 3) {
                    api = methodList.at(2);
                }
            }
        }
    }

    if (obj.contains("params")) {
        QJsonValue value = obj.value("params");
        if (value.isObject()) {
            paramsObj = value.toObject();
        }
    }

    if (method.isEmpty()) {
        return false;
    }

    return true;
}

QJsonValue DemoRequestPacket::getParamValue(QString key) const
{
    return paramsObj.value(key);
}



DemoResultPacket::DemoResultPacket(quint64 id, quint16 port) : m_id(id), m_wsPort(port)
{
    m_resObj.insert("jsonrpc", JsonRPCVersion);
    m_resObj.insert("WSport", m_wsPort);
    m_resObj.insert("id", static_cast<double>(m_id));
}

DemoResultPacket::DemoResultPacket(const DemoRequestPacket &packet)
{
    m_resObj.insert("jsonrpc", JsonRPCVersion);
    m_resObj.insert("WSport", packet.wsPort);
    m_resObj.insert("id", static_cast<double>(packet.id));
}

bool DemoResultPacket::isEmpty()
{
    if (m_resObj.contains("result")) {
        return false;
    } else if (m_resObj.contains("error")) {
        return false;
    }
    return true;
}

QJsonObject DemoResultPacket::getResultObj(QJsonValue data)
{
    if (data.isNull() or data.isUndefined()) {
        m_resObj.insert("result", true);
    } else {
        m_resObj.insert("result", data);
    }
    return m_resObj;
}

QJsonObject DemoResultPacket::getErrorObj(int code, QString message)
{
    QJsonObject errorObj;
    errorObj.insert("code", code);
    if (!message.isEmpty()) {
        errorObj.insert("message", message);
    }

    m_resObj.insert("error", errorObj);
    return m_resObj;
}

QJsonObject DemoResultPacket::getErrorObjWithCode(int code)
{
    QJsonObject errorObj;
    errorObj.insert("code", code);
    errorObj.insert("message", QString("code:%1").arg(code));

    m_resObj.insert("error", errorObj);
    return m_resObj;
}

QJsonObject DemoResultPacket::setErrorObj(QJsonObject errorObj)
{
    m_resObj.insert("error", errorObj);
    return m_resObj;
}
