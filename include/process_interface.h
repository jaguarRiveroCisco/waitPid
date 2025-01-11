#ifndef PROCESS_INTERFACE_H
#define PROCESS_INTERFACE_H
#include "process_helper.h"
#include <iostream>
namespace process
{
    class IProcess 
    {
    public:
        virtual ~IProcess() {}
        virtual void work() = 0;

    protected:
        IProcess() = default;
        void setupSignalHandling()
        {
            signal(SIGTERM, IProcess::signalHandler);
            signal(SIGINT, IProcess::signalHandler);
        }
    private:
        static void signalHandler(int signum)
        {
            std::cout << "Process " << getpid() << " received signal " << signum << std::endl;
            exit(signum);
        }
    };

    class BaseProcess : public IProcess 
    {
    public:
        virtual ~BaseProcess() = default;

    protected:
        std::atomic<bool>                                           continue_{true};
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime_ = std::chrono::high_resolution_clock::now();

        BaseProcess() = default;

        void logLifetime() const
        {
            auto endTime  = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_).count();
            std::cout << "Process " << getpid() << " lifetime: " << duration << " milliseconds." << std::endl;
        }
    };
} // namespace process

#endif // PROCESS_INTERFACE_H