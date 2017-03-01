#include <vector>
#include <thread>
#include <iostream>

#include "bulkworker.h"
#include "event.h"

namespace lockfree {

#define FREE_BUFFER_SIZE 250000
#define QUEUE_SIZE 25000

BulkWorkerTest::BulkWorkerTest (int iterations, int threads)
  : Test {"bulkworker", iterations, threads}
{
}

void BulkWorkerTest::backgroundWriter ()
{
    running_ = true;
    while (running_)
    {
        {
            std::lock_guard<std::mutex> lock (logMutex_);
            while (logQueue_.size () > 0)
            {
                auto * logLine = logQueue_.front ();
                log (*logLine);
                delete logLine;
                logQueue_.pop_front ();
            }
        }
        std::this_thread::sleep_for (std::chrono::milliseconds (200));
    }
}

void BulkWorkerTest::log (const std::string & logLine)
{
    logFile_ << logLine << std::endl;
}

void BulkWorkerTest::run ()
{
    std::thread threadWriter (
        &BulkWorkerTest::backgroundWriter,
        this
    );

    Event e;


    auto thread_count = threads ();
    auto lines_per_thread = iterations () / threads ();

    std::vector<std::list<std::string *>> threadData;
    for (unsigned int i = 0; i < thread_count; ++i)
    {
        std::list<std::string *> newBuffers;
        for (auto i = 0; i < lines_per_thread; ++i)
        {
            auto * s = new std::string ();
            s->reserve (1024);
            newBuffers.push_back (s);
        }
        std::cout.flush();
        threadData.push_back (newBuffers);
    }

    std::vector<std::thread> workers;
    for (unsigned int i = 0; i < thread_count; ++i)
    {
        workers.push_back (
            std::thread ([i, &e, &threadData, this, lines_per_thread]()
            {
                std::list<std::string *> & freeBuffers = threadData[i];

                e.wait ();

                for (unsigned int l = 0; l < lines_per_thread; l+=10000)
                {
                    std::list<std::string *> tempQueue;
                    unsigned numLines = std::min((unsigned)10000, lines_per_thread-l);
                    {
                        for (unsigned m = 0; m < numLines; ++m)
                        {
                            auto * buffer = freeBuffers.front ();
                            freeBuffers.pop_front ();
                            Test::getSentence (*buffer);
                            tempQueue.push_back (buffer);
                        }
                    }

                    {
                        std::lock_guard<std::mutex> lock (logMutex_);
                        for (auto s : tempQueue)
                        {
                            logQueue_.push_back (s);
                        }
                    }
                    tempQueue.clear();
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
