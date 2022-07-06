#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QThread>
#include <QTimer>

class MyThread : public QThread {
  public:
    MyThread();
    void setThreadStart();
    void setThreadStop();

    void setTimerStart(bool isTimerStart);
    bool getTimerStart();
    void run();

  private:
    //volatile 易失性变量，不断重内存中读取值，而不是从编译器优化存储的寄存器中存取
    volatile bool m_isStop;
    volatile bool m_isTimerStart;
    QTimer m_timer;
};

#endif // MYTHREAD_H
