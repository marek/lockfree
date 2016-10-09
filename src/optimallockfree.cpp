#include <iostream>
#include <vector>
#include <thread>

#include "OptimalLockFree.h"
#include "event.h"

namespace lockfree {

#define FREE_BUFFER_SIZE 25000
#define QUEUE_SIZE 25000

OptimalLockFreeTest::OptimalLockFreeTest (int iterations, int threads)
  : Test {"optimallockfree", iterations, threads},
    freeBuffers_ {FREE_BUFFER_SIZE},
    logQueue_ {QUEUE_SIZE}
{
    for (auto i = 0; i < FREE_BUFFER_SIZE; ++i)
    {
        auto s = new std::string ();
        s->reserve(1024);
        freeBuffers_.push (s);
    }
}

OptimalLockFreeTest::~OptimalLockFreeTest ()
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

void OptimalLockFreeTest::worker ()
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

void OptimalLockFreeTest::log (const std::string & logLine)
{
    logFile_ << logLine << std::endl;
}

void OptimalLockFreeTest::getWord (std::string & s)
{
    int maxLen = 2+rand ()%18;
    for (int i = 0; i < maxLen; ++i)
    {
        char c = '!' + rand ()%93;
        s += c;
    }
}

void OptimalLockFreeTest::getSentence (std::string & s)
{
    s.clear ();
    s += "[";
    s += std::to_string (std::time (nullptr));
    s += "] ";

    int maxLen = 2+rand ()%18;

    for (int i = 0; i < maxLen; ++i)
    {
        if (i != 0)
        {
            s += " ";
        }
        getWord (s);
    }
}

void OptimalLockFreeTest::run ()
{
    std::thread threadWorker (&OptimalLockFreeTest::worker, this);

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
                    OptimalLockFreeTest::getSentence (*s);
                    //s->assign (Test::getSentence ());
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
