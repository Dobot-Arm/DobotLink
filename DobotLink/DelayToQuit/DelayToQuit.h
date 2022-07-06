#ifndef DELAYTOQUIT_H
#define DELAYTOQUIT_H

#include <QObject>
#include <QWebSocket>
#include <QTimer>

class DelayToQuit : public QObject
{
    Q_OBJECT

public:
    explicit DelayToQuit(QObject *parent = nullptr);
    ~DelayToQuit();

    void start();

private:
    QWebSocket *m_websocket;
    QTimer *m_timer;

private slots:
    void timeOutAndQuit_slot();
    void onConnected_slot();
    void onMessageReceive_slot(QString message);
};

#endif // DELAYTOQUIT_H
