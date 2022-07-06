#include "EmSerialPort.h"
#include <string>
#include <iostream>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <QDebug>
#include <QCoreApplication>

EM_ASYNC_JS(
    void, WebRequestPorts, (uint8_t *errBytes, uint8_t *errLen),
    {
        try {
            await navigator.serial.requestPort();
        } catch (err) {
            const _errLen = lengthBytesUTF8(err.message) + 1;
            stringToUTF8(err.message, errBytes, _errLen);
            Module.HEAPU8[errLen] = _errLen;
        }
    });

EM_ASYNC_JS(
    void, WebAvailablePorts, (uint16_t *products, uint16_t *vendors, uint8_t *num, uint8_t maxNum, uint8_t *errBytes, uint8_t *errLen),
    {
        let _num = 0;
        self.searchedPorts = {};
        try {
            const ports = await navigator.serial.getPorts();
            for (let i = 0; i < ports.length; ++i) {
                const port = ports[i];
                let productId = 0;
                let vendorId = 0;
                if (port.getInfo !== undefined) {
                    const info = ports[i].getInfo();
                    productId = info["usbProductId"];
                    vendorId = info["usbVendorId"];
                } else {
                    // 旧版本的chrome没有getInfo方法
                    if (!self.serialPortIdFlag) self.serialPortIdFlag = 1;

                    productId = self.serialPortIdFlag;
                    vendorId = self.serialPortIdFlag;

                    self.serialPortIdFlag += 1;
                }

                if (_num <= maxNum) {
                    self.searchedPorts[`${productId},${vendorId}`] = port;
                    Module.HEAPU16[(products >> 1) + _num] = productId;
                    Module.HEAPU16[(vendors >> 1) + _num] = vendorId;
                    _num += 1;
                } else {
                    throw new Error("num > EMSERIALPORT_SEARCH_BUFFER_SIZE");
                }
            }

            Module.HEAPU8[num] = _num;
        } catch (err) {
            const _errLen = lengthBytesUTF8(err.message) + 1;
            stringToUTF8(err.message, errBytes, _errLen);
            Module.HEAPU8[errLen] = _errLen;
        }
    });

EM_ASYNC_JS(
    void, WebSerialPortOpen, (uint16_t product, uint16_t vendor, uint8_t *errBytes, uint8_t *errLen),
    {
        try {
            if (self.searchedPorts === undefined) {
                throw new Error("self.searchedPorts undefined, invail port");
            }

            let port = undefined;
            const portName = `${product},${vendor}`;
            const keys = Object.keys(self.searchedPorts);
            for (let i = 0; i < keys.length; ++i) {
                const _portName = keys[i];
                if (_portName === portName) {
                    port = self.searchedPorts[_portName];
                    break;
                }
            }

            if (port === undefined) {
                throw new Error("cannot find port, invail port");
            }

            await port.open({baudRate : 115200});
            if (self.SERIALPORT_POENED_READER === undefined)
                self.SERIALPORT_POENED_READER = {};
            if (self.SERIALPORT_POENED_WRITER === undefined)
                self.SERIALPORT_POENED_WRITER = {};

            self.SERIALPORT_POENED_READER[portName] = port.readable.getReader();
            self.SERIALPORT_POENED_WRITER[portName] = port.writable.getWriter();
        } catch (err) {
            const _errLen = lengthBytesUTF8(err.message) + 1;
            stringToUTF8(err.message, errBytes, _errLen);
            Module.HEAPU8[errLen] = _errLen;
        }
    });

