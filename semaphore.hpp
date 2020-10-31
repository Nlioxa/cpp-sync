#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>

namespace udt // User Defined Types
{
    class counting_semaphore
    {
    public:
        counting_semaphore(ptrdiff_t desired) : m_counter(desired) {}
        counting_semaphore(counting_semaphore const &) = delete;
        
        counting_semaphore &operator=(counting_semaphore const &) = delete;

        void release(ptrdiff_t update = 1)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_counter += update;
            m_cond.notify_one();
        }

        void acquire()
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cond.wait(lock, [this] { return m_counter > 0; });
            m_counter--;
        }

    private:
        ptrdiff_t m_counter;
        std::mutex m_mutex;
        std::condition_variable m_cond;
    };
} // namespace udt

#endif // SEMAPHORE_HPP