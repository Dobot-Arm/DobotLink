#include "VirtualSerialPort.h"

#include <QtGlobal>

#include "VirtualSerialPortManager.h"

CVirtualSerialPort::CVirtualSerialPort(QObject *parent) : QIODevice(parent)
{
    CVirtualSerialPortManager::GetInstance()->Register(this);
    connect(this, SIGNAL(signalInnerWriteData(QByteArray)),
            CVirtualSerialPortManager::GetInstance(),SLOT(slotWriteData(QByteArray)));
    connect(this,SIGNAL(signalInnerReadData(QByteArray)),
            this, SLOT(slotInnerReadData(QByteArray)));
}

CVirtualSerialPort::~CVirtualSerialPort()
{
    CVirtualSerialPortManager::GetInstance()->Unregister(this);
}
	
void CVirtualSerialPort::setPortName(const QString &name)
{
    m_strPortName = name;
}

QString CVirtualSerialPort::portName() const
{
    return m_strPortName;
}

bool CVirtualSerialPort::open(QIODevice::OpenMode mode)
{
    m_readBuffer.clear();
    setOpenMode(mode);
    return true;
}

void CVirtualSerialPort::close()
{
    m_readBuffer.clear();
    setOpenMode(QIODevice::NotOpen);
    return;
}
	
	
bool CVirtualSerialPort::setBaudRate(qint32 baudRate, int directions)
{
    Q_UNUSED(baudRate);
    Q_UNUSED(directions);
    return true;
}

bool CVirtualSerialPort::setDataBits(int dataBits)
{
    Q_UNUSED(dataBits);
    return true;
}

bool CVirtualSerialPort::setParity(int parity)
{
    Q_UNUSED(parity);
    return true;
}

bool CVirtualSerialPort::setStopBits(int stopBits)
{
    Q_UNUSED(stopBits);
    return true;
}

bool CVirtualSerialPort::setFlowControl(int flowControl)
{
    Q_UNUSED(flowControl);
    return true;
}

void CVirtualSerialPort::slotInnerReadData(QByteArray data)
{
    m_readBuffer.append(data);
    emit readyRead();
}

qint64 CVirtualSerialPort::readData(char *data, qint64 maxSize)
{
    int iLen = qMin<int>(m_readBuffer.length(), maxSize);
    if (nullptr != data && iLen > 0)
    {
        memcpy(data, m_readBuffer.data(), iLen);
        m_readBuffer.remove(0, iLen);
    }
    return iLen;
}

qint64 CVirtualSerialPort::writeData(const char *data, qint64 maxSize)
{
    QByteArray arr(data, (int)maxSize);
    emit signalInnerWriteData(arr, QPrivateSignal());
    return maxSize;
}
