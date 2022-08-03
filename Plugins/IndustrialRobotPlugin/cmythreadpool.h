#ifndef CMYTHREADPOOL_H
#define CMYTHREADPOOL_H

#include <QObject>
#include <iostream>
#include <thread>
#include <vector>
#include <list>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <memory>
#include <atomic>

class CMyThreadPool
{
    std::vector<std::thread> m_threads;

    std::list<std::pair<QObject*,std::function<void(bool isCanceled)>>> m_tasks;
    std::mutex m_mtx;
    std::condition_variable m_cond;

    std::atomic_bool m_bStart;

    size_t m_iMaxThreadCount;
    std::atomic_size_t m_iIdleCount;

public:
    CMyThreadPool(size_t nMaxThreadSize = 10);
    CMyThreadPool(const CMyThreadPool&) = delete;
    CMyThreadPool& operator=(const CMyThreadPool&) = delete;
    CMyThreadPool(CMyThreadPool&&) = delete;
    CMyThreadPool& operator=(CMyThreadPool&&) = delete;
    ~CMyThreadPool();

    void deleteLater();
    void start(QObject* pObj, const std::function<void(bool isCanceled)>& exec);
    void Clear();
    void Clear(QObject* pObj);

private:
    void CreateThread(size_t nThreadSize);
    void Start(size_t nThreadSize);

    void Stop();

    void Run();
};

#endif // CMYTHREADPOOL_H
