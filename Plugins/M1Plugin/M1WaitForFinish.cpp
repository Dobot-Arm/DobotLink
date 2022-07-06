#include "M1WaitForFinish.h"

#include <QDebug>
#include "DobotDll.h"

M1WaitForFinish::M1WaitForFinish(int devid, quint64 targetID, M1Packet packet, QObject *parent) : QObject(parent),
    m_packet(packet),
    m_devid(devid),
    m_targetid(targetID)
{
    minTimer = new QTimer(this);
    minTimer->setSingleShot(true);
    connect(minTimer, &QTimer::timeout, this, &M1WaitForFinish::onMinTimeOut_slot);

    maxTimer = new QTimer(this);
    maxTimer->setSingleShot(true);
    connect(maxTimer, &QTimer::timeout, this, &M1WaitForFinish::onMaxTimeOut_slot);

    checkTimer = new QTimer(this);
    checkTimer->setInterval(200);
    connect(checkTimer, &QTimer::timeout, this, &M1WaitForFinish::onCheckTimeOut_slot);
}

M1WaitForFinish::~M1WaitForFinish()
{
    qDebug() << "M1WaitForFinish:destroyed";
}

void M1WaitForFinish::startWaiting(int minMs, int maxMs)
{
    if (maxMs < minMs) {
        qDebug() << "startWaiting params error";
    }
    minTimer->setInterval(minMs);
    maxTimer->setInterval(maxMs);

    minTimer->start();
}

void M1WaitForFinish::stopWaiting()
{
    minTimer->stop();
    maxTimer->stop();
    checkTimer->stop();
    qDebug() << "M1WaitForFinish:stopWaiting";
}

/* SLOT */

void M1WaitForFinish::onMinTimeOut_slot()
{
    checkTimer->start();
    maxTimer->start();
}

void M1WaitForFinish::onMaxTimeOut_slot()
{
    checkTimer->stop();
    qDebug() << "M1WaitForFinish: time out! id:" << m_devid;
    emit finish_signal(DobotCommunicate_Timeout);
}

void M1WaitForFinish::onCheckTimeOut_slot()
{
    quint64 curIndex = 0;
    int res = GetQueuedCmdCurrentIndex(&curIndex);
    if (res != DobotCommunicate_NoError) {
        stopWaiting();
        emit finish_signal(res);
        qDebug() << "M1WaitForFinish::GetQueuedCmdCurrentIndex Error";
    } else {
        if (m_targetid == curIndex) {
            stopWaiting();
            emit finish_signal(res);
        }
    }
}
