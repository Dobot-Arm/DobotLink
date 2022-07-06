#ifndef DSERIALPORT_H
#define DSERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QJsonObject>
#include <QMap>

class DSerialPort : public QObject
{
    Q_OBJECT
public:
    explicit DSerialPort(QObject *parent = nullptr);

    static QJsonValue getAvailablePortInfo();
    static QStringList getAvailablePortList();
};

#endif // DSERIALPORT_H
