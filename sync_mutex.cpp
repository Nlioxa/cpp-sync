#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <list>

// #define USE_SYNC

std::mutex routine_mutex;

void routine(int &counter, int times)
{
    while (times--)
    {
    #ifdef USE_SYNC
        /** wait until is succeeded acquiring ownership 
         * of the scope via mutex*/
        std::scoped_lock<std::mutex> lock(routine_mutex);
    #endif
        /** wait for a while to imitate some work 
         * being done by the thread*/
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        /** modify the shared variable*/
        counter++;
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <threads_num> <times_to_calc>\n";
        exit(-1);
    }

    auto const threads_num{atoi(argv[1])};
    auto const times_to_calc{atoi(argv[2])};
    
    auto counter{0};

    std::list<std::thread> threads;

    // 1. Make threads
    for (auto t{0}; t < threads_num; t++)
    {
        threads.emplace_back(routine, std::ref(counter), times_to_calc);
    }

    // 2. Wait for threads to finish
    for (auto &thread : threads)
    {
        thread.join();
    }

    std::cout << "counter = " << counter << '\n';
}
