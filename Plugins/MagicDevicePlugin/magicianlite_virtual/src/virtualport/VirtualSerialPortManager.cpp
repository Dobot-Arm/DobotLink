#include "VirtualSerialPortManager.h"
#include "VirtualSerialPort.h"

#include <QThread>
#include <QMutexLocker>
#include <QAtomicInt>

QMutex CVirtualSerialPortManager::m_mtxMgr;
CVirtualSerialPortManager* CVirtualSerialPortManager::m_pVirtualSerialPortManager = nullptr;
QThread* CVirtualSerialPortManager::m_ThdVSerialPortWorker = nullptr;

CVirtualSerialPortManager::CVirtualSerialPortManager(QObject *parent)
    : QObject(parent)
{
}

CVirtualSerialPortManager::~CVirtualSerialPortManager()
{
}

CVirtualSerialPortManager* CVirtualSerialPortManager::GetInstance()
{
    if (nullptr == m_pVirtualSerialPortManager)
    {
        QMutexLocker guard(&m_mtxMgr);
        if (nullptr == m_pVirtualSerialPortManager)
        {
            m_pVirtualSerialPortManager = new CVirtualSerialPortManager();

            m_ThdVSerialPortWorker = new QThread();
            connect(m_ThdVSerialPortWorker, SIGNAL(finished()), m_ThdVSerialPortWorker, SLOT(deleteLater()));
            m_pVirtualSerialPortManager->moveToThread(m_ThdVSerialPortWorker);
            m_ThdVSerialPortWorker->start();
        }
    }
    return m_pVirtualSerialPortManager;
}

void CVirtualSerialPortManager::Register(CVirtualSerialPort* ptr)
{
    QMutexLocker guard(&m_mtx);
    m_setSerialPort.insert(ptr);
}

void CVirtualSerialPortManager::Unregister(CVirtualSerialPort* ptr)
{
    QMutexLocker guard(&m_mtx);
    m_setSerialPort.remove(ptr);
    if (m_setSerialPort.empty())
    {
        guard.unlock();

        QMutexLocker locker(&m_mtxMgr);
        m_ThdVSerialPortWorker->quit();
        m_ThdVSerialPortWorker->deleteLater();
        m_ThdVSerialPortWorker = nullptr;

        m_pVirtualSerialPortManager->deleteLater();
        m_pVirtualSerialPortManager = nullptr;
    }
}

void CVirtualSerialPortManager::slotWriteData(QByteArray data)
{
    CVirtualSerialPort* ptr = qobject_cast<CVirtualSerialPort*>(sender());
    if (nullptr == ptr) return ;
    QString strPort = ptr->portName();
    if (strPort.isEmpty() || strPort.isNull()) return ;

    CVirtualSerialPort* ptrReciver = nullptr;
    QMutexLocker guard(&m_mtx);
    for (auto itr = m_setSerialPort.begin(); itr != m_setSerialPort.end(); ++itr)
    {
        if (ptr == *itr) continue;
        if (strPort != (*itr)->portName()) continue;
        ptrReciver = *itr;
        break;
    }
    guard.unlock();

    if (ptrReciver && ptrReciver->isOpen())
    {
        emit ptrReciver->signalInnerReadData(data, CVirtualSerialPort::QPrivateSignal());
    }
}
