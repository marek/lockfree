#pragma once

#include <string>
#include <list>
#include <mutex>

#include "test.h"
#include "circularbuffer.h"

namespace lockfree {

class DualRBLockFreeTest : public Test
{
  public:
    DualRBLockFreeTest (int iterations, int threads);
    ~DualRBLockFreeTest ();
    void run ();

  private:
    inline void log (const std::string & logLine);
    void worker ();
    std::mutex logMutex_;
    volatile bool running_;
    CircularBuffer<std::string*> freeBuffers_;
    CircularBuffer<std::string*> logQueue_;
};

} // namespace lockfree
