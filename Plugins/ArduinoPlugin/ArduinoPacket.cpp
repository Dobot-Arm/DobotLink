#include "ArduinoPacket.h"

#include <QStringList>

const QString JsonRPCVersion = "2.0";
const int ErrorBaseIndex = 200;

ArduinoPacket::ArduinoPacket()
{

}

bool ArduinoPacket::setPacket(QJsonObject obj)
{
    if (obj.contains("WSport")) {
        port = static_cast<quint16>(obj.value("WSport").toInt());
    }

    id = static_cast<quint64>(obj.value("id").toDouble());
    if (obj.contains("originRequestId"))
    {
        originRequestId = static_cast<quint64>(obj.value("originRequestId").toDouble());
    }
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
            if (paramsObj.contains("type")) {
                type = paramsObj.value("type").toString();
            }
            if (paramsObj.contains("data")) {
                data = paramsObj.value("data").toString();
            }
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

ArduinoResPacket::ArduinoResPacket(quint64 id, quint64 originRequestId, quint16 port)
    : m_id(id), m_originRequestId(originRequestId), m_port(port)
{
    resObj.insert("WSport", port);
    resObj.insert("id", static_cast<double>(id));
    resObj.insert("originRequestId", static_cast<double>(originRequestId));
    resObj.insert("jsonrpc", JsonRPCVersion);
}

void ArduinoResPacket::setResultObj(QJsonObject data)
{
    m_resultObj = data;
}

void ArduinoResPacket::setErrorObj(int code, QString message)
{
    if (code != 0) {
        m_errorObj.insert("code", ErrorBaseIndex + code);
    }
    if (!message.isEmpty()) {
        m_errorObj.insert("message", message);
    }
}

QJsonObject ArduinoResPacket::getResultObj()
{
    if (m_errorObj.isEmpty()) {
        if (m_resultObj.isEmpty()) {
            resObj.insert("result", true);
        } else {
            resObj.insert("result", m_resultObj);
        }
    } else {
        resObj.insert("error", m_errorObj);
    }
    return resObj;
}
