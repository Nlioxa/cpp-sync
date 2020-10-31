#ifndef WALLET_HPP
#define WALLET_HPP

#include <shared_mutex>
#include <condition_variable>


class Wallet
{
public:
    Wallet(int balance = 0) : m_balance(balance) {}

    bool Deposit(int amount)
    {
#ifdef USE_SYNC
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        deposit_cnd.wait(lock, [&] { return m_insufficient_funds; });
#else
        if (m_insufficient_funds)
#endif
        {
            m_balance += amount;
            m_insufficient_funds = false;
#ifdef USE_SYNC
            withdraw_cnd.notify_one();
#endif
            return true;
        }
        return false;
    }

    bool Withdraw(int amount)
    {
#ifdef USE_SYNC
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        withdraw_cnd.wait(lock, [&] {
            if (m_insufficient_funds = amount > m_balance)
            {
                deposit_cnd.notify_one();
                return false;
            }
            else
            {
                return true;
            }
        });
#else
        if (m_insufficient_funds = amount > m_balance, !m_insufficient_funds)
#endif
        {
            m_balance -= amount;
            return true;
        }
        return false;
    }

    int balance() const
    {
#ifdef USE_SYNC
        std::shared_lock lock(m_mutex);
#endif
        return m_balance;
    }

private:
    int m_balance = 0;
    bool m_insufficient_funds = false;

    mutable std::shared_mutex m_mutex;
    std::condition_variable_any withdraw_cnd;
    std::condition_variable_any deposit_cnd;
};

#endif