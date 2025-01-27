#include "base_handler.h"
#include <chrono>
#include <csignal>
#include <thread>
#include <unistd.h> // Include this header for _exit
#include "logger_instance.h"
#include "nap_time.h"

namespace process
{
    std::atomic<bool> ProcessMonitor::running_ = true;
    std::atomic<bool> ProcessMonitor::respawn_ = true;

    void ProcessMonitor::displayProcessStatus(int &status)
    {
        if (WIFEXITED(status))
        {
        }
        else if (WIFSIGNALED(status))
        {
            tools::LoggerManager::getInstance().logWarning("[PARENT PROCESS] | Child process " + std::to_string(pid_)
                                                           + " was terminated by signal " + std::to_string(WTERMSIG(status)) + ".");
        }
        else
        {
            tools::LoggerManager::getInstance().logWarning("[PARENT PROCESS] | Child process " + std::to_string(pid_) + " exited with an unknown status.");
        }
    }
    
    pid_t ProcessMonitor::getPid() const { return pid_; }

    bool ProcessMonitor::isProcessRunning()
    {
        if (kill(pid_, 0) == -1 && errno == ESRCH)
        {
            tools::LoggerManager::getInstance() << "[PARENT PROCESS] | Process " << pid_ << " is not running.";
            tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
            return false;
        }
        return true;
    }

    void ProcessMonitor::terminateProcess() { sendSignal(SIGTERM); }

    void ProcessMonitor::killProcess() { sendSignal(SIGKILL); }

    void ProcessMonitor::intProcess() { sendSignal(SIGINT); }

    void ProcessMonitor::sendSignal(int signal)
    {
        if (kill(pid_, signal) == -1)
        {
            perror("kill");
        }
    }

    bool ProcessMonitor::monitoring() const
    {
        return monitor_;
    }

    void ProcessMonitor::monitoring(bool value) 
    
    { 
        monitor_ = value;
    }

    bool ProcessMonitor::running() { return running_; }

    bool ProcessMonitor::respawn() { return respawn_; }

    void ProcessMonitor::running(bool value) { running_ = value; }

    void ProcessMonitor::respawn(bool value) { respawn_ = value; }

    void ProcessMonitor::createMonitorProcessThread()
    {
        if( monitoring() )
        {
            tools::LoggerManager::getInstance().logWarning("[PARENT PROCESS] | Monitoring thread already running.");
            return;
        }
        // Parent process
        // Create a thread to check the state of the child process
        try
        {
            std::thread checkThread(&ProcessMonitor::monitorProcessThread, this);
            checkThread.detach();
        }
        catch (const std::system_error &e)
        {
            tools::LoggerManager::getInstance() << "[PARENT PROCESS] | Thread creation failed: " << e.what();
            tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
            exit(EXIT_FAILURE); // Ensure the child process exits
        }
    }

    void ProcessMonitor::monitorProcessThread()
    {
        int  status   = -1;
        monitoring(true);
        while (monitoring())
        {
            // Check if the process with PID = pid_ is running
            if (!isProcessRunning())
                break;

            pid_t result = waitpid(pid_, &status, WNOHANG);
            if (result == 0)
            {
                // Child still running
                std::this_thread::sleep_for(std::chrono::milliseconds(tools::NapTimeMs::SHORT));
            }
            else if (result == pid_)
            {
                break;
            }
            else
            {
                // Error occurred
                perror("waitpid");
                break;
            }
        }
        monitoring(false);
        displayProcessStatus(status);
        concurrency::Synchro::getInstance().pushPid(pid_);
    }

} // namespace process
