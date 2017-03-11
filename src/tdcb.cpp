#include <iostream>
#include <vector>
#include <thread>

#include "tdcb.h"
#include "event.h"

namespace lockfree {

#define FREE_BUFFER_SIZE 25000
#define QUEUE_SIZE 25000

ThreadedDualCBTest::ThreadData::ThreadData ()
  : freeBuffers_ {FREE_BUFFER_SIZE},
    logQueue_ {FREE_BUFFER_SIZE}
{
    for (auto i = 0; i < FREE_BUFFER_SIZE; ++i)
    {
        auto s = new std::string ();
        s->reserve (1024);
        freeBuffers_.push (s);
    }
}

ThreadedDualCBTest::ThreadData::~ThreadData ()
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


ThreadedDualCBTest::ThreadedDualCBTest (int iterations, int threads)
  : Test {"threaded-dual-cb", iterations, threads}
{
    for (unsigned i = 0; i < threads; ++i)
    {
        threadData_.push_back (new ThreadData ());
    }
}

ThreadedDualCBTest::~ThreadedDualCBTest ()
{
    for ( auto & td : threadData_)
    {
        delete td;
    }
}

void ThreadedDualCBTest::backgroundWriter ()
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

void ThreadedDualCBTest::log (const std::string & logLine)
{
    logFile_ << logLine << std::endl;
}

void ThreadedDualCBTest::run ()
{
    std::thread threadWriter (
        &ThreadedDualCBTest::backgroundWriter,
        this
    );

    Event e;

    auto thread_count = threads ();
    auto lines_per_thread = iterations () / threads ();

    std::vector<std::thread> workers;
    for (auto i = 0; i < thread_count; ++i)
    {
        workers.push_back (
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
    for (auto & thread : workers)
    {
        thread.join ();
    }
    stop ();
    running_ = false;
    threadWriter.join ();
}

} // namespace lockfree
