#include <csignal>
#include <iostream>
#include <thread>
#include "process_base.h"
#include <chrono>

extern std::atomic<bool> g_display;

namespace process
{
    size_t BaseHandler::processCounter_ = 0;

    BaseHandler::BaseHandler() { ++processCounter_; }

    BaseHandler::~BaseHandler()
    {
        --processCounter_;
        if (processCounter_ == 0)
        {
            std::cout << "All processes have finished.\n";
        }
    }

    void BaseHandler::displayProcessStatus(int &status)
    {
        // Child finished
        if (!WIFEXITED(status))
        {
            if (WIFSIGNALED(status))
            {
                std::cout << "Child process " << pid_ << " was terminated by signal " << WTERMSIG(status) << ".\n";
            }
            else
            {
                std::cerr << "Child process " << pid_ << " exited with status " << status << ".\n";
            }
        }
    }

    pid_t BaseHandler::getPid() const { return pid_; }

    bool BaseHandler::isProcessRunning() const
    {
        if (kill(pid_, 0) == -1 && errno == ESRCH)
        {
            std::cerr << "Process " << pid_ << " is not running.\n";
            return false;
        }
        return true;
    }

    void BaseHandler::terminateProcess() { sendSignal(SIGTERM); }

    void BaseHandler::killProcess() { sendSignal(SIGKILL); }

    void BaseHandler::sendSignal(int signal)
    {
        if (kill(pid_, signal) == -1)
        {
            perror("kill");
        }
    }

    void BaseHandler::createCheckProcessThread()
    {
        // Parent process
        // Create a thread to check the state of the child process
        try
        {
            std::thread checkThread(&BaseHandler::checkProcessState, this);
            checkThread.detach();
        }
        catch (const std::system_error &e)
        {
            std::cerr << "Thread creation failed: " << e.what() << std::endl;
        }
    }

    void BaseHandler::checkProcessState()
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
