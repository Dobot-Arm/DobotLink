#ifndef Controller_H
#define Controller_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>
#include<QMap>
#include <QList>
#include "ProtocolFramePacket.h"
#include "DobotType.h"
#include "MoozApi.h"
extern "C" {
#include "DobotV3Func.h"
}

class DeviceInfo {
  public:
    DeviceInfo();

    bool isConnected;
    QString status;

    QString portName;
    QString description;
};

class Controller : public QObject {
    Q_OBJECT

  public:
    Controller();
    ~Controller();

    QStringList getAvailablePortNameList();
    void initSearchInfo();

    static uint16_t readData(uint8_t *dataArray, uint16_t dataLength);
    static void writeData(uint8_t *dataArray, uint16_t dataLength);

    bool setSendParm(uint8_t repeatNum, uint32_t timeOut);
    int sendCmd(PDobotV3Format_Packet pPacket, uint16_t payloadLen);

    bool isDeviceConnected(const QString &portName);
    void updataDeviceState();

    QJsonObject SearchPort(const ProtocolFramePacket &packet);

    QJsonObject ConnectPort(const ProtocolFramePacket &packet);
    QJsonObject DisconnectPort(const ProtocolFramePacket &packet);
    void disconnectDevice();

    const QString &getPortName();

  public slots:
    void slot_readDataSerialPort(quint16 *realLength, uint8_t *dataArray, quint16 dataLength);
    void slot_writeDataSerialPort(uint8_t *dataArray, quint16 dataLength);
    void showError(QSerialPort::SerialPortError error);

  signals:
    void signal_readDataSerialPort(quint16 *realLength, uint8_t *dataArray, quint16 dataLength);
    void signal_writeDataSerialPort(uint8_t *dataArray, quint16 dataLength);

  private:
    QTimer m_timer;
    QString m_portName;
    QSerialPort *m_serialport;
    static Controller *m_staticinstance;
    int m_correctNum;
    QMap<QString, DeviceInfo> m_devInfoMap;  //记录每个设备信息<设备名，设备结构体信息>

};
#endif // Controller_H
