#include <vector>
#include <thread>

#include "multipleworker.h"
#include "event.h"

namespace lockfree {

MultipleWorkerTest::MultipleWorkerTest (int iterations, int threads)
  : Test {"multipleworker", iterations, threads}
{

}

void MultipleWorkerTest::backgroundWriter ()
{
    running_ = true;
    while (running_)
    {
        {
            std::lock_guard<std::mutex> lock (logMutex_);
            while (logQueue_.size () > 0)
            {
                auto & logLine = logQueue_.front ();
                log (logLine);
                logQueue_.pop_front ();
            }
        }
        std::this_thread::sleep_for (std::chrono::milliseconds (200));
    }
}

void MultipleWorkerTest::log (const std::string & logLine)
{
    logFile_ << logLine << std::endl;
}

void MultipleWorkerTest::run ()
{
    std::thread threadWorker (&MultipleWorkerTest::backgroundWriter, this);

    Event e;

    auto thread_count = threads ();
    auto lines_per_thread = iterations () / threads ();

    std::vector<std::thread> writers;
    for (unsigned int i = 0; i < thread_count; ++i)
    {
        writers.push_back (
            std::thread ([&]()
            {
                e.wait ();

                for (unsigned int l = 0; l < lines_per_thread; ++l)
                {
                    std::lock_guard<std::mutex> lock (logMutex_);
                    logQueue_.push_back (Test::getSentence ());
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
