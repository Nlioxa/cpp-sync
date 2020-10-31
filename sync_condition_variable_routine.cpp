#include <iostream>
#include <thread>
#include <chrono>
#include <list>

#define USE_SYNC
#include "wallet.hpp"


void deposit(Wallet &wallet, int const &transfer_limit)
{
    while (true)
    {
        auto amount = rand() % transfer_limit;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        if (wallet.Deposit(amount))
        {
            std::cout << "D(" << std::this_thread::get_id() << ")"
                      << ": deposit(" << amount << ")"
                      << ", have(" << wallet.balance() << ")\n";
        }
        else
        {
            std::cout << "D(" << std::this_thread::get_id() << ")"
                      << ": waiting...\n";
        }
    }
}

void withdraw(Wallet &wallet, int const &transfer_limit)
{
    while (true)
    {
        auto amount = rand() % transfer_limit;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        if (wallet.Withdraw(amount))
        {
            std::cout << "W(" << std::this_thread::get_id() << ")"
                      << ": withdraw(" << amount << ")"
                      << ", left(" << wallet.balance() << ")\n";
        }
        else
        {
            std::cout << "W(" << std::this_thread::get_id() << ")"
                      << ": waiting...\n";
        }
    }
}

int main(int argc, char **argv)
{
    Wallet wallet(0);
    auto const transfer_limit{100};

    auto const
        depositors_num = 2,
        withdrawer_num = 1;

    std::list<std::thread>
        threads;

    // 1. Make threads 
    for (auto t{0}; t < depositors_num; t++)
    {
        threads.emplace_back(deposit, std::ref(wallet), std::ref(transfer_limit));
    }

    for (auto t{0}; t < withdrawer_num; t++)
    {
        threads.emplace_back(withdraw, std::ref(wallet), std::ref(transfer_limit));
    }

    // 2. Wait for threads to finish
    for (auto &thread : threads)
    {
        thread.join();
    }
}