EM_ASYNC_JS(
    bool, WebSerialPortClose, (uint16_t product, uint16_t vendor, uint8_t *errBytes, uint8_t *errLen),
    {
        const portName = `${product},${vendor}`;
        reader = self.SERIALPORT_POENED_READER[portName];
        reader.cancel();
        reader.releaseLock();
        delete self.SERIALPORT_POENED_READER[portName];

        writer = self.SERIALPORT_POENED_WRITER[portName];
        writer.releaseLock();
        delete self.SERIALPORT_POENED_WRITER[portName];

        let port = undefined;
        const ports = await navigator.serial.getPorts();
        for (let i = 0; i < ports.length; ++i) {
            const info = ports[i].getInfo();
            const productId = info["usbProductId"];
            const vendorId = info["usbVendorId"];

            if (`${productId},${vendorId}` === portName) {
                port = ports[i];
            }
        }

        if (port == undefined) {
            console.warn("port had been closed: ", portName);
        } else {
            try {
                await port.close();
            } catch (err) {
                const _errLen = lengthBytesUTF8(err.message) + 1;
                stringToUTF8(err.message, errBytes, _errLen);
                Module.HEAPU8[errLen] = _errLen;
            }
        }
    });

EM_ASYNC_JS(
    bool, WebSerialPortIsOpen, (uint16_t product, uint16_t vendor),
    {
        const portName = `${product},${vendor}`;
        if (self.SERIALPORT_POENED_READER !== undefined &&
            self.SERIALPORT_POENED_READER[portName] !== undefined &&
            self.SERIALPORT_POENED_WRITER !== undefined &&
            self.SERIALPORT_POENED_WRITER[portName] !== undefined) {
            return true;
        }
        return false;
    });

EM_ASYNC_JS(
    void, WebSerialPortRead, (uint16_t product, uint16_t vendor, uint8_t *data, uint8_t *dataLen, int maxLen, uint8_t *errBytes, uint8_t *errLen, uint8_t *isFinish),
    {
        const portName = `${product},${vendor}`;
        let reader;
        reader = self.SERIALPORT_POENED_READER[portName];

        reader.read().then(({value, done}) => {
            if (done) { // stream关闭了
                console.warn("serial port has been closed while reading.");
                const _errLen = lengthBytesUTF8("serial port has been closed while reading.") + 1;
                stringToUTF8(err.message, errBytes, _errLen);
                Module.HEAPU8[errLen] = _errLen;
            }else{
                if (value !== undefined) {
                    if (value.length > maxLen) {
                        throw new Error("recived data over size");
                    }
                    Module.HEAPU8[dataLen] = value.length;
                    Module.HEAPU8.set(value, data);
                }
            }

            Module.HEAPU8[isFinish] = 1;
        }).catch((err) => {
            const _errLen = lengthBytesUTF8(err.message) + 1;
            stringToUTF8(err.message, errBytes, _errLen);
            Module.HEAPU8[errLen] = _errLen;

            Module.HEAPU8[isFinish] = 1;
        });
    });

EM_ASYNC_JS(
    void, WebSerialPortWrite, (uint16_t product, uint16_t vendor, uint8_t *data, int maxLen, uint8_t *errBytes, uint8_t *errLen),
    {
        const portName = `${product},${vendor}`;
        let reader;
        writer = self.SERIALPORT_POENED_WRITER[portName];

        // 需要将ShareArrayBuffer的数据复制到ArrayBuffer, 所以执行了两次TypeBuffer的创建
        let bufData = new Uint8Array(new Uint8Array(Module.HEAPU8.buffer, data, maxLen));
        try {
            await writer.write(bufData);
        } catch (err) {
            const _errLen = lengthBytesUTF8(err.message) + 1;
            stringToUTF8(err.message, errBytes, _errLen);
            Module.HEAPU8[errLen] = _errLen;
        }
    });

