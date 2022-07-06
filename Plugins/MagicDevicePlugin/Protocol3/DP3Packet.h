#ifndef PROTOCOLFRAMEPACKET_H
#define PROTOCOLFRAMEPACKET_H

#include <QString>
#include <QJsonObject>
#include "DError/DError.h"
class P3Packet {
public:
    P3Packet();

public:
    quint16 wsPort;  //websocket的端口

    quint64 id;
    QString target;  //M1
    QString api;  //searchDobot

    QJsonObject paramsObj;  //传入参数
    QString portName;  //连接的串口名字

    bool setPacketFromObj(const QJsonObject& obj);
};

class P3ResPacket {
public:
    P3ResPacket(quint64 id = 0, quint16 port = 0);

public:
    QJsonObject resObj;
    QJsonObject m_errorObj;
    QJsonObject m_resultObj;

    void setResultObj(const QJsonObject& data);
    void setErrorObj(int code);

    const QJsonObject& getResultObj();

private:
    quint64 m_id;
    quint16 m_wsPort;
};

#endif // PROTOCOLFRAMEPACKET_H
