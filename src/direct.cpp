#include "direct.h"


namespace lockfree {

DirectWriteTest::DirectWriteTest (int iterations, int threads)
  : Test {"directwrite", iterations, threads}
{

}

void DirectWriteTest::log (const std::string & logLine)
{
    logFile_ << logLine << "\n";
}

void DirectWriteTest::run ()
{
    auto lines = iterations ();
    start ();
    for (int i = 0; i < lines; ++i)
    {
        log (Test::getSentence ());
    }
    stop ();
    logFile_.flush ();
}

} // namespace lockfree