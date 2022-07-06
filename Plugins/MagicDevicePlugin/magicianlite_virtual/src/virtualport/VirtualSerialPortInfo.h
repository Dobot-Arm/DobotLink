#pragma once

#include <QList>

class CVirtualSerialPortInfo
{
public:
    explicit CVirtualSerialPortInfo();
    explicit CVirtualSerialPortInfo(const QString &name);
    ~CVirtualSerialPortInfo();

    QString portName() const;

    static bool IsVirtualPort(const QString& strPortName);

    static QList<CVirtualSerialPortInfo> availablePorts();

private:
    QString m_strPortName;
};
