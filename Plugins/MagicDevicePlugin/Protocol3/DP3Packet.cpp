#include "DP3Packet.h"
#include <QStringList>

const QString JsonRPCVersion = "2.0";
/*
    | 400~499 | ProtocolFrame错误码                         |
*/

P3Packet::P3Packet() {
    wsPort = 0;
    id = 0;
}

bool P3Packet::setPacketFromObj(const QJsonObject& obj) {
    if (obj.contains("WSport")) {
        wsPort = static_cast<quint16>(obj.value("WSport").toInt());
    }

    id = static_cast<quint64>(obj.value("id").toDouble());
    target = obj.value("targetType").toString();
    api = obj.value("cmd").toString();

    if (obj.contains("params")) {
        QJsonValue value = obj.value("params");
        if (value.isObject()) {
            paramsObj = value.toObject();
            if (paramsObj.contains("portName")) {
                portName = paramsObj.value("portName").toString();
            }
        }
    }

    if (api.isEmpty() || target.isEmpty() || !wsPort) {
        return false;
    }
    return true;
}

P3ResPacket::P3ResPacket(quint64 id, quint16 wsPort) : m_id(id), m_wsPort(wsPort) {
    resObj.insert("WSport", wsPort);    //port指websocket通信的端口
    resObj.insert("id", static_cast<double>(id));
    resObj.insert("jsonrpc", JsonRPCVersion);
}

void P3ResPacket::setResultObj(const QJsonObject& data) {
    m_resultObj = data;
}

void P3ResPacket::setErrorObj(int code) {
    if (code != 0) {
        m_errorObj.insert("code", code);
        m_errorObj.insert("message", DError::getErrorMessage(code));
    }
}

const QJsonObject& P3ResPacket::getResultObj() {
    if (m_errorObj.isEmpty()) {
        if(m_resultObj.contains("result") and m_resultObj.value("result") == "true"){
            resObj.insert("result", true);
        } else {
            resObj.insert("result", m_resultObj);
        }
    } else {
        resObj.insert("error", m_errorObj);
    }

    return resObj;
}

