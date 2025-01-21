#include "base_handler.h"
#include <chrono>
#include <csignal>
#include <thread>
#include <unistd.h> // Include this header for _exit
#include "logger_interface.h"

namespace process
{

    void BaseHandler::displayProcessStatus(int &status, const std::string& tid)
    {
        if (WIFSTOPPED(status))
        {
            tools::LoggerManager::getInstance().logWarning("[PARENT PROCESS][MONITORING THREAD] " + tid + " [END] Child process " + std::to_string(pid_) + " was stopped by signal " + std::to_string(WSTOPSIG(status)) + ".");}
        else if (WIFEXITED(status))
        {
            tools::LoggerManager::getInstance().logInfo("[PARENT PROCESS][MONITORING THREAD] " + tid + " [END] Child process " + std::to_string(pid_)
                                                        + " exited normally with status " + std::to_string(WEXITSTATUS(status)) + ".");
        }
        else if (WIFSIGNALED(status))
        {
            tools::LoggerManager::getInstance().logWarning("[PARENT PROCESS][MONITORING THREAD] " + tid + " [END] Child process " + std::to_string(pid_)
                                                           + " was terminated by signal " + std::to_string(WTERMSIG(status)) + ".");
        }
        else
        {
            tools::LoggerManager::getInstance().logError("[PARENT PROCESS][MONITORING THREAD] " + tid + " [END] Child process " + std::to_string(pid_)
                                                           + " exited with an unknown status.");
        }
    }
    
    pid_t BaseHandler::getPid() const { return pid_; }

    bool BaseHandler::isProcessRunning()
    {
        if (kill(pid_, 0) == -1 && errno == ESRCH)
        {
            tools::LoggerManager::getInstance().logError("[PARENT PROCESS] Process " + std::to_string(pid_) + " is not running.");
            return false;
        }
        return true;
    }

    void BaseHandler::terminateProcess() { sendSignal(SIGTERM); }

    void BaseHandler::killProcess() { sendSignal(SIGKILL); }

    void BaseHandler::intProcess() { sendSignal(SIGINT); }

    void BaseHandler::sendSignal(int signal)
    {
        if (kill(pid_, signal) == -1)
        {
            perror("kill");
        }
    }

    void BaseHandler::createMonitorProcessThread()
    {
        // Parent process
        // Create a thread to check the state of the child process
        try
        {
            std::thread checkThread(&BaseHandler::monitorProcessThread, this);
            checkThread.detach();
        }
        catch (const std::system_error &e)
        {
            tools::LoggerManager::getInstance() << "[PARENT PROCESS] Thread creation failed: " << e.what();
            tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
            _exit(EXIT_FAILURE); // Ensure the child process exits
        }
    }

    void BaseHandler::stopMonitoring()
    {
        static std::mutex monitorMutex_;
        std::lock_guard<std::mutex> lock(monitorMutex_);
        if(!monitoring_)
            return;
        monitoring_ = false;
        while (threadRunning_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(Nap::TIME));
        }
    }

    void BaseHandler::monitorProcessThread()
    {
        monitoring_              = true;
        threadRunning_           = true;
        int               status = -1;
        std::stringstream ss;
        ss << std::this_thread::get_id();
        tools::LoggerManager::getInstance().logInfo("[PARENT PROCESS][MONITORING THREAD] " + ss.str() + " [START] for process " + std::to_string(pid_));
        tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
        while (monitoring_)
        {
            // Check if the process with PID = pid_ is running
            if (!isProcessRunning())
                break;

            pid_t result = waitpid(pid_, &status, WNOHANG);
            if (result == 0)
            {
                // Child still running
                std::this_thread::sleep_for(std::chrono::milliseconds(Nap::TIME));
            }
            else if (result == pid_)
            {
                displayProcessStatus(status, ss.str());
                concurrency::Synchro::getInstance().pushPid(pid_);
                break;
            }
            else
            {
                // Error occurred
                perror("waitpid");
                break;
            }
        }
        monitoring_ = false;
        threadRunning_ = false;
    }
} // namespace process
