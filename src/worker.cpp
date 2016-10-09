#include <thread>

#include "worker.h"

namespace lockfree {

WorkerTest::WorkerTest (int iterations, int threads)
  : Test ("worker", iterations, threads)
{

}

void WorkerTest::worker ()
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

void WorkerTest::log (const std::string & logLine)
{
    logFile_ << logLine << std::endl;
}

void WorkerTest::run ()
{
    std::thread threadWorker (&WorkerTest::worker, this);

    auto lines = iterations ();

    start ();
    for (int i = 0; i < lines; ++i)
    {
        std::lock_guard<std::mutex> lock (logMutex_);
        logQueue_.push_back (Test::getSentence ());
    }
    stop ();
    running_ = false;
    threadWorker.join ();
}

} // namespace lockfree
