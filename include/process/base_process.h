#ifndef BASE_PROCESS_H
#define BASE_PROCESS_H

#include "process_interface.h"
#include <memory>
#include <string>
namespace process
{

    void signalHandler(int signum);
    void setupSignalHandling();

    class BaseProcess : public IProcess 
    {
    public:

        static std::atomic<bool> &continueFlag();
        static int               &exitCode();

    protected:
        virtual ~BaseProcess() = default;
        BaseProcess();
        void logLifetime() const;

        std::chrono::time_point<std::chrono::high_resolution_clock> startTime_ =
                std::chrono::high_resolution_clock::now();
        static std::atomic<bool> continue_;
        static int exitCode_;
        std::string reason_ = "End of life";
    };

} // namespace process

#endif // BASE_PROCESS_H