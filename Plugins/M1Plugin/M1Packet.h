#ifndef MAGICIANPACKET_H
#define MAGICIANPACKET_H

#include <QString>
#include <QJsonObject>
#include <QJsonValue>

class M1Packet
{
public:
    M1Packet();

public:
    quint16 port;

    quint64 id;
    QString method;
    QString target;
    QString api;

    QJsonObject paramsObj;
    QString portName;

    bool setPacket(QJsonObject obj);
};

class M1ResPacket
{
public:
    M1ResPacket(quint64 id = 0, quint16 port = 0);

public:
    QJsonObject resObj;
    QJsonObject errorObj;
    QJsonValue m_resultVal;

    void setResultObj(QJsonValue data);
    void setErrorObj(int code, QString message = QString());

    QJsonObject getResultObj();

private:
    double m_id;
    quint16 m_port;
};


#endif // MAGICIANPACKET_H
