#pragma once

#include <mutex>

#include "test.h"

namespace lockfree {

class MultipleDirectWriteTest : public Test
{
  public:
    MultipleDirectWriteTest (int iterations, int threads);
    void run ();

  private:
    inline void log (const std::string & logLine);
    std::mutex logMutex_;
};

} // namespace lockfree
