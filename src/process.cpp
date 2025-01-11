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
        // Add real process work code here
        while (continue_)
        {
            /* code */
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate some work
        }
        logLifetime();
        _exit(0); // Ensure the child process exits immediately
    }
} // namespace process
