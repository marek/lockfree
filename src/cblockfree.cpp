#include <iostream>
#include <vector>
#include <thread>

#include "cblockfree.h"
#include "event.h"

namespace lockfree {

CBLockFreeTest::CBLockFreeTest (int iterations, int threads)
  : Test {"rblockfree", iterations, threads},
    logQueue_ {25000}
{

}

void CBLockFreeTest::backgroundWriter ()
{
    running_ = true;
    while (running_)
    {
        while (logQueue_.size () > 0)
        {
            auto logLine = logQueue_.pop ();
            log (*logLine);
            delete logLine;
        }
        std::this_thread::sleep_for (std::chrono::milliseconds (200));
    }
}

void CBLockFreeTest::log (const std::string & logLine)
{
    logFile_ << logLine << std::endl;
}

void CBLockFreeTest::run ()
{
    std::thread threadWorker (&CBLockFreeTest::backgroundWriter, this);

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
                    auto s = new std::string (
                        Test::getSentence ()
                    );
                    logQueue_.push (s);
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
