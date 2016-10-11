#pragma once

#include <vector>
#include <string>
#include <list>
#include <mutex>

#include "test.h"
#include "circularbuffer.h"
#include "spsc_queue.h"

namespace lockfree {

class ThreadedDualRBLockFreeTest : public Test
{
  public:
    struct ThreadData
    {
        ThreadData ();
        ~ThreadData ();
        CircularBuffer<std::string*> freeBuffers_;
        CircularBuffer<std::string*> logQueue_;
    };

    ThreadedDualRBLockFreeTest (int iterations, int threads);
    ~ThreadedDualRBLockFreeTest ();
    void run ();

  private:
    inline void log (const std::string & logLine);
    void worker ();
    std::mutex logMutex_;
    volatile bool running_;

    std::vector<ThreadData *> threadData_;

};

} // namespace lockfree
