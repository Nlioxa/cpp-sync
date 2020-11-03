#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

#define USE_SYNC

std::mutex transfer_mutex;
std::condition_variable cond;

/** the consumer thread does*/
void consume(bool &task_added)
{
#ifdef USE_SYNC
    /** wait until the producer adds a task*/
    std::unique_lock<std::mutex> lock(transfer_mutex);
    cond.wait(lock, [&] { return task_added; });
#endif
    if (task_added)
    {
        std::cout << "consumer: finished task\n";
        task_added = false;
    }
    else
    {
        std::cout << "consumer: no task\n";
    }
}
/** the producer thread does*/
void produce(bool &task_added)
{
#ifdef USE_SYNC
    std::lock_guard<std::mutex> lock(transfer_mutex);
#endif

    std::cout << "producer: added task\n";
    task_added = true;

#ifdef USE_SYNC
    /** notify the condition variable that
     * the task is added successfully
    */
    cond.notify_one();
#endif
}

int main()
{
    auto task_added{false};

    // 1. Make threads
    auto producer_thread{std::thread{produce, std::ref(task_added)}};
    auto consumer_thread{std::thread{consume, std::ref(task_added)}};

    // 2. Wait for threads to finish
    producer_thread.join();
    consumer_thread.join();
}