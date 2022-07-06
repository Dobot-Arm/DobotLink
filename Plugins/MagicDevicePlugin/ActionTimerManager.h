#ifndef ACTIONTIMERMANAGER_H
#define ACTIONTIMERMANAGER_H

#include <QObject>
#include <QTimer>
#include <QJsonObject>
#include <QList>
#include <QMap>

class ActionTimerSet
{
public:
    ActionTimerSet(quint64 id = 0);
    bool isTimeOver;
    bool isUpdated;

    int timeoutValue;
    quint64 id;
    QString cmd;
    quint64 targetIndex;
    QString targetType;
    quint8 slaveIndex;
};

class ActionTimerManager : public QObject
{
    Q_OBJECT

public:
    enum FinishType {NOERROR, TIMEOUT, CANCELED};
    explicit ActionTimerManager(QObject *parent = nullptr);
    ~ActionTimerManager();

    void addTimer(ActionTimerSet *timer);

    void setTargetIndexWithId(quint64 id, quint64 index);
    void updateCurrentIndex(quint64 index);

    void clearTimers();

    bool containsTimerId(quint64 id);
    quint64 getCurrentHandlingId();
    QString getCurrentTargetType();
    quint8 getCurrentSlaveIndex();
    void setWsPort(quint16 wsPort);
    quint16 getWsPort();

signals:
    void getCurrentIndex_signal();
    void actionFinish_signal(quint64 id, FinishType type, QString targetType);

private:
    QTimer *m_actionFinishTimer;
    QTimer *m_tickTimer;

    bool isUpdated;
    quint64 m_currentIndex;

    QList<ActionTimerSet*> m_timerSetList;

    quint16 wsPort;

private slots:
    void tickTimerTimeover_slot();
    void actionFinishTimeover_slot();
};

#endif // ACTIONTIMERMANAGER_H
