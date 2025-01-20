#ifndef SYNCHRO_H
#define SYNCHRO_H
#include <condition_variable>
#include <queue>
#include "logger_instance.h"
#include "base_process.h"

struct Synchro final
{
    std::mutex              mtx;
    std::condition_variable cv;
    std::queue<pid_t>       eventQueue;
    // Delete the new and delete operators to prevent dynamic allocation
    void* operator new(size_t) = delete;
    void operator delete(void*) = delete;

    // Function to get and pop the front element of the queue
    pid_t getAndPopFront()
    {
        std::lock_guard<std::mutex> lock(mymutex_);
        if (!eventQueue.empty())
        {
            pid_t frontElement = eventQueue.front();
            eventQueue.pop();
            return frontElement;
        }
        return -1; // Return -1 if the queue is empty
    }
    private:
        std::mutex mymutex_;
};

#endif // SYNCHRO_H