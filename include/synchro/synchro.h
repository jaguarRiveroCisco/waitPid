#ifndef SYNCHRO_H
#define SYNCHRO_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include "base_process.h"
#include "logger_instance.h"


namespace concurrency
{
    class Synchro final 
    {
    public:
        // Add an event to the queue and notify waiting threads
        void pushPid(pid_t pid);
        // Get and pop the front element of the queue
        pid_t removeFrontPid();

        // Wait for an event to be available
        void blockUntilPidAvailable();
        // Check if the queue is empty
        bool isPidQueueEmpty();

        void printPidQueue();

        static Synchro &getInstance();

    private:
        // Private constructor to prevent instantiation
        Synchro() = default;

        // Delete copy constructor and assignment operator
        Synchro(const Synchro &) = delete;
        Synchro &operator=(const Synchro &) = delete;


        std::mutex              mtx_;
        std::condition_variable cv_;
        std::queue<pid_t>       pidQueue_;
    };
}

#endif // SYNCHRO_H
