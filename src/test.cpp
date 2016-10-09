#include <cstdlib>
#include <unistd.h>
#include <limits.h>
#include <sstream>
#include <ctime>

#include "test.h"

namespace lockfree {

Test::Test (const std::string & testName, int iterations, int threads)
   : testName_ {testName},
     iterations_ {iterations},
     threads_ {threads}
{
    //
    // Save our log file in the current directory
    //
    std::string cwd = [] () -> std::string {
        char temp[PATH_MAX];
        getcwd (temp, PATH_MAX);
        return std::string (temp);
    } ();

    logFile_.open (cwd + "/logs/" + testName_ + ".log");
};

void Test::start ()
{
    start_ = std::chrono::high_resolution_clock::now ();
}

void Test::stop ()
{
    end_ = std::chrono::high_resolution_clock::now ();
}

double_seconds Test::duration ()
{
    std::chrono::duration<double> diff = end_ - start_;
    return std::chrono::duration_cast<double_seconds> (diff);
}

std::string Test::getWord ()
{
    std::string word;

    int maxLen = 2+rand ()%18;
    for (int i = 0; i < maxLen; ++i)
    {
        char c = '!' + rand ()%93;
        word += c;
    }
    return word;
}

std::string Test::getSentence ()
{
    std::stringstream sentence;

    sentence << "[" << std::time(nullptr) << "] ";

    int maxLen = 2+rand ()%18;

    for (int i = 0; i < maxLen; ++i)
    {
        if (i != 0)
        {
            sentence << " ";
        }
        sentence << getWord ();
    }
    return sentence.str ();
}

void Test::getWord (std::string & s)
{
    int maxLen = 2+rand ()%18;
    for (int i = 0; i < maxLen; ++i)
    {
        char c = '!' + rand ()%93;
        s += c;
    }
}

void Test::getSentence (std::string & s)
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

} // lockfree