QList<EmSerialPortInfo> EmSerialPortInfo::availablePorts()
{
    QList<EmSerialPortInfo> infos;

    uint8_t num = 0;
    uint16_t productIds[EMSERIALPORT_SEARCH_BUFFER_SIZE] = {0};
    uint16_t vendorIds[EMSERIALPORT_SEARCH_BUFFER_SIZE] = {0};

    uint8_t errBytes[EMSERIALPORT_ERROR_BUFFER_SIZE];
    uint8_t errLen(0);
    WebAvailablePorts(productIds, vendorIds, &num, EMSERIALPORT_SEARCH_BUFFER_SIZE, (uint8_t*)errBytes, &errLen);
    if (errLen > 0) {
        qWarning() << "EmSerialPortInfo::availablePorts-->"<< QLatin1String((char*)errBytes, errLen - 1);
    }

    for (int i = 0; i < num; ++i) {
        EmSerialPortInfo info;
        info.m_portName = QString("%1,%2").arg(productIds[i]).arg(vendorIds[i]);
        info.m_productIdentifier = productIds[i];
        info.m_vendorIdentifier = vendorIds[i];
        if (info.m_productIdentifier == 38912 && info.m_vendorIdentifier == 61525) {
            info.m_description = "(Web) Pyboard Virtual Comm Port in FS Mode";
        } else if (info.m_productIdentifier == 60000 && info.m_vendorIdentifier == 4292) {
            info.m_description = "(Web) CP210 USB to UART Bridge Controller";
        } else {
            info.m_description = "(Web) Unknow Serial Port";
        }

        infos << info;
    }

    return infos;
}

bool EmSerialPortInfo::requestPorts()
{
    uint8_t errBytes[EMSERIALPORT_ERROR_BUFFER_SIZE];
    uint8_t errLen(0);
    WebRequestPorts(errBytes, &errLen);
    if (errLen > 0)
    {
        qWarning() << QLatin1String((char *)errBytes, errLen - 1);
        return false;
    }
    return true;
}

void EmSerialPort::setPortName(const QString &name)
{
    m_portName = name;
    QStringList temp = name.split(",");
    if (temp.size() >= 2)
    {
        m_productId = temp[0].toInt();
        m_vendorId = temp[1].toInt();
    }
}

QString EmSerialPort::portName() const
{
    return m_portName;
}

EmSerialPort::EmSerialPort(QObject *parent) : QIODevice(parent),
                                              m_portName(""),
                                              m_productId(0),
                                              m_vendorId(0),
                                              m_error(SerialPortError::NoError),
                                              m_timer(new QTimer(this)),
                                              m_readRawBufferSize(0),
                                              m_readRawBuffer(new uint8_t[EMSERIALPORT_READ_RAW_BUFFER_ZISE])
{
    qDebug()<<"EmSerialPort::EmSerialPort create";
    m_bIsOpenDevice.store(false);
    m_timer->setInterval(WEB_SERIALPORT_READ_INTERVAL);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, [=]() {
        if(isOpen()) {
            quint64 readMaxSize = EMSERIALPORT_READ_RAW_BUFFER_ZISE - m_readRawBufferSize;
            uint8_t isFinish(0), chuckLen(0), errLen(0);
            uint8_t chuck[readMaxSize], errBytes[EMSERIALPORT_ERROR_BUFFER_SIZE];
            WebSerialPortRead(m_productId, m_vendorId, chuck, &chuckLen, readMaxSize, errBytes, &errLen, &isFinish);

            // 注意：
            //      1. WebSerialPortRead运行在Web Worker中，暂时不能注册函数到Web Worker上下文
            //      2. Web Worker的postMessage被Emscripten封印
            //      3. Web Worker的栈是隔离的，意味着c的函数指针不能传递到混编函数的作用域中
            //      4. await reader.read()会block qt的时间循环，所以不能在WebSerialPortRead等待数据
            //
            // 综上： 在qt层用事件循环死等
            while (!isFinish) {
                QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, QT_EVENT_PROCESS_TIME);
            }

            if (m_bIsOpenDevice.load())
            {//当主动disconnectDobot时，也会发射这个readerror错误信息，前端会同时收到2条，不合适，所以主动关闭串口时，不发射。
                if (!_handleWebError(errBytes, errLen, SerialPortError::ReadError) && chuckLen > 0) {
                    memcpy(m_readRawBuffer + m_readRawBufferSize, chuck, chuckLen);
                    m_readRawBufferSize += chuckLen;
                    emit readyRead();
                }
            }
            m_timer->start();
        } else {
            m_timer->start();
        }
    });
    m_timer->start();
}

