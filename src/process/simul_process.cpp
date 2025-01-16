#include "simul_process.h"
#include <chrono>
#include <ctime>
#include <thread>
#include <unistd.h>
#include "communicator.h"
#include "logger_instance.h"
namespace process
{
    int           ProcessSimulator::rndUpper_ = 20;
    constexpr int baseSleepDuration           = 10;

    void ProcessSimulator::preWork()
    {
        tools::LoggerManager::createLoggerType();
        setSleepDuration();
        Communicator::getInstance().sendCreationMessage(sleepDuration_);
        msSleepDuration_ = sleepDuration_ * 1000;

        tools::LoggerManager::getInstance().logInfo(
                "[PROCESS EXECUTING] | Simulated Process started. Duration : " + std::to_string(sleepDuration_) +
                " seconds (" + std::to_string(msSleepDuration_) + " ms)");
    }

    void ProcessSimulator::postWork()
    {
        // Add post-work code here

        logLifetime();
        _exit(exitCode_); // Ensure the child process exits immediately
    }

    void ProcessSimulator::setRndUpper(int rndUpper)
    {
        rndUpper_ = rndUpper;
    }

    void ProcessSimulator::setSleepDuration()
    {
        std::srand(std::time(nullptr) ^ getpid()); // Seed using time and PID for unique seeds
        sleepDuration_ = std::rand() % rndUpper_ + baseSleepDuration; // Random sleep duration between 1 and x seconds
    }

    void ProcessSimulator::work()
    {
        preWork();

        auto endTime = startTime_ + std::chrono::milliseconds(msSleepDuration_);

        // Maximum allowed lifetime to prevent indefinite execution
        auto maxLifetime   = std::chrono::milliseconds(msSleepDuration_ + 5000); // Add a buffer to the sleep duration
        auto currentTime   = std::chrono::high_resolution_clock::now();

        while (continue_)
        {
            if (currentTime >= endTime)
            {
                break;
            }

            if (currentTime - startTime_ >= maxLifetime)
            {
                reason_ = "Maximum allowed lifetime exceeded";
                break;
            }

            if (getppid() == 1)
            {
                reason_ = "Parent process has terminated. Exiting child process.";
                break;
            }


            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate some work
            currentTime = std::chrono::high_resolution_clock::now();
        }
        postWork();
    }
} // namespace process
