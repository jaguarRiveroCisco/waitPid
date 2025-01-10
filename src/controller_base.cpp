#include "controller_base.h"

namespace process
{
    int         ControllerBase::numProcesses_ = 4; // Default number of processes
    std::string ControllerBase::processType_  = "simul"; // Default process type
    bool        ControllerBase::running_      = true;
    bool        ControllerBase::respawn_      = true;
    std::vector<std::unique_ptr<ControllerBase>> ControllerBase::handlers_;

    Synchro *ControllerBase::synchro()
    {
        static Synchro instance;
        return &instance;
    }
    
    std::string ControllerBase::receiveCreationMessage() { return Communicator::getInstance().receiveCreationMessage(); }

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
        pid_ = fork();
        if (pid_ == 0)
        {
            try
            {
                process_->work();
            }
            catch (const std::exception &e)
            {
                // Handle exceptions in child process
                std::cerr << "Exception in child process: " << e.what() << std::endl;
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

    void ControllerBase::setProcessType(const std::string &processType) { processType_ = processType; }

    void  ControllerBase::displayAllPids()
    {
        std::cout << "Current PIDs:" << std::endl;
        for (const auto &handler: handlers_)
        {
            std::cout << handler->getPid() << std::endl;
        }
        std::cout << "Total number of processes: " << handlers_.size() << std::endl;
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
            std::cerr << "Process with PID: " << pid << " not found." << std::endl;
        }
    }
    
    void ControllerBase::terminateAll()
    {
        for (auto &handler: handlers_)
        {
            handler->terminateProcess();
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
            std::cerr << "Process with PID: " << pid << " not found." << std::endl;
        }
    }
}