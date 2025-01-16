#include "controller_base.h"
#include <unistd.h> // Include this header for fork
#include "logger_instance.h"
namespace process
{
    int         ControllerBase::numProcesses_ = 4; // Default number of processes
    std::string ControllerBase::processType_  = "simul"; // Default process type
    bool        ControllerBase::running_      = true;
    bool        ControllerBase::respawn_      = true;
    std::vector<std::unique_ptr<ControllerBase>> ControllerBase::handlers_;
    // Initialize static members
    LoggingType ControllerBase::loggingType_ = LoggingType::Console;


    Synchro *ControllerBase::synchro()
    {
        static Synchro instance;
        return &instance;
    }
    
    void ControllerBase::init(Synchro *synchro, std::unique_ptr<IProcess> process)
    {
        if (synchro == nullptr)
        {
            throw std::runtime_error("Synchro object is null");
        }
        synchro_ = synchro;
        process_ = std::move(process);
        createChild();
    }
    
    void ControllerBase::createChild()
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

        // Your code that uses vfork()
        pid_t pid = vfork();

#pragma clang diagnostic pop
        if (pid_ == 0)
        {
            //setupSignalHandling();
            try
            {
                process_->work();
            }
            catch (const std::exception &e)
            {
                // Handle exceptions in child process
                tools::LoggerManager::getInstance() << "Exception in child process: " << e.what();
                tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
                _exit(EXIT_FAILURE); // Ensure child process exits
            }
        }
        else if (pid_ < 0)
        {
            // Fork failed
            perror("fork");
            throw std::runtime_error("Failed to fork process");
        }
        else
        {
            // Parent process
            // std::cout << "Parent process created child process with PID: " << pid_ << std::endl;
        }
    }

    void  ControllerBase::start() { createCheckProcessThread(); }
    
    bool &ControllerBase::running() { return running_; }

    bool &ControllerBase::respawn() { return respawn_; }

    LoggingType &ControllerBase::loggingType() { return loggingType_; }

    // Implementation of the new method
    std::string ControllerBase::loggingTypeToString()
    {
        switch (loggingType_)
        {
            case LoggingType::Console:
                return "Console";
            case LoggingType::File:
                return "File";
            default:
                return "Unknown";
        }
    }

    void ControllerBase::setProcessType(const std::string &processType) { processType_ = processType; }

    void  ControllerBase::displayAllPids()
    {
        tools::LoggerManager::getInstance() << "Current PIDs: ";
        for (const auto &handler: handlers_)
        {
            tools::LoggerManager::getInstance() << handler->getPid() << " | ";
        }
        tools::LoggerManager::getInstance() << " Total number of processes: " << handlers_.size();
        tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
    }

    void ControllerBase::killAll()
    {
        for (auto &handler: handlers_)
        {
            handler->killProcess();
        }
    }

    void ControllerBase::killProcessByPid(pid_t pid)
    {
        auto it = std::find_if(handlers_.begin(), handlers_.end(),
                               [pid](const std::unique_ptr<ControllerBase> &handler) { return handler->getPid() == pid; });
        if (it != handlers_.end())
        {
            (*it)->killProcess();
        }
        else
        {
            tools::LoggerManager::getInstance() << "Process with PID: " << pid << " not found.";
            tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
        }
    }
    
    void ControllerBase::terminateAll()
    {
        for (auto &handler: handlers_)
        {
            handler->terminateProcess();
        }
    }

    void ControllerBase::intAll()
    {
        for (auto &handler: handlers_)
        {
            handler->intProcess();
        }
    }

    void ControllerBase::terminateProcessByPid(pid_t pid)
    {
        auto it = std::find_if(handlers_.begin(), handlers_.end(),
                               [pid](const std::unique_ptr<ControllerBase> &handler) { return handler->getPid() == pid; });
        if (it != handlers_.end())
        {
            (*it)->terminateProcess();
        }
        else
        {
            tools::LoggerManager::getInstance() << "Process with PID: " << pid << " not found.";
            tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
        }
    }

    void ControllerBase::intProcessByPid(pid_t pid)
    {
        auto it = std::find_if(handlers_.begin(), handlers_.end(),
                               [pid](const std::unique_ptr<ControllerBase> &handler) { return handler->getPid() == pid; });
        if (it != handlers_.end())
        {
            (*it)->intProcess();
        }
        else
        {
            tools::LoggerManager::getInstance() << "Process with PID: " << pid << " not found.";
            tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
        }
    }
}