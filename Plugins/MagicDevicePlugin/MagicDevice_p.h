#ifndef MAGICDEVICE_P_H
#define MAGICDEVICE_P_H

#include <QObject>
#include <QJsonObject>
#include <QMap>
#include <QList>

#include "ActionTimerManager.h"

enum ConnectStatus {UNCONNECTED, CONNECTED, OCCUPIED, UNKNOWN};

class MessageHandler;
class MessagePacket;
class MagicDevice;
class MagicDevicePrivate : public QObject
{
    Q_OBJECT
public:
    MagicDevicePrivate(MagicDevice *parent);
    virtual ~MagicDevicePrivate();

    static QMap<int, QString> DeviceTypeMap;
    static QMap<int, QString> initDeviceTypeStringMap();
    static QString checkIsIpAddress(QString ip);

    quint16 m_wsport;
    int m_deviceType;

    float poseX, poseY, poseZ, poseR;
    ConnectStatus m_status;

    typedef QList<MessagePacket> DPacketList;
    QMap<quint64, MessagePacket> m_RequestMap;
    QMap<quint64, DPacketList> m_PacketListMap;

    MessageHandler *m_MessageHandler;

    ActionTimerManager *m_actionTimerManager;

private:
    MagicDevice * const q_ptr;
    Q_DISABLE_COPY(MagicDevicePrivate)
    Q_DECLARE_PUBLIC(MagicDevice)

private slots:
    void checkCurrentIndex_slot();
    void errorOccurred_slot(int code, quint64 id);
    void recviceP2Data_slot(QJsonObject message);
    void handleActionFinish_slot(quint64 id, ActionTimerManager::FinishType type, QString targetType);
    /* send to MagicDevicePlugin */
    void recviceP3Data_slot(QJsonObject obj);
    /* 串口错误*/
    void serialErrorOccurred_slot(int code, QString errorString);
};

#endif // MAGICDEVICE_P_H
