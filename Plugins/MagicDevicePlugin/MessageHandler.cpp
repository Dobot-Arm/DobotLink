#include "MessageHandler.h"
#ifndef __wasm__
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QUdpSocket>
#include <QNetworkDatagram>
#else
#include "WebAssembly/EmSerialPort.h"
#endif
#include <QTimer>
#include <QStringList>
#include <QDebug>
#include <QEventLoop>

#include "DError/DError.h"
#include "DProtocol2.h"
#include "DProtocol3.h"

#include "virtualport/VirtualSerialPort.h"
#include "virtualport/VirtualSerialPortInfo.h"
#include "virtualport/VSPServer.h"

#define CLIENT_PORT (12345)     /* udp客户端端口号默认设置 */
#define HEAD_P2_1 (0xAA)
#define HEAD_P2_2 (0xAA)
#define HEAD_P3_1 (0xAA)
#define HEAD_P3_2 (0xBB)

MessageHandler::MessageHandler(QObject *parent) : QObject(parent)
{ 
    m_protocol2 = new DProtocol2(this);
    connect(m_protocol2, &DProtocol2::sendRawData_signal, this, &MessageHandler::sendRawData_slot);
    connect(m_protocol2, &DProtocol2::recviceData_signal, this, &MessageHandler::recviceP2Data_signal);
    connect(m_protocol2, &DProtocol2::errorOccurred_signal, this, &MessageHandler::errorOccurred_signal);

    m_protocol3 = new DProtocol3(this);
    connect(m_protocol3, &DProtocol3::sendRawData_signal, this, &MessageHandler::sendRawData_slot);
    connect(m_protocol3, &DProtocol3::recviceData_signal, this, &MessageHandler::recviceP3Data_signal);
    connect(m_protocol3, &DProtocol3::errorOccurred_signal, this, &MessageHandler::errorOccurred_signal);

    m_isConnecting = false;
    m_isConnected = false;
    m_isSerialConnection = true;
    m_deviceUdpPort = CLIENT_PORT;

    _TimerInit();
    _SerialPortInit();
    _UdpSocketInit();
}

MessageHandler::~MessageHandler()
{
    if (m_isConnected == true) {
       disconnectDevice();
    }
    m_ioDevice->close();
}

void MessageHandler::setProtocolType(ProtocolType type)
{
    m_ProtocolType = type;
}

