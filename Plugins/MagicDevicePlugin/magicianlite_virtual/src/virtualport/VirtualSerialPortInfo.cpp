#include "VirtualSerialPortInfo.h"

#include <QRegExp>

CVirtualSerialPortInfo::CVirtualSerialPortInfo()
{
}

CVirtualSerialPortInfo::CVirtualSerialPortInfo(const QString &name)
{
    m_strPortName = name;
}

CVirtualSerialPortInfo::~CVirtualSerialPortInfo(){}

QString CVirtualSerialPortInfo::portName() const
{
    return m_strPortName;
}

bool CVirtualSerialPortInfo::IsVirtualPort(const QString& strPortName)
{
    /*虚拟串口遵循一种规则:以VSP开头，后面紧跟正整数.
     * 例如: VSP1   VSP2   VSP3 ...
    */
    QRegExp reg("^VSP[1-9][0-9]{0,}$");
    return reg.exactMatch(strPortName);
}

QList<CVirtualSerialPortInfo> CVirtualSerialPortInfo::availablePorts()
{
    QList<CVirtualSerialPortInfo> all;
    all.append(CVirtualSerialPortInfo("VSP1"));
    return all;
}
