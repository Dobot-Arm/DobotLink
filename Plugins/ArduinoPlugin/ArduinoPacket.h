#ifndef ARDUINOPACKET_H
#define ARDUINOPACKET_H

#include <QString>
#include <QJsonObject>

class ArduinoPacket
{
public:
    ArduinoPacket();

public:
    quint16 port;

    quint64 id;
    QString method;
    QString target;
    QString api;
    quint64 originRequestId;

    QJsonObject paramsObj;
    QString type;
    QString portName;
    QString data;

    bool setPacket(QJsonObject obj);
};

class ArduinoResPacket
{
public:
    ArduinoResPacket(quint64 id = 0, quint64 originRequestId = 0, quint16 port = 0);

public:
    QJsonObject resObj;
    QJsonObject m_errorObj;
    QJsonObject m_resultObj;

    void setResultObj(QJsonObject data = QJsonObject());
    void setErrorObj(int code, QString message = QString());

    QJsonObject getResultObj();

private:
    QJsonObject getParamsObj();

private:
    quint64 m_id;
    quint64 m_originRequestId;
    quint16 m_port;
};

#endif // ARDUINOPACKET_H
