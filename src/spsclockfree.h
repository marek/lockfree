#pragma once

#include <vector>
#include <string>
#include <list>
#include <mutex>

#include "test.h"
#include "circularbuffer.h"
#include "spsc_queue.h"

namespace lockfree {

class SPSCLockFreeTest : public Test
{
  public:
    struct ThreadData
    {
        ThreadData ();
        ~ThreadData ();
        SPSCQueue<std::string*> freeBuffers_;
        SPSCQueue<std::string*> logQueue_;
    };

    SPSCLockFreeTest (int iterations, int threads);
    ~SPSCLockFreeTest ();
    void run ();

  private:
    inline void log (const std::string & logLine);
    void backgroundWriter ();
    std::mutex logMutex_;
    volatile bool running_;

    std::vector<ThreadData *> threadData_;

};

} // namespace lockfree
