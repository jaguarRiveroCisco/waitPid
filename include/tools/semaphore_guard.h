#ifndef TOOLS_SEMAPHORE_GUARD_H
#define TOOLS_SEMAPHORE_GUARD_H

#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <stdexcept>
#include <string>

namespace tools
{

    class SemaphoreGuard 
    {
    public:
        static SemaphoreGuard &getInstance();
        void lock();
        void unlock();
        // Separate function to unlink semaphore when appropriate
        static void unlinkSemaphore();
        const std::string &getName() const { return sem_name; }

    private:
        SemaphoreGuard();
        ~SemaphoreGuard();
        SemaphoreGuard(const SemaphoreGuard &)            = delete;
        SemaphoreGuard &operator=(const SemaphoreGuard &) = delete;
        static std::string sem_name;
        sem_t      *sem;
    };

    struct locker 
    {
        SemaphoreGuard *sem_;

        locker(SemaphoreGuard *sem) : sem_(sem) { sem_->lock(); }
        ~locker() { sem_->unlock(); }
    };

} // namespace tools

#endif // TOOLS_SEMAPHORE_GUARD_H
