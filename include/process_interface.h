#ifndef PROCESS_INTERFACE_H
#define PROCESS_INTERFACE_H
#include "process_helper.h"
#include <iostream>
namespace process
{
    class IProcess
    {

    public:
        virtual ~IProcess() = default;
        virtual void work()         = 0;

    protected:
        void setupSignalHandling()
        {
            signal(SIGTERM, IProcess::signalHandler);
            signal(SIGINT, IProcess::signalHandler);
        }

        std::atomic<bool> continue_{true};

    private:
        static void signalHandler(int signum)
        {
            std::cout << "Process " << getpid() << " received signal " << signum << std::endl;
            exit(signum);
        }

    };
} // namespace process

#endif // PROCESS_INTERFACE_H