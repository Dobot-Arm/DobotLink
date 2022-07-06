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

#pragma once

#include <mutex>
#include <cstdint>
#include <condition_variable>

/**
 * 信号量
 */
class Semaphore
{
public:
    /**
     * \brief Ctor
     */
    explicit Semaphore(int32_t val = 0);

    /**
     * \brief Dtor
     */
    virtual ~Semaphore();

    /**
     * wait
     */
    void wait();

    /**
     * timeoutwait
     */
    bool timeoutwait(uint32_t millisecond);

    /**
     * post
     */
    void post();

    /**
     * postAll
     */
    void postAll();

    void clear();

private:
    int32_t cnt_;
    std::mutex mutex_;
    std::condition_variable cv_;
};
