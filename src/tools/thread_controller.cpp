#include "thread_controller.h"

namespace tools
{
    void ThreadController::run(std::function<void(const std::string &)> commandFunc)
    {
        this->commandFunc = std::move(commandFunc);
        stopFlag          = false;
        readerThread      = std::thread(&ThreadController::runThread, this);
    }

    void ThreadController::stop()
    {
        stopFlag = true;
        if (readerThread.joinable())
        {
            readerThread.join();
        }
    }
}