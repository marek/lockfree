#pragma once

#include <vector>
#include <string>
#include <list>
#include <mutex>

#include "test.h"
#include "hs_spsc_queue.h"

namespace lockfree {

class HSSPSCTest : public Test
{
  public:
    struct ThreadData
    {
        ThreadData ();
        ~ThreadData ();
        HSSPSCQueue<std::string*> freeBuffers_;
        HSSPSCQueue<std::string*> logQueue_;
    };

    HSSPSCTest (int iterations, int threads);
    ~HSSPSCTest ();
    void run ();

  private:
    inline void log (const std::string & logLine);
    void backgroundWriter ();
    std::mutex logMutex_;
    volatile bool running_;

    std::vector<ThreadData *> threadData_;

};

} // namespace lockfree
