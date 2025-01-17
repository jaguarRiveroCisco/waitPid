#include "base_handler.h"
#include <chrono>
#include <csignal>
#include <thread>
#include <unistd.h> // Include this header for _exit
#include "logger_interface.h"

namespace process
{

    void BaseHandler::displayProcessStatus(int &status)
    {
        if (WIFEXITED(status))
        {
            tools::LoggerManager::getInstance() << "[PARENT PROCESS] Child process " << pid_
                                                << " exited normally with status " << WEXITSTATUS(status) << ".";
            tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
        }
        else if (WIFSIGNALED(status))
        {
            tools::LoggerManager::getInstance() << "[PARENT PROCESS] Child process " << pid_
                                                << " was terminated by signal " << WTERMSIG(status) << ".";
            tools::LoggerManager::getInstance().flush(tools::LogLevel::WARNING);
        }
        else
        {
            tools::LoggerManager::getInstance() << "[PARENT PROCESS] Child process " << pid_ << " exited with an unknown status.";
            tools::LoggerManager::getInstance().flush(tools::LogLevel::WARNING);
        }
    }
    
    pid_t BaseHandler::getPid() const { return pid_; }

    bool BaseHandler::isProcessRunning()
    {
        if (kill(pid_, 0) == -1 && errno == ESRCH)
        {
            tools::LoggerManager::getInstance() << "[PARENT PROCESS] Process " << pid_ << " is not running.";
            tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
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

    void BaseHandler::startProcessMonitoringThread()
    {
        // Parent process
        // Create a thread to check the state of the child process
        try
        {
            std::thread checkThread(&BaseHandler::monitorProcessStatus, this);
            checkThread.detach();
        }
        catch (const std::system_error &e)
        {
            tools::LoggerManager::getInstance() << "[PARENT PROCESS] Thread creation failed: " << e.what();
            tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
            _exit(EXIT_FAILURE); // Ensure the child process exits
        }
    }

    void BaseHandler::monitorProcessStatus()
    {
        int status = -1;
        while (true)
        {
            // Check if the process with PID = pid_ is running
            if (!isProcessRunning())
                break;

            pid_t result = waitpid(pid_, &status, WNOHANG);
            if (result == 0)
            {
                // Child still running
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            else if (result == pid_)
            {
                displayProcessStatus(status);
                // Child finished
                {
                    std::lock_guard<std::mutex> lock(synchro_->mtx);
                    synchro_->eventQueue.push(pid_);
                }
                synchro_->cv.notify_one();
                break;
            }
            else
            {
                // Error occurred
                perror("waitpid");
                break;
            }
        }
    }
} // namespace process
