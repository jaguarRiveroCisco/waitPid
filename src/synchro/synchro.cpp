#include "synchro.h"
#include "logger_instance.h"
namespace concurrency
{
    Synchro &Synchro::getInstance()
    {
        static Synchro instance; // Guaranteed to be destroyed, instantiated on first use
        return instance;
    }

    void Synchro::pushPid(pid_t pid)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        pidQueue_.push(pid);
        cv_.notify_one();
    }

    // Print all elements in the queue
    void Synchro::printPidQueue()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        std::queue<pid_t> tempQueue = pidQueue_; // Create a copy of the queue
        while (!tempQueue.empty())
        {
            pid_t pid = tempQueue.front();
            tempQueue.pop();
            tools::LoggerManager::getInstance() << pid << " ";
        }
        tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
    }
    // Get and pop the front element of the queue
    pid_t Synchro::removeFrontPid()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!pidQueue_.empty())
        {
            pid_t frontElement = pidQueue_.front();
            pidQueue_.pop();
            return frontElement;
        }
        return -1; // Return -1 if the queue is empty
    }

    // Wait for an event to be available
    void Synchro::blockUntilPidAvailable()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !pidQueue_.empty(); });
    }

    // Check if the queue is empty
    bool Synchro::isPidQueueEmpty()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return pidQueue_.empty();
    }
} // namespace sync
