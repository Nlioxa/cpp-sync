#include <queue>
#include "semaphore.hpp"

namespace udt
{
    template<typename Task>
    class TasksQueue
    {
    public:
        TasksQueue(size_t capacity = 1) : m_capacity(capacity), m_take(0), m_add(capacity) {}

        Task Take()
        {
            auto task = Task{};
            /** Wait until the queue has any task
             * by attempting to decrement the take_semaphore's count
            */
            m_take.acquire();
            {
                /** get a task atomically from the tasks queue*/
                std::scoped_lock<std::mutex> lock(m_mutex);
                task = m_tasks.front();
                m_tasks.pop();
            }
            /** signal the add_semaphore a slot freed for a new task */
            m_add.release();
            return task;
        }

        void Add(Task const& task)
        {
            /** Wait until the queue has a free slot for a new task
             * by attempting to decrement the add_semaphore's count */
            m_add.acquire();
            {
                /** add a new task atomically to the tasks queue */
                std::scoped_lock<std::mutex> lock(m_mutex);
                m_tasks.emplace(task);
            }
            /** signal the take_semaphore a new task added */
            m_take.release();
        }

        size_t Count()
        {
            std::scoped_lock<std::mutex> lock(m_mutex);
            return m_tasks.size();
        }

    private:
        std::mutex m_mutex;
        udt::counting_semaphore m_add;
        udt::counting_semaphore m_take;
        std::queue<Task> m_tasks;
        size_t m_capacity;
    };
} // namespace udt