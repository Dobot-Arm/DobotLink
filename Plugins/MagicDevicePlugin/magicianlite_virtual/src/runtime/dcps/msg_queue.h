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

#include <list>
#include <mutex>
#include <atomic>
#include <memory>
#include <stdexcept>
#include "semaphore.h"

class MsgQueueFullException : public std::logic_error
{
public:
    MsgQueueFullException(const char* pFile=__FILE__,const int iLine=__LINE__)
        : std::logic_error(std::string(pFile)+","+std::to_string(iLine)+" queue is full")
    {
    }
};

class MsgQueueDestroyException : public std::logic_error
{
public:
    MsgQueueDestroyException(const char* pFile=__FILE__,const int iLine=__LINE__)
        : std::logic_error(std::string(pFile)+","+std::to_string(iLine)+" queue is empty")
    {
    }
};

/**
 * 消息队列
 */
template <typename T>
class MsgQueue
{
private:
    uint32_t size_;
    bool is_finished_;
    std::atomic<uint32_t> length_;

    Semaphore sem_;
    std::mutex mutex_;
    std::list<std::shared_ptr<T>> msg_queue_{};

public:
    explicit MsgQueue(uint32_t size) : size_(size), is_finished_(false), length_(0)
    {
    }

    ~MsgQueue()
    {
        destroy();
    }

    uint32_t getSize() const
    {
        return size_;
    }

    uint32_t getLength() const
    {
        return length_;
    }

    void clean()
    {
        length_ = 0;
        sem_.clear();
        msg_queue_.clear();
    }

    void push(const std::shared_ptr<T>& msg)
    {
        if (length_ >= size_)
            throw MsgQueueFullException();
        if (is_finished_)
            return;
        mutex_.lock();
        msg_queue_.push_back(msg);
        length_++;
        mutex_.unlock();
        sem_.post();
    }

    std::shared_ptr<T> pop()
    {
        sem_.wait();
        if (is_finished_)
            throw MsgQueueDestroyException();
        mutex_.lock();
        std::shared_ptr<T> msg = msg_queue_.front();
        msg_queue_.pop_front();
        length_--;
        mutex_.unlock();
        return msg;
    }

    void destroy()
    {
        is_finished_ = true;
        sem_.postAll();
    }
};
