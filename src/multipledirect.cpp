#include <vector>
#include <thread>

#include "multipledirect.h"
#include "event.h"


namespace lockfree {

MultipleDirectWriteTest::MultipleDirectWriteTest (int iterations, int threads)
  : Test {"multipledirectwrite", iterations, threads}
{

}

void MultipleDirectWriteTest::log (const std::string & logLine)
{
    std::lock_guard<std::mutex> l (logMutex_);
    logFile_ << logLine << std::endl;
}

void MultipleDirectWriteTest::run ()
{
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
                    log (Test::getSentence ());
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
}

} // namespace lockfree