#include <iostream>
#include <vector>
#include <thread>

#include "tdrblockfree.h"
#include "event.h"

namespace lockfree {

#define FREE_BUFFER_SIZE 25000
#define QUEUE_SIZE 25000

ThreadedDualRBLockFreeTest::ThreadData::ThreadData ()
  : freeBuffers_ {FREE_BUFFER_SIZE},
    logQueue_ {FREE_BUFFER_SIZE}
{
    for (auto i = 0; i < FREE_BUFFER_SIZE; ++i)
    {
        auto s = new std::string ();
        s->reserve(1024);
        freeBuffers_.push (s);
    }
}

ThreadedDualRBLockFreeTest::ThreadData::~ThreadData ()
{
    std::string * s;
    while (freeBuffers_.size () > 0)
    {
        s = freeBuffers_.pop ();
        delete s;
    }

    while (logQueue_.size () > 0)
    {
        s = logQueue_.pop ();
        delete s;
    }
}


ThreadedDualRBLockFreeTest::ThreadedDualRBLockFreeTest (int iterations, int threads)
  : Test {"tdrblockfree", iterations, threads}
{
    for (unsigned i = 0; i < threads; ++i)
    {
        threadData_.push_back (new ThreadData ());
    }
}

ThreadedDualRBLockFreeTest::~ThreadedDualRBLockFreeTest ()
{
    for ( auto & td : threadData_)
    {
        delete td;
    }
}

void ThreadedDualRBLockFreeTest::worker ()
{
    running_ = true;
    while (running_)
    {
        for (auto td : threadData_)
        {
            std::string * logLine;
            while ((logLine = td->logQueue_.tryPop ()))
            {
                log (*logLine);
                td->freeBuffers_.push (logLine);
            }

        }
        std::this_thread::sleep_for (std::chrono::milliseconds (200));
    }
    logFile_.flush ();
}

void ThreadedDualRBLockFreeTest::log (const std::string & logLine)
{
    logFile_ << logLine << std::endl;
}

void ThreadedDualRBLockFreeTest::run ()
{
    std::thread threadWorker (&ThreadedDualRBLockFreeTest::worker, this);

    Event e;

    auto thread_count = threads ();
    auto lines_per_thread = iterations () / threads ();

    std::vector<std::thread> writers;
    for (auto i = 0; i < thread_count; ++i)
    {
        writers.push_back (
            std::thread ([&e, i, this, lines_per_thread]()
            {
                auto td = threadData_[i];
                e.wait ();

                for (auto l = 0; l < lines_per_thread; ++l)
                {
                    auto s = td->freeBuffers_.pop ();
                    Test::getSentence (*s);
                    td->logQueue_.push (s);
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
