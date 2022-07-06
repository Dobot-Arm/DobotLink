#pragma once

#include <QObject>

#include <QMutex>
#include <QSet>

class CVirtualSerialPort;

class CVirtualSerialPortManager : public QObject
{
    Q_OBJECT
public:
    ~CVirtualSerialPortManager();

    static CVirtualSerialPortManager* GetInstance();

    void Register(CVirtualSerialPort*);
    void Unregister(CVirtualSerialPort*);

protected slots:
    void slotWriteData(QByteArray data);

protected:
    CVirtualSerialPortManager(QObject *parent = nullptr);
    CVirtualSerialPortManager(const CVirtualSerialPortManager&) = default;
    CVirtualSerialPortManager(CVirtualSerialPortManager&&) = default;
    CVirtualSerialPortManager& operator=(const CVirtualSerialPortManager&) = default;
    CVirtualSerialPortManager& operator=(CVirtualSerialPortManager&&) = default;

private:
    QMutex m_mtx;
    QSet<CVirtualSerialPort*> m_setSerialPort;

    static QMutex m_mtxMgr;
    static CVirtualSerialPortManager* m_pVirtualSerialPortManager;
    static QThread* m_ThdVSerialPortWorker;
};

