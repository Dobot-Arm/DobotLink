#include "MyThread.h"
#include <QDebug>
#include <QTimer>
extern "C" {
#include "DobotV3Func.h"
}

MyThread::MyThread() {
    m_isStop = false;
    m_isTimerStart = false;

    m_timer.start(10);
    connect(&m_timer, &QTimer::timeout, [ = ]() {
        DobotV3_TimeCNT(10);
    });
}

void MyThread::setThreadStart() {
    m_isStop = false;
}

void MyThread::setThreadStop() {
    m_isStop = true;
}

void MyThread::setTimerStart(bool isTimerStart) {
    m_isTimerStart = isTimerStart;
}

bool MyThread::getTimerStart() {
    return m_isTimerStart;
}

void MyThread::run() {

    while (1) {
        if (!m_isStop) {
            DobotV3_Exec();
            msleep(5);
        }
    }
}
