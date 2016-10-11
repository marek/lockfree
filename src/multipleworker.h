#pragma once

#include <list>
#include <mutex>

#include "test.h"

namespace lockfree {

class MultipleWorkerTest : public Test
{
  public:
    MultipleWorkerTest (int iterations, int threads);
    void run ();

  private:
    inline void log (const std::string & logLine);
    void backgroundWriter ();
    std::mutex logMutex_;
    volatile bool running_;
    std::list<std::string> logQueue_;

};

} // namespace lockfree