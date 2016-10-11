#include <iostream>
#include <vector>
#include <thread>

#include "spsclockfree.h"
#include "event.h"

namespace lockfree {

#define FREE_BUFFER_SIZE 250000
#define QUEUE_SIZE 25000

SPSCLockFreeTest::ThreadData::ThreadData ()
  : freeBuffers_ {FREE_BUFFER_SIZE},
    logQueue_ {QUEUE_SIZE}
{
    for (auto i = 0; i < FREE_BUFFER_SIZE; ++i)
    {
        auto s = new std::string ();
        s->reserve (1024);
        freeBuffers_.push (s);
    }
}

SPSCLockFreeTest::ThreadData::~ThreadData ()
{
    std::string * s;
    while (freeBuffers_.tryPop (s))
    {
        if (s)
        {
            delete s;
        }
    }

    while (logQueue_.tryPop (s))
    {
        delete s;
    }
}


SPSCLockFreeTest::SPSCLockFreeTest (int iterations, int threads)
  : Test {"spsclockfree", iterations, threads}
{
    for (unsigned i = 0; i < threads; ++i)
    {
        threadData_.push_back (new ThreadData ());
    }
}

SPSCLockFreeTest::~SPSCLockFreeTest ()
{
    for ( auto & td : threadData_)
    {
        delete td;
    }
}

void SPSCLockFreeTest::backgroundWriter ()
{
    running_ = true;
    bool empty_ = false;
    while (running_ || !empty_)
    {
        empty_ = true;
        for (auto td : threadData_)
        {
            std::string * logLine;
            while (td->logQueue_.tryPop (logLine))
            {
                if (empty_)
                {
                    empty_ = false;
                }
                log (*logLine);
                td->freeBuffers_.push (logLine);
            }

        }
        std::this_thread::sleep_for (std::chrono::milliseconds (200));
    }
    logFile_.flush ();
}

void SPSCLockFreeTest::log (const std::string & logLine)
{
    logFile_ << logLine << std::endl;
}

void SPSCLockFreeTest::run ()
{
    std::thread threadWorker (&SPSCLockFreeTest::backgroundWriter, this);

    Event e;

    auto thread_count = threads ();
    auto lines_per_thread = iterations () / threads ();

    std::vector<std::thread> writers;
    for (auto i = 0; i < thread_count; ++i)
    {
        writers.push_back (
            std::thread ([&e, i, this, lines_per_thread] ()
            {
                auto td = threadData_[i];
                e.wait ();

                for (auto l = 0; l < lines_per_thread; ++l)
                {
                    std::string * s;
                    s = td->freeBuffers_.pop ();
                    Test::getSentence (*s);
                    td->logQueue_.push (s);
                }
            })
        );
    }

    start ();
    e.set ();
    for (auto & thread : writers)
    {
        thread.join ();
    }
    stop ();
    running_ = false;
    threadWorker.join ();
}

} // namespace lockfree
