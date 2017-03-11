#pragma once

#include <string>
#include <list>
#include <mutex>

#include "test.h"
#include "circularbuffer.h"

namespace lockfree {

class DualCBTest : public Test
{
  public:
    DualCBTest (int iterations, int threads);
    ~DualCBTest ();
    void run ();

  private:
    inline void log (const std::string & logLine);
    void backgroundWriter ();
    std::mutex logMutex_;
    volatile bool running_;
    CircularBuffer<std::string*> freeBuffers_;
    CircularBuffer<std::string*> logQueue_;
};

} // namespace lockfree
