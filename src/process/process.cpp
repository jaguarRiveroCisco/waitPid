#include "process.h"
#include <iostream>
#include <thread>
#include "communicator.h"
#include "logger_instance.h"

namespace process
{
    void Process::preWork()
    {
        tools::LoggerManager::createLoggerType();
        // Real process work implementation
        Communicator::getInstance().sendCreationMessage();
        tools::LoggerManager::getInstance() << "[PROCESS EXECUTING] | Process work started";
        tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
    }
    void Process::postWork()
    {
        logLifetime();
        _exit(exitCode_); // Ensure the child process exits immediately
    }

    void Process::work()
    {
        preWork();
        // Add real process work code here
        while (continue_)
        {
            /* code */
            if (getppid() == 1)
            {
                reason_ = "Parent process has terminated. Exiting child process.";
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate some work
        }
        postWork();
    }
} // namespace process
