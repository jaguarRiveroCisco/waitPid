#ifndef BASE_PROCESS_H
#define BASE_PROCESS_H

#include "process_interface.h"
namespace process
{

    class BaseProcess : public IProcess 
    {
    public:
        virtual ~BaseProcess() = default;

    protected:
        std::atomic<bool>                                           continue_{true};
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime_ =
                std::chrono::high_resolution_clock::now();

        BaseProcess() = default;

        void logLifetime(const std::string &reason = "End of life") const;

    protected:
        void setupSignalHandling();

    private:
        static void signalHandler(int signum);
    };
}

#endif // BASE_PROCESS_H