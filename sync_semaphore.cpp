#include <iostream>
#include <thread>
#include <string>
#include <list>
#include "tasksqueue.hpp"

const auto queue_capacity{10};

void produce(udt::TasksQueue<std::string> &tasks)
{
    while (true)
    {
        /** postpone the thread execution 
         * to simulate randomness of the appearance of a new task */
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 2000));
        /** generate a new improvised task*/
        auto task_to_produce = "task #" + std::to_string((rand() % 1000));
        /** add the task to the queue synchronously */
        tasks.Add(task_to_produce);
        std::cout << "producer: added " << task_to_produce
                  << " total(" << tasks.Count() << ")"
                  << "\n";
    }
}

void consume(udt::TasksQueue<std::string> &tasks)
{
    while (true)
    {
        /** try to take a task from the queue synchronously
         * wait until it has any accessible task */
        auto task_to_consume = tasks.Take();
        /** postpone the thread execution 
         * to simulate the task completion */
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1500));
        std::cout << "consumer: finished " << task_to_consume
                  << " total(" << tasks.Count() << ")"
                  << "\n";
    }
}

int main(int argc, char **argv)
{
    const int
        producers_num = 2,
        consumers_num = 1,
        tasks_max_num = 10;

    udt::TasksQueue<std::string> tasks(tasks_max_num);

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