qint64 EmSerialPort::readData(char *data, qint64 maxSize)
{
    if (!data || !maxSize) {
        qWarning() << "EmSerialPort::readData" << "invaild params";
        return 0;
    }

    if (!m_readRawBufferSize) return 0;

    if (!isOpen()) {
        setErrorString(QString("port(%1) has not opened.").arg(m_portName));
        m_error = SerialPortError::HasNotOpen;
        emit errorOccurred(SerialPortError::HasNotOpen);
        return 0;
    }

    qint64 cpySize = maxSize >= m_readRawBufferSize ? m_readRawBufferSize : maxSize;
    memcpy(data, m_readRawBuffer, cpySize);
    qint64 leftSize = m_readRawBufferSize - cpySize;
    memmove(m_readRawBuffer, m_readRawBuffer+cpySize, leftSize);
    m_readRawBufferSize = leftSize;

    return cpySize;
}

EmSerialPort::~EmSerialPort()
{
    delete[] m_readRawBuffer;
}

bool EmSerialPort::open(OpenMode mode)
{
    // web端没有模式设置
    Q_UNUSED(mode);
    if (isOpen()) return true;
    m_readRawBufferSize = 0;

    uint8_t errLen(0);
    uint8_t errBytes[EMSERIALPORT_ERROR_BUFFER_SIZE];
    WebSerialPortOpen(m_productId, m_vendorId, errBytes, &errLen);
    _handleWebError(errBytes, errLen, SerialPortError::OpenError);

    // 根据web端的实际情况设置Mode，基类会用到这个mode
    bool res = WebSerialPortIsOpen(m_productId, m_vendorId);
    if (res) {
        m_bIsOpenDevice.store(true);
        setOpenMode(QIODevice::ReadWrite);
    }else{
        setOpenMode(QIODevice::NotOpen);
    }
    return res;
}

qint64 EmSerialPort::writeData(const char *data, qint64 maxSize)
{
    if (data == nullptr || maxSize == 0) {
        qWarning() << "EmSerialPort::writeData" << "invaild params";
        return 0;
    }

    if (!isOpen()) {
        setErrorString(QString("port(%1) has not opened.").arg(m_portName));
        m_error = SerialPortError::HasNotOpen;
        emit errorOccurred(SerialPortError::HasNotOpen);
        return 0;
    }

    uint8_t errLen(0);
    uint8_t errBytes[EMSERIALPORT_ERROR_BUFFER_SIZE];

    WebSerialPortWrite(m_productId, m_vendorId, (uint8_t *)data, maxSize, errBytes, &errLen);
    if (_handleWebError(errBytes, errLen, SerialPortError::WriteError)) {
        return 0;
    }

    return maxSize;
}

void EmSerialPort::close()
{
    if (m_bIsOpenDevice.load())
    {
        m_bIsOpenDevice.store(false);
        uint8_t errLen(0);
        uint8_t errBytes[EMSERIALPORT_ERROR_BUFFER_SIZE];
        WebSerialPortClose(m_productId, m_vendorId, errBytes, &errLen);

        //做close操作时，不要发射信号了，QSerialPort和QSerialPortPrivate源码close时也没有发射信号
        //_handleWebError(errBytes, errLen, SerialPortError::CloseError);
        if (errLen>0)
        {
            QString errStr = QLatin1String((char*)errBytes, errLen - 1);
            qDebug()<<"EmSerialPort::close error:"<<errStr;
        }

        // 根据web端的实际情况设置Mode，基类会用到这个mode
        //WebSerialPortIsOpen(m_productId, m_vendorId);
        //无论web的串口是否真实打开，既然调用close了，就改变打开状态。
        setOpenMode(QIODevice::NotOpen);
    }
}

bool EmSerialPort::_handleWebError(uint8_t *errBytes, uint8_t errLen, SerialPortError error)
{
    if (errLen <= 0) {
        return false;
    }

    char *_errBytes = (char *)errBytes;

    QString errStr = QLatin1String(_errBytes, errLen - 1);
    setErrorString(errStr);
    m_error = error;
    emit errorOccurred(m_error);
    return true;
}
