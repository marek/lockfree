#pragma once

#include <vector>
#include <string>
#include <list>
#include <mutex>

#include "test.h"
#include "circularbuffer.h"
#include "spsc_queue.h"

namespace lockfree {

class PoorSPSCTest : public Test
{
  public:
    struct ThreadData
    {
        ThreadData ();
        ~ThreadData ();
        SPSCQueue<std::string*> logQueue_;
    };

    PoorSPSCTest (int iterations, int threads);
    ~PoorSPSCTest ();
    void run ();

  private:
    inline void log (const std::string & logLine);
    void backgroundWriter ();
    std::mutex logMutex_;
    volatile bool running_;

    std::vector<ThreadData *> threadData_;

};

} // namespace lockfree
