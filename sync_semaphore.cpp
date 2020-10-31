#include <iostream>
#include <thread>
#include <queue>
#include <string>
#include <list>
#include "semaphore.hpp"

const auto queue_capacity{10};

udt::counting_semaphore
    produce_sem(queue_capacity),
    consume_sem(0);
std::mutex tasks_mtx;

void produce(std::queue<std::string> &tasks)
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 2000));
        auto task_to_produce = "task #" + std::to_string((rand() % 1000));

        produce_sem.acquire();
        {
            std::lock_guard<std::mutex> lock(tasks_mtx);
            tasks.emplace(task_to_produce);
        }
        consume_sem.release();

        std::cout << "producer: added " << task_to_produce
                  << " total(" << tasks.size() << ")"
                  << "\n";
    }
}

void consume(std::queue<std::string> &tasks)
{
    while (true)
    {
        auto task_to_consume = std::string{};

        consume_sem.acquire();
        {
            std::lock_guard<std::mutex> lock(tasks_mtx);
            task_to_consume = tasks.front();
            tasks.pop();
        }
        produce_sem.release();

        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1500));
        std::cout << "consumer: finished " << task_to_consume
                  << " total(" << tasks.size() << ")"
                  << "\n";
    }
}

int main()
{
    const int
        producers_num = 2,
        consumers_num = 1;

    std::queue<std::string> tasks;

    std::list<std::thread>
        threads;

    // 1. Make threads
    for (auto t{0}; t < producers_num; t++)
    {
        threads.emplace_back(produce, std::ref(tasks));
    }

    for (auto t{0}; t < consumers_num; t++)
    {
        threads.emplace_back(consume, std::ref(tasks));
    }

    // 2. Wait for threads to finish
    for (auto &thread : threads)
    {
        thread.join();
    }
}