#include <iostream>
#include <vector>
#include <thread>

#include "mpsclockfree.h"
#include "event.h"

namespace lockfree {

#define FREE_BUFFER_SIZE 25000

MPSCLockFreeTest::MPSCLockFreeTest (int iterations, int threads)
  : Test {"mpsclockfree", iterations, threads},
    freeBuffers_ {FREE_BUFFER_SIZE}
{
    for (auto i = 0; i < FREE_BUFFER_SIZE; ++i)
    {
        auto s = new std::string ();
        s->reserve(1024);
        freeBuffers_.push (s);
    }
}

MPSCLockFreeTest::~MPSCLockFreeTest ()
{
    std::string * s;
    while (freeBuffers_.size () > 0)
    {
        s = freeBuffers_.pop ();
        delete s;
    }

    while (logQueue_.tryPop (s))
    {
        delete s;
    }
}

void MPSCLockFreeTest::worker ()
{
    running_ = true;
    while (running_)
    {
        std::string * logLine;
        while (logQueue_.tryPop (logLine))
        {
            log (*logLine);
            freeBuffers_.push (logLine);
        }
        std::this_thread::sleep_for (std::chrono::milliseconds (200));
    }
    logFile_.flush ();
}

void MPSCLockFreeTest::log (const std::string & logLine)
{
    logFile_ << logLine << std::endl;
}

void MPSCLockFreeTest::run ()
{
    std::thread threadWorker (&MPSCLockFreeTest::worker, this);

    Event e;

    auto thread_count = threads ();
    auto lines_per_thread = iterations () / threads ();

    std::vector<std::thread> writers;
    for (auto i = 0; i < thread_count; ++i)
    {
        writers.push_back (
            std::thread ([&]()
            {
                e.wait ();

                for (auto l = 0; l < lines_per_thread; ++l)
                {
                    auto s = freeBuffers_.pop ();
                    s->assign (Test::getSentence ());
                    logQueue_.push (s);
                }
            })
        );
    }

    start ();
    e.set ();
    for(auto & thread : writers)
    {
        thread.join();
    }
    stop ();
    running_ = false;
    threadWorker.join ();
}

} // namespace lockfree
