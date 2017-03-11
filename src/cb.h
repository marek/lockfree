#pragma once

#include <string>
#include <list>
#include <mutex>

#include "test.h"
#include "circularbuffer.h"

namespace lockfree {

class CBTest : public Test
{
  public:
    CBTest (int interations, int threads);
    void run ();

  private:
    inline void log (const std::string & logLine);
    void backgroundWriter ();
    std::mutex logMutex_;
    volatile bool running_;
    CircularBuffer<std::string*> logQueue_;

};

} // namespace lockfree