void MessageHandler::_TimerInit()
{
    /* Timer - keep alive */
    m_timerDeviceKeepAlive = new QTimer(this);
    m_timerDeviceKeepAlive->setInterval(500);
    connect(m_timerDeviceKeepAlive, &QTimer::timeout, this, [=](){
        bool check = false;
        if (nullptr != qobject_cast<CVirtualSerialPort*>(m_ioDevice))
        {
            foreach (const CVirtualSerialPortInfo &portInfo, CVirtualSerialPortInfo::availablePorts()){
                if (portInfo.portName() == getPortName()) {
                    check = true;
                    break;
                }
            }
        }
        else
        {
#ifndef __wasm__
            foreach (const QSerialPortInfo &portInfo, QSerialPortInfo::availablePorts()) {
#else
            foreach (const EmSerialPortInfo &portInfo, EmSerialPortInfo::availablePorts()) {
#endif
                if (portInfo.portName() == getPortName()) {
                    check = true;
                    break;
                }
            }
        }
        if (check == false) {
            disconnectDevice();
            emit errorOccurred_signal(ERROR_DEVICE_LOST_CONNECTION);
        }
    });
}

bool MessageHandler::_bindLocalHostIp()
{
#ifndef __wasm__
    quint16 port;
    for (port = m_deviceUdpPort + 1; port < m_deviceUdpPort + 100; ++port) {
        bool ok = m_udpSocket->bind(QHostAddress(m_localhostIp), port);
        if (ok) {
            qDebug().noquote() << "UDP:set HostIpAddress:" << m_localhostIp << "port:" << port;
            return true;
        }
    }
    qDebug() << "UDP:fail to find HostIpAddress.";
#endif
    return false;
}

void MessageHandler::_SerialPortInit()
{
#ifndef __wasm__
    QSerialPort *_serialPort = new QSerialPort(this);
    _serialPort->setBaudRate(QSerialPort::Baud115200);
    _serialPort->setDataBits(QSerialPort::Data8);
    _serialPort->setStopBits(QSerialPort::OneStop);
    _serialPort->setParity(QSerialPort::NoParity);
    _serialPort->setFlowControl(QSerialPort::NoFlowControl);

    connect(_serialPort, &QSerialPort::errorOccurred, this, [=](QSerialPort::SerialPortError error) {
        // 串口报错，使用通知的形式上报给上位机
        if (error != QSerialPort::NoError) {
            qDebug() << "QSerialPort error:" << error << _serialPort->errorString();
            int code = ERROR_SERIALPORT + error;
            if (error == QSerialPort::ResourceError)
            {//当串口被拔下来时，发生了这个错误
                m_isConnected = false;

                //但是此时 m_ioDevice->isOpen()仍然返回true，所以强制close();
                if (m_ioDevice->isOpen()){
                    m_ioDevice->close();
                }
            }

            emit serialErrorOccuerd_signal(code, _serialPort->errorString());
        }
    });
#else
    EmSerialPort *_serialPort = new EmSerialPort(this);

    connect(_serialPort, &EmSerialPort::errorOccurred, this, [=](EmSerialPort::SerialPortError error) {
        // 串口报错，使用通知的形式上报给上位机
        if (error != EmSerialPort::NoError) {
            qDebug() << "EmSerialPort error:" << error << _serialPort->errorString();
            m_isConnected = false;
            if (m_ioDevice->isOpen()){
                m_ioDevice->close();
            }
            emit serialErrorOccuerd_signal(int(error), _serialPort->errorString());
        }
    });
#endif
    connect(_serialPort, &QIODevice::readyRead, this, &MessageHandler::onSerialReadyRead_slot);
    m_ioDevice = _serialPort;
}

void MessageHandler::_UdpSocketInit()
{
#ifndef __wasm__
    m_udpSocket = new QUdpSocket(this);
    connect(m_udpSocket, &QUdpSocket::readyRead,
            this, &MessageHandler::onReadDatagram_slot);
#endif
}

bool MessageHandler::_checkMarlin(QString &fwVer)
{
    QRegExp regExpMARLIN("MARLIN\\:\\sV((\\d+\\.){2}\\d+)");
    QByteArray sendBytes = "\nM10\nM10\nM10\nM10\nM10\n";

    if (m_isSerialConnection) {
        m_ioDevice->write(sendBytes);
    } else {
#ifndef __wasm__
        m_udpSocket->writeDatagram(sendBytes, QHostAddress(m_deviceUdpIp), m_deviceUdpPort);
#endif
    }

    QEventLoop loop;
    QTimer timer;
    timer.setInterval(100);
    timer.setSingleShot(true);
    timer.start();

    connect(&timer, &QTimer::timeout, &loop, [&loop]() {
        loop.quit();
    });

    if (m_isSerialConnection) {
        connect(m_ioDevice, &QIODevice::readyRead, &loop, [this, &loop, &regExpMARLIN, &fwVer]() {
            QString recvStr = m_ioDevice->readAll();
            if (recvStr.indexOf(regExpMARLIN) >= 0) {
                fwVer = regExpMARLIN.cap(1);
                loop.quit();
            }
        });
    } else {
#ifndef __wasm__
        connect(m_udpSocket, &QUdpSocket::readyRead, &loop, [this, &loop, &regExpMARLIN, &fwVer]() {
            while (m_udpSocket->hasPendingDatagrams()) {
                QNetworkDatagram datagram = m_udpSocket->receiveDatagram();
                QString recvStr = datagram.data();

                if (recvStr.indexOf(regExpMARLIN) >= 0) {
                    fwVer = regExpMARLIN.cap(1);
                    loop.quit();
                }
            }
        });
#endif
    }

    loop.exec();

    if (timer.isActive()) {
        timer.stop();
        return true;
    } else {
        return false;
    }
}

void MessageHandler::_checkMarlin(const std::function<void(QString fwVer, bool bIsMarlin)>& callback)
{
    QRegExp regExpMARLIN("MARLIN\\:\\sV((\\d+\\.){2}\\d+)");
    QByteArray sendBytes = "\nM10\nM10\nM10\nM10\nM10\n";

    if (m_isSerialConnection) {
        m_ioDevice->write(sendBytes);
    } else {
#ifndef __wasm__
        m_udpSocket->writeDatagram(sendBytes, QHostAddress(m_deviceUdpIp), m_deviceUdpPort);
#endif
    }

    QTimer* pTimer = new QTimer(this);
    pTimer->setInterval(100);
    pTimer->setSingleShot(true);
    pTimer->start();

    connect(pTimer, &QTimer::timeout, [pTimer,callback]() {
        callback("", false);
        pTimer->deleteLater();
    });

    if (m_isSerialConnection) {
        connect(m_ioDevice, &QIODevice::readyRead, [this, regExpMARLIN, callback, pTimer]() {
            QString recvStr = m_ioDevice->readAll();
            if (recvStr.indexOf(regExpMARLIN) >= 0) {
                pTimer->stop();
                pTimer->deleteLater();
                callback(regExpMARLIN.cap(1), true);
            }
        });
    } else {
#ifndef __wasm__
        connect(m_udpSocket, &QUdpSocket::readyRead, [this, regExpMARLIN, callback, pTimer]() {
            while (m_udpSocket->hasPendingDatagrams()) {
                QNetworkDatagram datagram = m_udpSocket->receiveDatagram();
                QString recvStr = datagram.data();

                if (recvStr.indexOf(regExpMARLIN) >= 0) {
                    pTimer->stop();
                    pTimer->deleteLater();
                    callback(regExpMARLIN.cap(1), true);
                }
            }
        });
#endif
    }
}

/* 连接设备串口 */
bool MessageHandler::connectDevice(quint64 id, QString &devType, QString &fwType, QString &fwVer)
{
    if (m_isConnecting) {
        emit errorOccurred_signal(ERROR_SERIALPORT_OPEN, id);
        return false;
    }
    m_isConnecting = true;

    if (m_isConnected == true) {
        qDebug() << "Please disconnect device first. 请先断开连接";
        emit errorOccurred_signal(ERROR_SERIALPORT_OPEN, id);
        m_isConnecting = false;
        return false;
    }

    if (m_isSerialConnection) {
        if(m_ioDevice->open(QIODevice::ReadWrite)) {
            m_timerDeviceKeepAlive->start();
        } else {
            emit errorOccurred_signal(ERROR_SERIALPORT_OPEN, id);
            m_isConnecting = false;
            return false;
        }
    } else {
        if(!_bindLocalHostIp()) {
            emit errorOccurred_signal(ERROR_UDP_BIND_FAILED, id);
            m_isConnecting = false;
            return false;
        }
    }

    if (_checkMarlin(fwVer)) {
        devType = "Magician";
        fwType = "Marlin";
    } else {
        m_protocol2->start();
        m_protocol3->start();
        fwType = "Dobot";
    }

    m_isConnected = true;
    m_isConnecting = false;
    return true;
}

void MessageHandler::connectDevice(quint64 id, const std::function<void(QString devType, QString fwType, QString fwVer)>& callback)
{
    if (m_isConnecting) {
        emit errorOccurred_signal(ERROR_SERIALPORT_OPEN, id);
        return ;
    }
    m_isConnecting = true;

    if (m_isConnected == true) {
        qDebug() << "Please disconnect device first. 请先断开连接";
        emit errorOccurred_signal(ERROR_SERIALPORT_OPEN, id);
        m_isConnecting = false;
        return ;
    }

    if (m_isSerialConnection) {
        if(m_ioDevice->open(QIODevice::ReadWrite)) {
            m_timerDeviceKeepAlive->start();
        } else {
            emit errorOccurred_signal(ERROR_SERIALPORT_OPEN, id);
            m_isConnecting = false;
            return ;
        }
    } else {
        if(!_bindLocalHostIp()) {
            emit errorOccurred_signal(ERROR_UDP_BIND_FAILED, id);
            m_isConnecting = false;
            return ;
        }
    }

    _checkMarlin([this, callback, id](QString fwVer, bool bIsMarlin){
        QString devType;
        QString fwType;
        if (bIsMarlin)
        {
            devType = "Magician";
            fwType = "Marlin";
        }
        else
        {
            m_protocol2->start();
            m_protocol3->start();
            fwType = "Dobot";
        }
        callback(devType, fwType, fwVer);

        qDebug() << "MessageHandler::connectDevice end" << QString("id:%1, portName:%2").arg(id).arg(fwType);
        m_isConnected = true;
        m_isConnecting = false;
    });

    return ;
}

/* 断开连接 */
void MessageHandler::disconnectDevice(quint64 id)
{
    if (m_ioDevice->isOpen()) {
        m_ioDevice->close();
    }
#ifndef __wasm__
    if (m_udpSocket->isOpen()) {
        m_udpSocket->abort();
        m_udpSocket->close();
    }
#endif
    if (m_timerDeviceKeepAlive->isActive()) {
        m_timerDeviceKeepAlive->stop();
    }

    m_protocol2->stop();
    m_protocol3->stop();

    m_rxBuffer.clear();

    m_isConnected = false;

    QJsonObject resObj;
    resObj.insert("id", static_cast<double>(id));
    resObj.insert("cmd", "DisconnectDobot");
    emit recviceP2Data_signal(resObj);
}

bool MessageHandler::isConnected() const
{
    return m_isConnected;
}

void MessageHandler::setPortName(QString portName)
{
    m_isSerialConnection = true;

    if (CVirtualSerialPortInfo::IsVirtualPort(portName))
    {//有点恶心的做法，因为构造函数中已经把串口对象创建好了，实际还不知道当前是否为虚拟串口
        CVirtualSerialPort* pVSP = qobject_cast<CVirtualSerialPort*>(m_ioDevice);
        if (nullptr != pVSP)
        {
            pVSP->setPortName(portName);
        }
        else
        {
            m_ioDevice->disconnect();
            m_ioDevice->deleteLater();

            auto pVSPServer = new CVSPServer(this);
            pVSPServer->start(portName);

            CVirtualSerialPort *pPort = new CVirtualSerialPort(this);
            pPort->setBaudRate(115200);
            pPort->setDataBits(8);
            pPort->setStopBits(1);
            pPort->setParity(0);
            pPort->setFlowControl(0);
            pPort->setPortName(portName);

            connect(pPort, &QIODevice::readyRead, this, &MessageHandler::onSerialReadyRead_slot);
            m_ioDevice = pPort;
        }
    }
    else
    {
#ifndef __wasm__
        QSerialPort *_serialPort = static_cast<QSerialPort *>(m_ioDevice);
#else
        EmSerialPort *_serialPort = static_cast<EmSerialPort *>(m_ioDevice);
#endif
        _serialPort->setPortName(portName);
    }
}

QString MessageHandler::getPortName()
{
    if (!m_isSerialConnection) {
        return m_deviceUdpIp;
    }

    CVirtualSerialPort* pVSP = qobject_cast<CVirtualSerialPort*>(m_ioDevice);
    if (nullptr != pVSP)
    {
        return pVSP->portName();
    }
    else
    {
#ifndef __wasm__
        QSerialPort *_serialPort = static_cast<QSerialPort *>(m_ioDevice);
#else
        EmSerialPort *_serialPort = static_cast<EmSerialPort *>(m_ioDevice);
#endif
        return _serialPort->portName();
    }
}

void MessageHandler::setUdpIpAddress(QString ip)
{
    m_isSerialConnection = false;
    m_deviceUdpIp = ip;
}

QString MessageHandler::getUdpIpAddress()
{
    return m_deviceUdpIp;
}

/* 常见端口号设置 0~65535
 * 0-1023: Well-Known Ports（即公认端口号）
 * 它是一些众人皆知著名的端口号，这些端口号固定分配给一些服务，我们上面提到的 HTTP 服务、 FTP服务等都属于这一类。
 *
 * 1024-49151: Registered Ports（即注册端口）
 * 它是不可以动态调整的端口段，这些端口没有明确定义服务哪些特定的对象。不同的程序可以根据自己的需要自己定义。
 *
 * 49152–65535: Dynamic, private or ephemeral ports（即动态、私有或临时端口号）
 * 这些端口号是不可以注册的，这一段的端口被用作一些私人的或者定制化的服务，当然也可以用来做动态端口服务。
 */

void MessageHandler::setDeviceUdpPort(quint16 port)
{
    m_deviceUdpPort = port;
}

void MessageHandler::setHostIpAddress(QString ip)
{
    m_localhostIp = ip;
}

bool MessageHandler::sendProtocolData(quint64 id, DeviceType devType, QJsonObject &obj, bool needReply)
{
    if (devType == DeviceType::DEVICE_MAGICIAN_GO) {
        return m_protocol3->sendData(id, obj, needReply);
    } else {
        return m_protocol2->sendData(id, obj, needReply);
    }
}

void MessageHandler::setCommuTimeout(quint64 id, quint8 resendTimesMax, quint32 commTimeout)
{
    m_protocol2->setCommPars(resendTimesMax, commTimeout);
    m_protocol3->setCommPars(resendTimesMax, commTimeout);

    QJsonObject resObj;
    resObj.insert("id", static_cast<double>(id));
    resObj.insert("cmd", "SetCommuTimeout");
    emit recviceP2Data_signal(resObj);
}

/* 发送数据 */
void MessageHandler::sendRawData_slot(QByteArray data)
{
    if (data.isEmpty()) {
        qDebug() << __FUNCTION__ << "data is empty";
        return;
    }
    // todo: 蓝牙连接中， 一次发送量太多会有问题, 要多做一层缓冲
    if (m_isSerialConnection == true) {
        qDebug().noquote() << "# <<"
                           << QString("SerialPort:[%1][send]").arg(getPortName())
                           << data.toHex();
        qint64 byteCount = m_ioDevice->write(data);
        if (byteCount < 0) {
            qDebug() << "error:SendData error occurred.";
        }
    } else {
#ifndef __wasm__
        qDebug().noquote() << "# <<"
                           << QString("UDP:[%1][send]").arg(m_localhostIp)
                           << data.toHex();
        m_udpSocket->writeDatagram(data, QHostAddress(m_deviceUdpIp), m_deviceUdpPort);
#endif
    }
}

/* 收到串口消息-入口 */
void MessageHandler::onSerialReadyRead_slot()
{
    // 正在连的过程中会产生数据
    if (!m_isConnected) return;

    QByteArray newdata = m_ioDevice->readAll();

    qDebug().noquote() << "# >>"
                       << QString("SerialPort:[%1][receive]").arg(getPortName())
                       << newdata.toHex();

    _HandleNewDataFromDevice(newdata);
}

void MessageHandler::onReadDatagram_slot()
{
    // 正在连的过程中会产生数据
    if (!m_isConnected) return;

#ifndef __wasm__
    while (m_udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_udpSocket->receiveDatagram();
        QByteArray newdata = datagram.data();
        qDebug().noquote() << "# >>"
                           << QString("UDP:[%1][receive]").arg(m_deviceUdpIp)
                           << newdata.toHex();

        _HandleNewDataFromDevice(newdata);
    }
#endif
}

void MessageHandler::_HandleNewDataFromDevice(QByteArray newdata)
{
    m_rxBuffer.append(newdata);

    while(1) {
        quint8 h1(0), h2(0);
        if (m_rxBuffer.size() <= 1) {
            break;
        } else {
            h1 = static_cast<quint8>(m_rxBuffer[0]);
            h2 = static_cast<quint8>(m_rxBuffer[1]);
        }

        if (h1 != HEAD_P2_1 && h1 != HEAD_P3_1) {
            qDebug() << "bad pack" << m_rxBuffer.toHex();
            m_rxBuffer.remove(0, 1);
            continue;
        } else if (h2 != HEAD_P2_2 && h2 != HEAD_P3_2) {
            qDebug() << "bad pack" << m_rxBuffer.toHex();
            m_rxBuffer.remove(0, 2);
            continue;
        }

        HandleBytesState state(OK);
        if (h1 == HEAD_P2_1 && h2 == HEAD_P2_2) {
            state = m_protocol2->handleRawData(m_rxBuffer);
        } else if (h1 == HEAD_P3_1 && h2 == HEAD_P3_2) {
            state = m_protocol3->handleRawData(m_rxBuffer);
        }

        if (state == HandleBytesState::OK) {
        } else if (state == HandleBytesState::TOO_SHORT) {
            // todo： 这里要在思考一下, 有可能导致不必要的重发
            qDebug() << "too short" << m_rxBuffer.toHex();
            break;
        } else if (state == HandleBytesState::CHECKSUM_ERR) {
            qDebug() << "crc error" << m_rxBuffer.toHex();
            _ClearBuffer();
        }
    }
}

void MessageHandler::_ClearBuffer()
{
    // 清除下一个头之前的数据， 避免粘包
    int end(m_rxBuffer.size() - 1);
    for(int index(2); index < end; ++index) {
        quint8 h1(static_cast<quint8>(m_rxBuffer[index]));
        quint8 h2(static_cast<quint8>(m_rxBuffer[index+1]));
        if ((h1 == HEAD_P2_1 && h2 == HEAD_P2_2) || (h1 == HEAD_P3_1 && h2 == HEAD_P3_2)) {
            QByteArray data(m_rxBuffer.mid(0, index));
            m_rxBuffer.remove(0, index);
            return;
        }
    }
    m_rxBuffer.clear();
}

