#ifndef MYSAFEUI_H
#define MYSAFEUI_H

#include <QObject>

class MySafeUI : public QObject
{
    Q_OBJECT
public:
    explicit MySafeUI();

    void CreateUI();

signals:
    void signalCreatUI();

private slots:
    void slotCreateUI();
};

#endif // MYSAFEUI_H
