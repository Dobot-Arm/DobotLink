#include "M1Packet.h"

#include <QStringList>
#include <QtDebug>

const QString JsonRPCVersion = "2.0";
const int ErrorBaseIndex = 100;

M1Packet::M1Packet()
{

}

bool M1Packet::setPacket(QJsonObject obj)
{
    if (obj.contains("WSport")) {
        port = static_cast<quint16>(obj.value("WSport").toInt());
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
            /* maybe it contains 'data' 'type' etc. */
        }
    }

    if (api.isEmpty()) {
        return false;
    }
    return true;
}

M1ResPacket::M1ResPacket(quint64 id, quint16 port) : m_id(id), m_port(port)
{
    resObj.insert("WSport", port);
    resObj.insert("id", static_cast<double>(id));
    resObj.insert("jsonrpc", JsonRPCVersion);
}

void M1ResPacket::setResultObj(QJsonValue data)
{
    m_resultVal = data;
}

void M1ResPacket::setErrorObj(int code, QString message)
{
    if (code != 0) {
        errorObj.insert("code", ErrorBaseIndex + code);
    }
    if (!message.isEmpty()) {
        errorObj.insert("message", message);
    }
}

QJsonObject M1ResPacket::getResultObj()
{
    if (errorObj.isEmpty()) {
        resObj.insert("result", m_resultVal);
    } else {
        resObj.insert("error", errorObj);
    }
    return resObj;
}


