#pragma once

#include <QByteArray>
#include <QIODevice>

class CVirtualSerialPort : public QIODevice
{
    friend class CVirtualSerialPortManager;
    Q_OBJECT
public:
    explicit CVirtualSerialPort(QObject *parent = nullptr);
    virtual ~CVirtualSerialPort();
	
    void setPortName(const QString &name);
    QString portName() const;

    bool open(QIODevice::OpenMode mode) override;
    void close() override;
	
    //参数同QSerialPort
    bool setBaudRate(qint32 baudRate, int directions = 3);
    bool setDataBits(int dataBits);
    bool setParity(int parity);
    bool setStopBits(int stopBits);
    bool setFlowControl(int flowControl);
	
Q_SIGNALS:
    //参数同QSerialPort
    void errorOccurred(int error);
	
    //private signal
    void signalInnerReadData(QByteArray data, QPrivateSignal);
    void signalInnerWriteData(QByteArray data, QPrivateSignal);

protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

private slots:
    void slotInnerReadData(QByteArray data);

private:
    QString m_strPortName;
    QByteArray m_readBuffer;
};
