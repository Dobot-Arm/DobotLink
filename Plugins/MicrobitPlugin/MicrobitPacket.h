#ifndef MICROBITPACKET_H
#define MICROBITPACKET_H

#include <QString>
#include <QJsonObject>

class MicrobitPacket
{
public:
    MicrobitPacket();

public:
    quint16 port;

    double id;
    QString method;
    QString target;
    QString api;

    QJsonObject paramsObj;
    QString type;
    QString data;

    bool setPacket(QJsonObject obj);
};

class MicrobitResPacket
{
public:
    MicrobitResPacket(double id = 0, quint16 port = 0);

public:
    QJsonObject resObj;
    QJsonObject m_errorObj;
    QJsonObject m_resultObj;

    void setResultObj(QJsonObject data = QJsonObject());
    void setErrorObj(int code, QString message = QString());

    QJsonObject getResultObj();

private:
    double m_id;
    quint16 m_port;
};

#endif // MICROBITPACKET_H
