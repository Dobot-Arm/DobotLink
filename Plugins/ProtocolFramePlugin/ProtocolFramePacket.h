#ifndef PROTOCOLFRAMEPACKET_H
#define PROTOCOLFRAMEPACKET_H

#include <QString>
#include <QJsonObject>

class ProtocolFramePacket {
public:
    ProtocolFramePacket();

public:
    quint16 wsPort;  //websocket的端口

    quint64 id;
    QString method;
    QString target;  //M1
    QString api;  //searchDobot

    QJsonObject paramsObj;  //传入参数
    QString portName;  //连接的串口名字

    bool setPacketFromObj(const QJsonObject& obj);
};

class ProtocolFrameResPacket {
public:
    ProtocolFrameResPacket(quint64 id = 0, quint16 port = 0);

public:
    QJsonObject resObj;
    QJsonObject m_errorObj;
    QJsonObject m_resultObj;

    void setResultObj(const QJsonObject& data);
    void setErrorObj(int code, QString message = QString());

    const QJsonObject& getResultObj();

private:
    quint64 m_id;
    quint16 m_wsPort;
};

#endif // PROTOCOLFRAMEPACKET_H
