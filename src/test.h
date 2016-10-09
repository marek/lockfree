#pragma once

#include <chrono>
#include <fstream>
#include <string>

namespace lockfree {

typedef std::chrono::duration<double, std::chrono::seconds::period>
    double_seconds;

class Test
{
  public:
    Test (const std::string & testName, int iterations, int threads);
    virtual ~Test () { }

    void start ();
    void stop ();
    double_seconds duration ();

    const std::string & name () { return testName_; }
    int iterations () { return iterations_; }
    int threads () { return threads_; }

    virtual void run () = 0;    // implement test here

    static std::string getWord ();
    static std::string getSentence ();

  protected:

    std::ofstream logFile_;

  private:
    std::string testName_;
    int iterations_;
    int threads_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_;
};


}