#ifndef EM_SERIALPORT
#define EM_SERIALPORT

#include <QList>
#include <QIODevice>
#include <QTimer>

#include <atomic>

#include "Config.h"

class EmSerialPortInfo
{
public:
    static QList<EmSerialPortInfo> availablePorts();
    static bool requestPorts();

    QString description() const {return m_description;};
    QString portName() const {return m_portName;};
    quint16 productIdentifier(){return m_productIdentifier;};
    quint16 vendorIdentifier(){return m_vendorIdentifier;};

private:
    EmSerialPortInfo():
        m_portName(""),
        m_description(""),
        m_productIdentifier(0),
        m_vendorIdentifier(0)
    {};

    QString m_portName;
    QString m_description;
    quint16 m_productIdentifier;
    quint16 m_vendorIdentifier;
};

class EmSerialPort: public QIODevice
{
    Q_OBJECT
public:
    enum SerialPortError {
        NoError,
        OpenError,
        CloseError,
        ReadError,
        WriteError,
        HasNotOpen
    };

    EmSerialPort(QObject *parent=nullptr);
    ~EmSerialPort();

    virtual bool open(QIODevice::OpenMode mode) override;
    virtual void close() override;

    void setPortName(const QString &name);
    QString portName() const;

signals:
    void errorOccurred(EmSerialPort::SerialPortError error);

protected:
    virtual qint64 writeData(const char *data, qint64 maxSize) override;
    virtual qint64 readData(char *data, qint64 maxSize) override;

private:
    QString m_portName;
    uint16_t m_productId;
    uint16_t m_vendorId;
    SerialPortError m_error;

    std::atomic_bool m_bIsOpenDevice;

    QTimer *m_timer;

    qint64 m_readRawBufferSize;
    uint8_t *m_readRawBuffer;

    bool _handleWebError(uint8_t *errBtyes, uint8_t errLen, SerialPortError error);
    void portOpenInner();
    bool isPortOpenInner();
    void closePortInner();
    qint64 writePortInner(const char *data, qint64 maxSize);
};
#endif // EM_SERIALPORT
