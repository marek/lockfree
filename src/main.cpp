#include <iomanip>
#include <memory>
#include <iostream>
#include <vector>

#include "direct.h"
#include "multipledirect.h"
#include "worker.h"
#include "multipleworker.h"
#include "multiplelockfree.h"
#include "optimallockfree.h"

using namespace lockfree;


template <typename T>
std::shared_ptr<Test> createInstance (int logLines)
{
    return std::dynamic_pointer_cast<Test>(
        std::make_shared<T> (logLines, 5)
    );
}

int main ()
{
    std::vector<std::function<std::shared_ptr<Test>(int)>> tests = {
        &createInstance<DirectWriteTest>,
        &createInstance<MultipleDirectWriteTest>,
        &createInstance<WorkerTest>,
        &createInstance<MultipleWorkerTest>,
        &createInstance<MultipleLockFreeTest>,
        &createInstance<OptimalLockFreeTest>
    };

    int sample_sizes [] = {
        5000,
        10000,
        25000,
        100000,
        250000,
        500000,
        1000000
    };

    std::cout << std::setw(30) << std::left
              << "[ TESTNAME ]";

    for (auto size: sample_sizes)
    {
        std::cout << std::setw(16) << std::right
                  << size;
    }
    std::cout << std::endl;

    for (auto & creator : tests)
    {
        bool first = true;
        for (auto size : sample_sizes)
        {
            auto test = creator (size);
            if (first)
            {
                std::string testName =  "[ " + test->name () + " ]";
                std::cout << std::setw(30) << std::left
                          << testName;
                first = false;
            }

            test->run ();

            std::string testTime = std::to_string(test->duration ().count ()) + "s";
            std::cout << std::setw(16) << std::right
                      << testTime;
            std::cout.flush ();
        }
        std::cout << std::endl;
    }

    return 0;
}
