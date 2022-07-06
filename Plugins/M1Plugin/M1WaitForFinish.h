#ifndef M1WAITFORFINISH_H
#define M1WAITFORFINISH_H

#include <QObject>
#include <QTimer>
#include "M1Packet.h"

class M1WaitForFinish : public QObject
{
    Q_OBJECT
public:
    explicit M1WaitForFinish(int devid, quint64 targetID, M1Packet packet, QObject *parent = nullptr);
    ~M1WaitForFinish();

    double m_cmdID;
    M1Packet m_packet;

    void startWaiting(int minMs, int maxMs);
    void stopWaiting();

signals:
    void finish_signal(int res);

private:
    int m_devid;
    quint64 m_targetid;

    QTimer *minTimer;
    QTimer *maxTimer;
    QTimer *checkTimer;

private slots:
    void onMinTimeOut_slot();
    void onMaxTimeOut_slot();
    void onCheckTimeOut_slot();
};

#endif // MWAITFORFINISH_H
