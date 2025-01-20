#include "synchro.h"
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
