#include "ProtocolFramePacket.h"
#include <QDebug>
#include <QStringList>

const QString JsonRPCVersion = "2.0";
const int ErrorBaseIndex = 400;
/*
    | 400~499 | ProtocolFrame错误码                         |
*/

ProtocolFramePacket::ProtocolFramePacket() {
    wsPort = 0;
    id = 0;
}

bool ProtocolFramePacket::setPacketFromObj(const QJsonObject& obj) {
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
            if (paramsObj.contains("portName")) {
                portName = paramsObj.value("portName").toString();
            }
        }
    }

    if (api.isEmpty()) {
        return false;
    }
    return true;
}

ProtocolFrameResPacket::ProtocolFrameResPacket(quint64 id, quint16 wsPort) : m_id(id), m_wsPort(wsPort) {
    resObj.insert("WSport", wsPort);    //port指websocket通信的端口
    resObj.insert("id", static_cast<double>(id));
    resObj.insert("jsonrpc", JsonRPCVersion);
}

void ProtocolFrameResPacket::setResultObj(const QJsonObject& data) {
    m_resultObj = data;
}

void ProtocolFrameResPacket::setErrorObj(int code, QString message) {
    if (code != 0) {
        m_errorObj.insert("code", ErrorBaseIndex + code);
    }
    if (!message.isEmpty()) {
        m_errorObj.insert("message", message);
    }
}

const QJsonObject& ProtocolFrameResPacket::getResultObj() {
    if (m_errorObj.isEmpty()) {
        resObj.insert("result", m_resultObj);
    } else {
        resObj.insert("error", m_errorObj);
    }

    return resObj;
}

