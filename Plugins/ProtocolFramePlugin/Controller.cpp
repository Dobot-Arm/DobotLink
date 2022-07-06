#include "Controller.h"
#include "memory.h"
#include <QDebug>
#include <QEventLoop>

DeviceInfo::DeviceInfo() {
    isConnected = false;
    status = PORT_UNCONNECTED;
}

Controller *Controller::m_staticinstance = nullptr;

Controller::Controller(): QObject(nullptr) {
    m_portName = "";
    m_correctNum = 0;

    m_serialport = nullptr;

    m_timer.start(500);
    connect(&m_timer, &QTimer::timeout, this, &Controller::updataDeviceState);

    m_staticinstance = this;
    connect(m_staticinstance, SIGNAL(signal_readDataSerialPort(quint16 *, uint8_t *, quint16)), this, SLOT(slot_readDataSerialPort(quint16 *, uint8_t *, quint16)), Qt::BlockingQueuedConnection);
    connect(m_staticinstance, SIGNAL(signal_writeDataSerialPort(uint8_t *, quint16)), this, SLOT(slot_writeDataSerialPort(uint8_t *, quint16)));

}

Controller::~Controller() {
    m_timer.stop();
}

//返回可用的串口列表
QStringList Controller::getAvailablePortNameList() {
    QStringList portNameList;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        portNameList << info.portName();
    }

    return portNameList;
}

void Controller::initSearchInfo() {
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        DeviceInfo decInfo = m_devInfoMap.value(info.portName());
        decInfo.portName = info.portName();
        decInfo.description = info.description();
        m_devInfoMap.insert(info.portName(), decInfo);
    }
}

//真正从串口中读取数据
void Controller::slot_readDataSerialPort(quint16 *realLength, uint8_t *dataArray, quint16 dataLength) {
    if (m_serialport) {
        *realLength = m_serialport->read(reinterpret_cast<char *>(dataArray), static_cast<qint64>(dataLength));
        if (*realLength != 0) {
            QByteArray data(reinterpret_cast<char *>(dataArray), *realLength);
            qDebug() << "recv <--" << data.toHex() << "realLength" << *realLength ;
        }
    }
}

//真正向串口写数据
void Controller::slot_writeDataSerialPort(uint8_t *dataArray, quint16 dataLength) {
    if (m_serialport) {
        QByteArray data(reinterpret_cast<char *>(dataArray), dataLength);
        qDebug() << "send -->" << data.toHex().data();
        m_serialport->write(reinterpret_cast<char *>(dataArray), static_cast<qint64>(dataLength));
    }
}

void Controller::showError(QSerialPort::SerialPortError error) {
    qDebug() << "Serial:" << SerialError[error];
}

//注册的读函数
uint16_t Controller::readData(uint8_t *dataArray, uint16_t dataLength) {
    //获取数据dataArray,读取dataLength长度，返回真正长度
    uint16_t realLength = 0;
    emit m_staticinstance->signal_readDataSerialPort(&realLength, dataArray, dataLength);

    return realLength;
}

//注册的写函数
void Controller::writeData(uint8_t *dataArray, uint16_t dataLength) {
    //如果dataArray有数据，往下发
    emit m_staticinstance->signal_writeDataSerialPort(dataArray, dataLength);
}

//判断设备是否已连接
bool Controller::isDeviceConnected(const QString &portName) {
    if (m_devInfoMap.contains(portName)) {
        DeviceInfo info = m_devInfoMap.value(portName);
        if (info.isConnected) {
            return true;
        }
        qDebug() << "Device status:" << info.status;
    }
    return false;
}

//心跳检测判断设备连接状态
void Controller::updataDeviceState() {
    QStringList serialPortList;
    serialPortList = getAvailablePortNameList();
    foreach (auto item, m_devInfoMap.keys()) {
        if (!serialPortList.contains(item)) {
            m_devInfoMap.remove(item);
        }
    }
    initSearchInfo();

    if (m_correctNum != 0 && m_portName != "" && !serialPortList.contains(m_portName)) {
        qDebug() << "Mooz device:" << m_portName << "lost connection.";
        disconnectDevice();
    }

}

