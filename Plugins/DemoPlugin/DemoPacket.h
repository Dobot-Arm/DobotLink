#ifndef DEMOPACKET_H
#define DEMOPACKET_H

#include <QString>
#include <QJsonObject>
#include <QJsonValue>

class DemoRequestPacket
{
public:
    DemoRequestPacket();

public:
    quint16 wsPort;

    quint64 id;
    QString method;
    QString target;
    QString api;
    QJsonObject paramsObj;

    bool setPacketFromObj(QJsonObject obj);
    QJsonValue getParamValue(QString key) const;
};

class DemoResultPacket
{
public:
    DemoResultPacket(quint64 id = 0, quint16 port = 0);
    DemoResultPacket(const DemoRequestPacket &packet);

public:
    QJsonObject m_resObj;

    bool isEmpty();
    QJsonObject getResultObj(QJsonValue data = QJsonValue());
    QJsonObject getErrorObj(int code, QString message = QString());
    QJsonObject getErrorObjWithCode(int code);
    QJsonObject setErrorObj(QJsonObject errorObj);

private:
    quint64 m_id;
    quint16 m_wsPort;
};


#endif // DEMOPACKET_H
