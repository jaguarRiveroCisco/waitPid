#include "process.h"
#include <iostream>
#include <thread>
#include "communicator.h"
#include "logger_instance.h"

namespace process
{
    void Process::work()
    {
        tools::LoggerManager::createLoggerType(tools::LoggerManager::loggerType());
        // Real process work implementation
        Communicator::getInstance().sendCreationMessage();
        tools::LoggerManager::getInstance() << "[EXECUTING] Process work started";
        tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
        // Add real process work code here
        while (continue_)
        {
            /* code */
            if (getppid() == 1)
            {
                tools::LoggerManager::getInstance() << "Parent process has terminated. Exiting child process.";
                tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate some work
        }
        logLifetime("Process ended");
        _exit(0); // Ensure the child process exits immediately
    }
} // namespace process
