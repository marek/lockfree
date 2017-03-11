#include <iomanip>
#include <memory>
#include <iostream>
#include <vector>

#include "direct.h"
#include "multipledirect.h"
#include "worker.h"
#include "multipleworker.h"
#include "cb.h"
#include "dualcb.h"
#include "hsspsc.h"
#include "mpsc.h"
#include "tdcb.h"
#include "bulkworker.h"
#include "spsc.h"

using namespace lockfree;


template <typename T>
std::shared_ptr<Test> createInstance (
    const int logLines,
    const int threads,
    const std::string & name)
{
    auto test = std::dynamic_pointer_cast<Test>(
        std::make_shared<T> (logLines, threads)
    );

    if (name.size () > 0)
    {
        test->setName (name);
    }
    return test;
}

template <typename T>
std::function<std::shared_ptr<Test> (int)> makeTest (
    int threads = 5,
    std::string name = ""
)
{
    return [threads,&name] (int logLines) {
        return createInstance<T> (logLines, threads, name);
    };
}

int main ()
{
    Test::useDevNull (false);

    std::vector<std::function<std::shared_ptr<Test> (int)>> tests = {
        makeTest<DirectWriteTest> (),
        makeTest<MultipleDirectWriteTest> (),
        //makeTest<WorkerTest> (),
        makeTest<BulkWorkerTest> (),
        makeTest<MultipleWorkerTest> (),
        //makeTest<MultipleWorkerTest> (25, "worker[25]"),
        //makeTest<MultipleWorkerTest> (50, "worker[50]"),
        //makeTest<CBTest> (),
        //makeTest<DualCBTest> (),
        makeTest<MPSCTest> (),
        makeTest<HSSPSCTest> (),
        makeTest<SPSCTest> (),
        makeTest<SPSCTest> (25, "spsc[25]"),
        makeTest<SPSCTest> (50, "spsc[50]"),
        makeTest<ThreadedDualCBTest> ()
    };

    unsigned sample_sizes [] = {
        5000,
        10000,
        25000,
        100000,
        250000,
        500000,
        1000000,
        5000000
    };

    const unsigned repeats = 1;//10;

    //
    // Print the header
    //
    std::cout << std::setw (30) << std::left
              << "[ TESTNAME ]";

    for (auto size: sample_sizes)
    {
        std::cout << std::setw (16) << std::right
                  << size;
    }
    std::cout << std::endl;

    //
    // Create & run the tests spitting out results
    //
    for (auto & creator : tests)
    {
        bool first = true;
        for (auto size : sample_sizes)
        {
            double time = 0;
            for (unsigned r = 0; r < repeats; ++r)
            {
                auto test = creator (size);
                if (first)
                {
                    std::string testName =  "[ " + test->name () + " ]";
                    std::cout << std::setw (30) << std::left
                              << testName;
                    first = false;
                }

                test->run ();

                time += test->duration ().count ();
            }

            std::string testTime = std::to_string (
                time / repeats
            ) + "s";

            std::cout << std::setw (16) << std::right
                      << testTime;
            std::cout.flush ();

        }
        std::cout << std::endl;
    }

    return 0;
}
