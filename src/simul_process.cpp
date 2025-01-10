
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
        
        startTime_ = std::chrono::high_resolution_clock::now();
        auto endTime = startTime_ + std::chrono::seconds(sleepDuration_);

        std::cout << "Simulated process is working: " << getpid() << " Sleep duration " << sleepDuration_ << std::endl;
        // Child process
        while (continue_)
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            if (currentTime >= endTime)
            {
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate some work
        }
        auto lifetime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - startTime_).count();
        std::cout << "Simulated process is done: " << getpid() << " Lifetime: " << lifetime << " seconds." << std::endl;
        _exit(0); // Ensure the child process exits immediately
    }
} // namespace process
