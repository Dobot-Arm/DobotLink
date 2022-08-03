#include "cmythreadpool.h"

#include <QSemaphore>
#include <QMutex>
#include <QList>


class CAutoDeletePool
{
    QSemaphore m_sem;
    QMutex m_mtx;
    QList<CMyThreadPool*> m_lst;
    std::thread m_thread;
    bool m_isRunning;

    CAutoDeletePool()
    {
        m_isRunning = true;
        std::thread thd([this]{run();});
        m_thread.swap(thd);
    }

    void run()
    {
        while (m_isRunning)
        {
            m_sem.acquire();
            QMutexLocker locker(&m_mtx);
            if (m_lst.isEmpty())
            {
                continue;
            }
            auto p = m_lst.takeFirst();
            locker.unlock();

            if (p) delete p;
        }
    }

public:
    ~CAutoDeletePool()
    {
        m_isRunning = false;
        Delete(nullptr);
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    static CAutoDeletePool& GetIntance()
    {
        static CAutoDeletePool obj;
        return obj;
    }

    void Delete(CMyThreadPool *thread)
    {
        QMutexLocker locker(&m_mtx);
        m_lst.push_back(thread);
        m_sem.release();
    }
};

CMyThreadPool::CMyThreadPool(size_t nMaxThreadSize)
{
    m_iMaxThreadCount = nMaxThreadSize;
    m_threads.reserve(m_iMaxThreadCount*2);
    Start(1);
}

CMyThreadPool::~CMyThreadPool()
{
    Clear();
    Stop();
}

void CMyThreadPool::deleteLater()
{
    CAutoDeletePool::GetIntance().Delete(this);
}

void CMyThreadPool::start(QObject* pObj, const std::function<void (bool)> &exec)
{
    if (!m_bStart)
    {
        exec(true);
        return;
    }
    std::unique_lock<std::mutex> lock(m_mtx);
    m_tasks.push_back(std::make_pair(pObj,exec));
    if (0 == m_iIdleCount && m_threads.size()<m_iMaxThreadCount)
    {
        CreateThread(1);
    }
    m_cond.notify_one();
}

void CMyThreadPool::Clear()
{
    std::unique_lock<std::mutex> lock(m_mtx);
    while(!m_tasks.empty())
    {
        auto task = std::move(m_tasks.front());
        m_tasks.pop_front();

        task.second(true);
    }
}

void CMyThreadPool::Clear(QObject* pObj)
{
    std::unique_lock<std::mutex> lock(m_mtx);
    auto itr = m_tasks.begin();
    while (itr != m_tasks.end())
    {
        if (itr->first == pObj)
        {
            itr->second(true);
            itr = m_tasks.erase(itr);
        }
        else
        {
            ++itr;
        }
    }
}

void CMyThreadPool::Start(size_t nThreadSize)
{
    m_bStart = true;
    CreateThread(nThreadSize);
}

void CMyThreadPool::CreateThread(size_t nThreadSize)
{
    for (size_t n=0; n<nThreadSize; ++n)
    {
        ++m_iIdleCount;
        m_threads.push_back(std::thread(&CMyThreadPool::Run,this));
    }
}

void CMyThreadPool::Stop()
{
    m_bStart = false;
    m_cond.notify_all();
    for (auto& thd : m_threads)
    {
        if (thd.joinable())
        {
            thd.join();
        }
    }
    m_threads.clear();
    return;
}

void CMyThreadPool::Run()
{
    while (m_bStart)
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        while (m_bStart && m_tasks.empty())
        {
            m_cond.wait(lock);
        }
        if (!m_bStart)
        {
            return;
        }
        auto task = std::move(m_tasks.front());
        m_tasks.pop_front();
        lock.unlock();

        --m_iIdleCount;
        task.second(false);
        ++m_iIdleCount;
    }
}
