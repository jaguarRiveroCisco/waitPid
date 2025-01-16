#include <iostream>
#include <thread>
#include "process_handler.h"
#include "console_control.h"
#include "semaphore_guard.h"

void displayCompilationInfo(const char *appName)
{
    std::cout << "*******************************************" << std::endl;
    std::cout << "*                                         *" << std::endl;
    std::cout << "*  Application Name: " << appName << std::endl;
    std::cout << "*  Compiled on: " << __DATE__ << " at " << __TIME__ << std::endl;
    std::cout << "*                                         *" << std::endl;
    std::cout << "*******************************************" << std::endl;
}


// Declare a static instance to trigger the display


auto main(int argc, char *argv[]) -> int
{

    displayCompilationInfo(argv[0]);

    int         numProcesses = 4;
    std::string processType  = "simul";
    int         rndUpper     = 10; // Default value for rndUpper

    cli::driver::parseArguments(argc, argv, numProcesses, processType, rndUpper);

    tools::LoggerManager::createLoggerType();

    cli::driver::printHelp(); // Call to printHelp


    std::thread readerThread(cli::driver::main);

    auto& sem = tools::SemaphoreGuard::getInstance();
    process::Controller::run(processType, numProcesses);

    readerThread.join(); // Ensure the reader thread is joined before exiting
    sem.unlinkSemaphore(sem.getName());

    cli::driver::printpid("[INFO] Main process exiting", "");

    // cli::driver::LoggerExample(); // Call to example

    return 0;
}