
#include "simul_process.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>
#include <unistd.h>
#include "process_helper.h"
namespace process
{
    int           ProcessSimulator::rndUpper_ = 20;
    constexpr int baseSleepDuration       = 10;

    void ProcessSimulator::setRndUpper(int rndUpper)
    {
        rndUpper_ = rndUpper;
        std::cout << "Children process will simulate work for a random duration between " << baseSleepDuration
                  << " and " << baseSleepDuration + rndUpper_ << " seconds." << std::endl;
    }
    void ProcessSimulator::setSleepDuration()
    {
        std::srand(std::time(nullptr) ^ getpid()); // Seed using time and PID to ensure different seeds
        sleepDuration_ = std::rand() % rndUpper_ + baseSleepDuration; // Random sleep duration between 1 and x seconds
    }

    void ProcessSimulator::work()
    {
        setSleepDuration();
        Communicator::getInstance().sendCreationMessage(sleepDuration_);

        auto msSleepDuration = sleepDuration_ * 1000;

        auto endTime = startTime_ + std::chrono::milliseconds(msSleepDuration);

        std::cout << "Simulated process is working: " << getpid() << " Sleep duration " << msSleepDuration << " ms"
                  << std::endl;

        // Maximum allowed lifetime to prevent indefinite execution
        auto maxLifetime = std::chrono::milliseconds(msSleepDuration + 5000); // Add a buffer to the sleep duration
        auto currentTime = std::chrono::high_resolution_clock::now();

        while (continue_)
        {
            if (currentTime >= endTime)
            {
                std::cout << getpid() << " Ending simulation: Reached end time " << "Sleep duration: " << msSleepDuration << " ms\n";
                break;
            }

            if (currentTime - startTime_ >= maxLifetime)
            {
                std::cout << getpid() << " Ending simulation: Reached maximum allowed lifetime " << maxLifetime.count() << " ms\n";
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate some work
            currentTime = std::chrono::high_resolution_clock::now();
        }
        logLifetime();
        _exit(0); // Ensure the child process exits immediately
    }
} // namespace process
