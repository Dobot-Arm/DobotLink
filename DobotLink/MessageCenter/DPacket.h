#ifndef DPACKET_H
#define DPACKET_H

#include <QString>
#include <QJsonObject>
#include <QJsonValue>

#include "DError/DError.h"

class DRequestPacket
{
public:
    DRequestPacket();

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


class DResultPacket
{
public:
    DResultPacket(quint64 m_id = 0, quint16 port = 0);
    DResultPacket(const DRequestPacket &packet);

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


class DNotificationPacket
{
public:
    DNotificationPacket(quint16 port);

    QJsonObject getNotificationObj(QString method, QJsonObject params = QJsonObject());

private:
    quint16 m_wsPort;
};
#endif // DPACKET_H
