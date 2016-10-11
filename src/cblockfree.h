#pragma once

#include <string>
#include <list>
#include <mutex>

#include "test.h"
#include "circularbuffer.h"

namespace lockfree {

class CBLockFreeTest : public Test
{
  public:
    CBLockFreeTest (int interations, int threads);
    void run ();

  private:
    inline void log (const std::string & logLine);
    void worker ();
    std::mutex logMutex_;
    volatile bool running_;
    CircularBuffer<std::string*> logQueue_;

};

} // namespace lockfree
