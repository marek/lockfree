#pragma once

#include <vector>
#include <string>
#include <list>
#include <mutex>

#include "test.h"
#include "circularbuffer.h"
#include "mpsc_queue.h"

namespace lockfree {

class MPSCLockFreeTest : public Test
{
  public:
    MPSCLockFreeTest (int iterations, int threads);
    ~MPSCLockFreeTest ();
    void run ();

  private:
    inline void log (const std::string & logLine);
    void backgroundWriter ();
    std::mutex logMutex_;
    volatile bool running_;

    CircularBuffer<std::string*> freeBuffers_;
    MPSCQueue<std::string*> logQueue_;
};

} // namespace lockfree
