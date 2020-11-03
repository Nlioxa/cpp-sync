#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

// #define USE_SYNC


std::mutex transfer_mutex;
std::condition_variable cond;

void consume(bool& is_task) {
    #ifdef USE_SYNC
    std::unique_lock<std::mutex> lock(transfer_mutex);
    cond.wait(lock, [&]{ return is_task; });
    #else
    
    if (is_task) 
    #endif
    {
        std::cout << "consumer: complete task\n";
        is_task = false;
    }
}

void produce(bool& is_task) {
    #ifdef USE_SYNC
    std::lock_guard<std::mutex> lock(transfer_mutex);
    #endif

    std::cout << "producer: create task\n";
    is_task = true;
    
    #ifdef USE_SYNC
    cond.notify_one();
    #endif
}

int main() {
    auto is_task{ false };

    // 1. Make threads
    auto producer_thread{ std::thread{ produce, std::ref(is_task) } };
    auto consumer_thread{ std::thread{ consume, std::ref(is_task) } };

    // 2. Wait for threads to finish
    producer_thread.join();
    consumer_thread.join();
}