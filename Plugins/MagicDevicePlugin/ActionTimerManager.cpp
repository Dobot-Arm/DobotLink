#include "ActionTimerManager.h"

#include <QDebug>

#define TICK_TIMER_VALUE (300)

ActionTimerSet::ActionTimerSet(quint64 id) : id(id)
{
    isTimeOver = false;
    isUpdated = false;

    targetIndex = 0;
    slaveIndex = 0;

    timeoutValue = 1000;
}

/* 入口：addTimer() */
/* 出口1：超时 timeout */
/* 出口2：运动结束 */

ActionTimerManager::ActionTimerManager(QObject *parent) : QObject(parent)
{
    m_tickTimer = new QTimer(this);
    m_tickTimer->setInterval(TICK_TIMER_VALUE);
    connect(m_tickTimer, &QTimer::timeout,
            this, &ActionTimerManager::tickTimerTimeover_slot);

    m_actionFinishTimer = new QTimer(this);
    m_actionFinishTimer->setSingleShot(true);
    connect(m_actionFinishTimer, &QTimer::timeout,
            this, &ActionTimerManager::actionFinishTimeover_slot);
}

ActionTimerManager::~ActionTimerManager()
{
    clearTimers();
}

void ActionTimerManager::addTimer(ActionTimerSet *timer)
{
    if (timer->timeoutValue > 0 and !containsTimerId(timer->id)) {
        m_timerSetList.append(timer);

        if (!m_tickTimer->isActive()) {
            m_tickTimer->start();
            isUpdated = true;
        }
    }
}

void ActionTimerManager::setTargetIndexWithId(quint64 id, quint64 targetIndex)
{
    qDebug().noquote() << QString("Set timer, id:%1, target index:%2")
                          .arg(id)
                          .arg(targetIndex);

    foreach (ActionTimerSet *timerSet, m_timerSetList) {
        if (timerSet->id == id) {
            timerSet->targetIndex = targetIndex;
            return;
        }
    }
    qDebug() << "can not find matched timerset.";
}

void ActionTimerManager::updateCurrentIndex(quint64 index)
{
    m_currentIndex = index;
    isUpdated = true;

    if (m_timerSetList.isEmpty()) {
        qDebug() << "no timerset.";
        return;
    }

    ActionTimerSet *timerSet = m_timerSetList.first();

    if (timerSet->targetIndex == 0) {
        qWarning() << "timer target error.";
        timerSet->targetIndex = 1;
    }

    if (m_currentIndex < timerSet->targetIndex - 1) {
        qDebug().noquote() << QString("# ACTION: [%1->%2]")
                              .arg(index)
                              .arg(timerSet->targetIndex - 1);
    } else if (m_currentIndex == timerSet->targetIndex - 1) {
        if (!m_actionFinishTimer->isActive()) {
            m_actionFinishTimer->start(timerSet->timeoutValue);
        }
        qDebug().noquote() << QString("# ACTION: [%1->%2] remain:%3ms")
                              .arg(index)
                              .arg(timerSet->targetIndex)
                              .arg(m_actionFinishTimer->remainingTime());
    } else if (m_currentIndex >= timerSet->targetIndex) {
//! [RESULT 2] ACTION FINISH
        qDebug() << "# TASK: ACTION finish.";

        timerSet->isTimeOver = false;
        emit actionFinish_signal(timerSet->id, NOERROR, this->getCurrentTargetType());

        delete timerSet;
        m_timerSetList.removeOne(timerSet);

        m_actionFinishTimer->stop();

        if (m_timerSetList.isEmpty()) {
            m_tickTimer->stop();
        }
    }
}

void ActionTimerManager::clearTimers()
{
    m_actionFinishTimer->stop();
    m_tickTimer->stop();

    foreach (ActionTimerSet *timer, m_timerSetList) {
        emit actionFinish_signal(timer->id, CANCELED, this->getCurrentTargetType());
        delete timer;
    }
    m_timerSetList.clear();
}

bool ActionTimerManager::containsTimerId(quint64 id)
{
    foreach (ActionTimerSet *timer, m_timerSetList) {
        if (timer->id == id) {
            return true;
        }
    }
    return false;
}

quint64 ActionTimerManager::getCurrentHandlingId()
{
    if (!m_timerSetList.isEmpty()) {
        ActionTimerSet *timer = m_timerSetList.first();
        return timer->id;
    }
    return 0;
}

QString ActionTimerManager::getCurrentTargetType()
{
    if (!m_timerSetList.isEmpty()) {
        ActionTimerSet *timer = m_timerSetList.first();
        return timer->targetType;
    }
    return QString();
}

quint8 ActionTimerManager::getCurrentSlaveIndex()
{
    if (!m_timerSetList.isEmpty()) {
        ActionTimerSet *timer = m_timerSetList.first();
        return timer->slaveIndex;
    }
    return 0;
}

/* TICK TIMER 200ms */
void ActionTimerManager::tickTimerTimeover_slot()
{
    if (isUpdated == true) {
        isUpdated = false;
        emit getCurrentIndex_signal();
    }
}

//![RESULT 1] ACTION TIMEOUT
void ActionTimerManager::actionFinishTimeover_slot()
{
    qDebug() << "# TASK: ACTION TIMEOUT.";
    if (m_timerSetList.isEmpty()) {
        return;
    }

    ActionTimerSet *timer = m_timerSetList.takeFirst();
    timer->isTimeOver = true;
    emit actionFinish_signal(timer->id, TIMEOUT, this->getCurrentTargetType());

    delete timer;

    if (m_timerSetList.isEmpty()) {
        m_tickTimer->stop();
    }
}

void ActionTimerManager::setWsPort(quint16 wsPort)
{
    this->wsPort = wsPort;
}

quint16 ActionTimerManager::getWsPort()
{
    return wsPort;
}
