#ifndef DPROTOCOLINTERFACE_H
#define DPROTOCOLINTERFACE_H

#include <QObject>
#include <QJsonObject>

enum HandleBytesState {OK, TOO_SHORT, CHECKSUM_ERR};

class DProtocolInterface: public QObject
{
    Q_OBJECT
public:
    DProtocolInterface(QObject *parent = nullptr);
    virtual ~DProtocolInterface();
    virtual bool sendData(quint64 id, const QJsonObject &obj, bool needReply=true) = 0;
    virtual HandleBytesState handleRawData(QByteArray &rawData) = 0;
    virtual void stop() = 0;
    virtual void start() = 0;
    virtual void setCommPars(quint8 resendTimesMax, quint32 commTimeout) = 0;

signals:
    void recviceData_signal(QJsonObject obj);
    void sendRawData_signal(QByteArray byteArry);
    void errorOccurred_signal(int code, quint64 id = 0);
};

#endif // DPROTOCOLINTERFACE_H
