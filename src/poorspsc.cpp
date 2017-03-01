#include <iostream>
#include <vector>
#include <thread>

#include "poorspsc.h"
#include "event.h"

namespace lockfree {

#define FREE_BUFFER_SIZE 250000
#define QUEUE_SIZE 25000

PoorSPSCTest::ThreadData::ThreadData ()
  : logQueue_ {QUEUE_SIZE}
{

}

PoorSPSCTest::ThreadData::~ThreadData ()
{
    std::string * s;
    while (logQueue_.tryPop (s))
    {
        delete s;
    }
}

PoorSPSCTest::PoorSPSCTest (int iterations, int threads)
  : Test {"poorspsc", iterations, threads}
{
    for (unsigned i = 0; i < threads; ++i)
    {
        threadData_.push_back (new ThreadData ());
    }
}

PoorSPSCTest::~PoorSPSCTest ()
{
    for ( auto & td : threadData_)
    {
        delete td;
    }
}

void PoorSPSCTest::backgroundWriter ()
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
                delete logLine;
            }

        }
        std::this_thread::sleep_for (std::chrono::milliseconds (200));
    }
    logFile_.flush ();
}

void PoorSPSCTest::log (const std::string & logLine)
{
    logFile_ << logLine << std::endl;
}

void PoorSPSCTest::run ()
{
    std::thread threadWriter (&PoorSPSCTest::backgroundWriter, this);

    Event e;

    auto thread_count = threads ();
    auto lines_per_thread = iterations () / threads ();

    std::vector<std::thread> workers;
    for (auto i = 0; i < thread_count; ++i)
    {
        workers.push_back (
            std::thread ([&e, i, this, lines_per_thread] ()
            {
                auto td = threadData_[i];
                e.wait ();

                for (auto l = 0; l < lines_per_thread; ++l)
                {
                    std::string * s = new std::string ();
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
