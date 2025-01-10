#ifndef PROCESS_INTERFACE_H
#define PROCESS_INTERFACE_H
#include "process_helper.h"
#include <iostream>
namespace process
{
    class IProcess
    {

    public:
        virtual ~IProcess()
        {
            auto endTime  = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_).count();
                std::cout << " :( Child process " << getpid() << " lifetime: " << duration << " milliseconds." << std::endl;
        }

        virtual void work()         = 0;

    protected:
        IProcess() = default;
        void setupSignalHandling()
        {
            signal(SIGTERM, IProcess::signalHandler);
            signal(SIGINT, IProcess::signalHandler);
        }

        std::atomic<bool> continue_{true};
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime_ = std::chrono::high_resolution_clock::now();

    private:
        static void signalHandler(int signum)
        {
            std::cout << "Process " << getpid() << " received signal " << signum << std::endl;
            exit(signum);
        }

    };
} // namespace process

#endif // PROCESS_INTERFACE_H