//搜索
QJsonObject Controller::SearchPort(const ProtocolFramePacket &packet) {
    QJsonObject resObj;
    QJsonArray deviceArray;
    QStringList portList = getAvailablePortNameList();
    initSearchInfo();

    ProtocolFrameResPacket resPacket(packet.id, packet.wsPort);
    if (portList.size() <= 0) {
        resPacket.setErrorObj(10, "No Device was found !");
        return resPacket.getResultObj();
    }
    for (int i = 0; i < portList.count(); i++) {
        QString portName = portList.at(i);
        DeviceInfo info = m_devInfoMap.value(portName);
        QJsonObject deviceInfoObj;
        deviceInfoObj.insert("description", info.description);
        deviceInfoObj.insert("portName", portName);
        deviceInfoObj.insert("status", info.status);
        deviceArray.append(deviceInfoObj);
    }
    resObj.insert("array", deviceArray);

    if (!deviceArray.isEmpty()) {
        resPacket.setResultObj(resObj);
    }
    return resPacket.getResultObj();
}

// 连接
QJsonObject Controller::ConnectPort(const ProtocolFramePacket &packet) {
    QJsonObject resObj;
    ProtocolFrameResPacket resPacket(packet.id, packet.wsPort);
    QStringList portList = getAvailablePortNameList();
    initSearchInfo();

    //如果没有给出串口的名字
    if (packet.portName == "") {
        resPacket.setErrorObj(11, QString("the api need a param:\"portName\" !!!"));
        return resPacket.getResultObj();
    }

    //串口不存在
    if (!portList.contains(packet.portName)) {
        resPacket.setErrorObj(11, QString("the serial port is not available!!!"));
        return resPacket.getResultObj();
    }

    //如果已连接就不用再连接了
    if (isDeviceConnected(packet.portName)) {
        resPacket.setErrorObj(16, QString("%1 connect now !!!").arg(packet.portName));
        return resPacket.getResultObj();
    }

    //有新的串口连接，提示用户使用DisconnectPort断开之前的连接 //或者先释放该连接并改变状态
    if (m_portName != "") {
        resPacket.setErrorObj(14, QString("Here is portName %1 already connected!").arg(m_portName));
        return resPacket.getResultObj();
    }

    DeviceInfo info = m_devInfoMap.value(packet.portName);
    QSerialPort *serialPort = new QSerialPort;
    serialPort->setPortName(packet.portName);
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    m_serialport = serialPort;

//    connect(m_serialport,SIGNAL(errorOccurred(QSerialPort::SerialPortError)),this,SLOT(showError(QSerialPort::SerialPortError)));
    //另外这五种状态需要转换connected,unconnected,nofound,occupied,unkown
    if (m_serialport->open(QIODevice::ReadWrite)) {
        m_correctNum = 1;
        m_portName = packet.portName;
        info.isConnected = true;
        info.status = PORT_CONNECTED;
        m_devInfoMap.insert(packet.portName, info);
        resObj.insert("info", QString("%1 connect success.").arg(packet.portName));
        resPacket.setResultObj(resObj);
    } else {
        m_serialport->close();
        m_serialport->deleteLater();
        m_serialport = nullptr;
        qDebug() << "QSerialport errorOccurred!";
        info.isConnected = false;
        info.status = PORT_UNCONNECTED;
        m_devInfoMap.insert(packet.portName, info);
        resPacket.setErrorObj(13, QString("QSerialport errorOccurred: "));
    }

    return resPacket.getResultObj();
}

// 断开连接
QJsonObject Controller::DisconnectPort(const ProtocolFramePacket &packet) {
    QJsonObject resObj;
    ProtocolFrameResPacket resPacket(packet.id, packet.wsPort);

    //如果没有连接上串口,直接返回结果
    if (packet.portName == "") {
        qDebug() << "missing a param:portName";
        resPacket.setErrorObj(14, QString("missing a param:portName"));
        return  resPacket.getResultObj();
    }

    if (packet.portName != m_portName) {
        qDebug() << QString("current connection portname is %1").arg(m_portName);
        resPacket.setErrorObj(14, QString("current connection portname is %1").arg(m_portName));
        return  resPacket.getResultObj();
    }

    DeviceInfo info = m_devInfoMap.value(packet.portName);

    if (info.isConnected == false) {
        info.status = PORT_UNCONNECTED;
        m_devInfoMap.insert(packet.portName, info);
        resPacket.setErrorObj(15, QString("%1 currentDevice already disconnected !!!").arg(packet.portName));
        return resPacket.getResultObj();
    }

    disconnectDevice();
    info.isConnected = false;
    info.status = PORT_UNCONNECTED;
    m_devInfoMap.insert(packet.portName, info);
    resObj.insert("info", QString("%1 disconnect success.").arg(packet.portName));
    resPacket.setResultObj(resObj);

    return  resPacket.getResultObj();
}

void Controller::disconnectDevice() {
    if (m_serialport) {
        m_serialport->close();
        m_serialport->deleteLater();
        m_serialport = nullptr;
    }
    m_correctNum = 0;
    m_portName = "";
}

const QString &Controller::getPortName() {
    return m_portName;
}
