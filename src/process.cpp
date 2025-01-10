#include "process.h"
#include <iostream>
#include "process_helper.h"
#include <thread>
namespace process
{
    void Process::work()
    {
        // Real process work implementation
        Communicator::getInstance().sendCreationMessage();
        std::cout << "Real process is working: " << getpid() << std::endl;
        startTime_ = std::chrono::high_resolution_clock::now();
        // Add real process work code here
        while (continue_)
        {
            /* code */
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate some work
        }
        auto lifetime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - startTime_).count();
        std::cout << "Real process is done: " << getpid() << " Lifetime: " << lifetime << " seconds." << std::endl;
        _exit(0); // Ensure the child process exits immediately
    }
} // namespace process
