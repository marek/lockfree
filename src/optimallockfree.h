#pragma once

#include <string>
#include <list>
#include <mutex>

#include "test.h"
#include "circularbuffer.h"
#include "mpsc_queue.h"

namespace lockfree {

class OptimalLockFreeTest : public Test
{
  public:
    OptimalLockFreeTest (int iterations, int threads);
    ~OptimalLockFreeTest ();
    void run ();

  private:
    inline void log (const std::string & logLine);
    void worker ();
    std::mutex logMutex_;
    volatile bool running_;
    CircularBuffer<std::string*> freeBuffers_;
    CircularBuffer<std::string*> logQueue_;

    static void getWord (std::string & s);
    static void getSentence (std::string & s);

};

} // namespace lockfree
