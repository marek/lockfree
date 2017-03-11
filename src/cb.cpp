#include <iostream>
#include <vector>
#include <thread>

#include "cb.h"
#include "event.h"

namespace lockfree {

CBTest::CBTest (int iterations, int threads)
  : Test {"cb", iterations, threads},
    logQueue_ {25000}
{

}

void CBTest::backgroundWriter ()
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

void CBTest::log (const std::string & logLine)
{
    logFile_ << logLine << std::endl;
}

void CBTest::run ()
{
    std::thread threadWriter (&CBTest::backgroundWriter, this);

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
    for (auto & thread : workers)
    {
        thread.join ();
    }
    stop ();
    running_ = false;
    threadWriter.join ();
}

} // namespace lockfree
