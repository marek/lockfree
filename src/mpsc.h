#pragma once

#include <vector>
#include <string>
#include <list>
#include <mutex>

#include "test.h"
#include "circularbuffer.h"
#include "mpsc_queue.h"

namespace lockfree {

class MPSCTest : public Test
{
  public:
    MPSCTest (int iterations, int threads);
    ~MPSCTest ();
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
