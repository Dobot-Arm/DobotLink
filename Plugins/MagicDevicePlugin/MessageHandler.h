#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QJsonObject>
#include <QHash>
#include <QQueue>
#include <QLinkedList>
#include <QObject>
#include "Types.h"
#include <functional>

//#define __wasm__

class QStringList;
class QTimer;
class QIODevice;
#ifndef __wasm__
class QUdpSocket;
#endif
class DProtocol2;
class DProtocol3;
class MessageHandler : public QObject
{
    Q_OBJECT

public:
    enum ProtocolType {DEVICE_MAGIC_PROTOCOL, DEVICE_M1_PROTOCOL};

    explicit MessageHandler(QObject *parent = nullptr);
    ~MessageHandler();

    void setProtocolType(ProtocolType type);

    void setPortName(QString portName);
    QString getPortName();

    void setHostIpAddress(QString ip);
    void setUdpIpAddress(QString ip);
    QString getUdpIpAddress();
    void setDeviceUdpPort(quint16 port);

    bool connectDevice(quint64 id, QString &devType, QString &fwType, QString &fwVer);
    void connectDevice(quint64 id, const std::function<void(QString devType, QString fwType, QString fwVer)>& callback);
    void disconnectDevice(quint64 id = 0);
    void setCommuTimeout(quint64 id=0, quint8 resendTimesMax=3, quint32 commTimeout=3000);
    bool isConnected() const;

    bool sendProtocolData(quint64 id, DeviceType devType, QJsonObject &obj, bool needReply=true);

signals:
    void errorOccurred_signal(int code, quint64 id = 0);  // 其他错误信息(CommunicationError)
    void serialErrorOccuerd_signal(int code, QString errorString); //串口报错

    // 收到的信息
    void recviceP2Data_signal(QJsonObject message);
    void recviceP3Data_signal(QJsonObject obj);

private:
    ProtocolType m_ProtocolType;

    /* SerialPort / UDP */
    bool m_isSerialConnection;

    bool m_isConnected;

    /* SerialPort */
    QIODevice *m_ioDevice;

#ifndef __wasm__
    /* UDP socket */
    QUdpSocket *m_udpSocket;
#endif
    QString m_localhostIp;
    QString m_deviceUdpIp;
    quint16 m_deviceUdpPort;
    bool m_isConnecting;

    /* Buffer: in case get less packet */
    QByteArray m_rxBuffer;

    /* Timer */
    QTimer *m_timerDeviceKeepAlive;

    DProtocol2 *m_protocol2;
    DProtocol3 *m_protocol3;

    void _SerialPortInit();
    void _UdpSocketInit();
    void _TimerInit();
    bool _bindLocalHostIp();
    bool _checkMarlin(QString &fwVer);
    void _checkMarlin(const std::function<void(QString fwVer, bool bIsMarlin)>& callback);

    // 接收到设备数据相关函数
    void _HandleNewDataFromDevice(QByteArray newdata);
    inline void _ClearBuffer();

private slots:
    void onSerialReadyRead_slot();
    void onReadDatagram_slot();
    void sendRawData_slot(QByteArray data);
};

#endif // MESSAGEHANDLER_H
