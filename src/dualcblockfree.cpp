#include <iostream>
#include <vector>
#include <thread>

#include "dualcblockfree.h"
#include "event.h"

namespace lockfree {

#define FREE_BUFFER_SIZE 25000
#define QUEUE_SIZE 25000

DualCBLockFreeTest::DualCBLockFreeTest (int iterations, int threads)
  : Test {"dualcblockfree", iterations, threads},
    freeBuffers_ {FREE_BUFFER_SIZE},
    logQueue_ {QUEUE_SIZE}
{
    for (auto i = 0; i < FREE_BUFFER_SIZE; ++i)
    {
        auto s = new std::string ();
        s->reserve (1024);
        freeBuffers_.push (s);
    }
}

DualCBLockFreeTest::~DualCBLockFreeTest ()
{
    while (freeBuffers_.size () > 0)
    {
        auto s = freeBuffers_.pop ();
        delete s;
    }

    while (logQueue_.size () > 0)
    {
        auto s = logQueue_.pop ();
        delete s;
    }
}

void DualCBLockFreeTest::backgroundWriter ()
{
    running_ = true;
    while (running_)
    {
        while (logQueue_.size () > 0)
        {
            auto logLine = logQueue_.pop ();
            log (*logLine);
            freeBuffers_.push (logLine);
        }
        std::this_thread::sleep_for (std::chrono::milliseconds (200));
    }
    logFile_.flush ();
}

void DualCBLockFreeTest::log (const std::string & logLine)
{
    logFile_ << logLine << std::endl;
}

void DualCBLockFreeTest::run ()
{
    std::thread threadWriter (&DualCBLockFreeTest::backgroundWriter, this);

    Event e;

    auto thread_count = threads ();
    auto lines_per_thread = iterations () / threads ();

    std::vector<std::thread> workers;
    for (auto i = 0; i < thread_count; ++i)
    {
        workers.push_back (
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
    for (auto & thread : workers)
    {
        thread.join ();
    }
    stop ();
    running_ = false;
    threadWriter.join ();
}

} // namespace lockfree
