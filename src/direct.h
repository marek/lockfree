#pragma once

#include "test.h"

namespace lockfree {

class DirectWriteTest : public Test
{
  public:
    DirectWriteTest (int iterations, int threads);
    void run ();

  private:
    inline void log (const std::string & logLine);

};

} // namespace lockfree