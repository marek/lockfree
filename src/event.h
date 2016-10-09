#pragma once

#include <condition_variable>
#include <thread>

namespace lockfree {

class Event
{
  public:
    Event(bool signaled = false) : signaled_(signaled)
    {
    }

    void set()
    {
        {
            std::unique_lock<std::mutex> lock(m_);
            signaled_ = true;
        }
        cv_.notify_all();
    }

    void unset()
    {
        std::unique_lock<std::mutex> lock(m_);
        signaled_ = false;
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(m_);
        while (!signaled_)
        {
            cv_.wait(lock);
        }
    }

  private:
    std::mutex m_;
    std::condition_variable cv_;
    bool signaled_;
};

} // namespace lockfree