#ifndef MONITORUI_H
#define MONITORUI_H

#include <QWidget>

class QLCDNumber;

class MonitorUI : public QWidget
{
    Q_OBJECT
public:
    explicit MonitorUI(QWidget *parent = nullptr);

signals:

public slots:
    void updateJoint(float joint1, float joint2, float joint3, float joint4);

private:
    void SetupUi();

private:
    QLCDNumber* m_pLcd1;
    QLCDNumber* m_pLcd2;
    QLCDNumber* m_pLcd3;
    QLCDNumber* m_pLcd4;
};

#endif // MONITORUI_H
