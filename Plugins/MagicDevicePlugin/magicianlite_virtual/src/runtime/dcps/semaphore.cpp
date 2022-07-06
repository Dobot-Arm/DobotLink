/**
 ***********************************************************************************************************************
 *
 * @author  ZhangRan
 * @date    2021/12/02
 * @version 1.0.0
 *
 * <h2><center>&copy; COPYRIGHT 2020 </center></h2>
 *
 ***********************************************************************************************************************
 */

#include "semaphore.h"

Semaphore::Semaphore(int32_t val) : cnt_(val)
{
}

Semaphore::~Semaphore()
{
}

void Semaphore::wait()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (--cnt_ < 0)
        cv_.wait(lock);
}

bool Semaphore::timeoutwait(uint32_t millisecond)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (--cnt_ < 0)
    {
        if (cv_.wait_for(lock, std::chrono::milliseconds(millisecond)) == std::cv_status::timeout)
            return false;
    }

    return true;
}

void Semaphore::post()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (++cnt_ <= 0)
        cv_.notify_one();
}

void Semaphore::postAll()
{
    cv_.notify_all();
}

void Semaphore::clear()
{
    cnt_ = 0;
}
