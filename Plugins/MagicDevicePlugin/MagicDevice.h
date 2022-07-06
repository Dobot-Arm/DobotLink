#ifndef MAGICDEVICE_H
#define MAGICDEVICE_H

//#define __wasm__

#include <QtCore/qglobal.h>
#include <QObject>
#include <QMap>
#include <QList>
#include <QJsonObject>
#include "Types.h"

QT_BEGIN_NAMESPACE

class MessagePacket
{
public:
    MessagePacket(QString cmd = QString(), quint64 id = 0);

    quint16 wsPort;

    quint64 id;          // *request id
    bool isEmpty();

    void setSourceObj(const QJsonObject obj);
    QJsonObject getSourceObj();

    void setCommand(QString cmd);
    QString getCmdStr();

    void setIsQueue(bool isQueued);
    bool getIsQueue();

    void setParams(QJsonObject params);
    QJsonObject getParamsObj();

    void setSlaveID(quint8 slaveid);
    quint8 getSlaveID();

    void setTargetType(DeviceType type);
    void setTargetType(QString typeStr, quint8 slaveIndex = 0);
    DeviceType getTargetType();
    QString getTargetTypeStr();

    void setWaitForFinishEnable(bool enable, int timeout = 10000);
    bool getIsWaitForFinish();
    int getTimeoutValue();

    QJsonObject getPacketObj();

    /* just for INTERNAL ONLY */
    void setPrivatePacket(bool isPrivate);

private:
    static void initDeviceTypeStringMap();

    bool m_isWaitForFinish;
    int m_timeout;

    /* (m_cmd + protocal) -> m_cmdid -> m_rw -> m_rwType */

    QString m_cmd;          //*Command str
    int m_cmdid;            // Command id
    int m_rw;               // (0/1)
    QString m_rwType;       // (Read/Write)

    quint8 m_slaveID;       // slave device id:(0~3)

    bool m_isQueued;        //*Queue Command:(true/false)
    QJsonObject m_params;   //*data

    DeviceType m_targetType;

    bool m_isPrivate;

    QJsonObject m_sourceObj;
};


/* Magic Device */
typedef QList<MessagePacket> DPacketList;

class MagicDevicePrivate;
class MagicDevice : public QObject
{
    Q_OBJECT
public:
    explicit MagicDevice(QObject *parent = nullptr);
    MagicDevice(DeviceType type = DEVICE_UNKNOWN, QObject *parent = nullptr);
    virtual ~MagicDevice();

    static QString getDeviceTypeToString(DeviceType type);
    static DeviceType getDeviceStringToType(QString type);

    static QString getProtocolVersion();
    static QStringList getCommandList(QString deviceType);
    static QStringList getDeviceList(bool isFiltered = false);

    /* Device type */
    void setDeviceType(DeviceType type);
    DeviceType getDeviceType();

    /* PortName */
    void setPortName(QString portName);
    QString getPortName();

    /* Websocket port */
    void setWebsocketPort(quint16 wsport);
    quint16 getWebsocketPort();

    /* UDPsocket */
    void setHostIpAddress(QString ip);
    void setDeviceIpAddress(QString ip);

    /* Connection: unconnected/connected/occupied/unknown */
    void connectDevice(quint16 wsPort, quint64 id);
    void disConnectDevice(quint64 requestid = 0);
    QString getConnectStatus();

    /* Communication timeout Setting */
    void setCommuTimeout(quint64 requestid, int ms = 3000);

    /* Send Command */
    bool sendCommand(MessagePacket &packet, bool connectCheck = true);
    bool sendCommandList(quint64 id, DPacketList packetList, bool enCheck = true);

    void stopQueueCmdTimer();

signals:
    void onResultMessage_signal(quint64 id, QString cmd, int res = 0, QJsonValue params = QJsonValue());
    void onMagicianGoResultMessage_signal(QJsonObject obj);
    void serialErrorOccurred_signal(int code, QString errorString);

private:
    Q_DISABLE_COPY(MagicDevice)
    MagicDevicePrivate * const Dptr;
    Q_DECLARE_PRIVATE_D(Dptr, MagicDevice)

    bool _checkP2Firmware(quint16 wsPort, quint64 id, QString &devType, QString &fwVer);
    bool _checkP3Firmware(quint16 wsPort, quint64 id, QString &devType, QString &fwVer);
};


Q_DECLARE_INTERFACE(MessagePacket, "MessagePacket")
Q_DECLARE_INTERFACE(MagicDevice, "MagicDevice")

QT_END_NAMESPACE

#endif // MAGICDEVICE_H
