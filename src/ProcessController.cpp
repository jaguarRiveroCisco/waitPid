#include <iostream>
#include <thread>
#include "console_control.h"
#include "process_handler.h"

void displayCompilationInfo(const char *appName)
{
    std::cout << "*******************************************" << std::endl;
    std::cout << "*                                         *" << std::endl;
    std::cout << "*  Application Name: " << appName << std::endl;
    std::cout << "*  Compiled on: " << __DATE__ << " at " << __TIME__ << std::endl;
    std::cout << "*                                         *" << std::endl;
    std::cout << "*******************************************" << std::endl;
}

auto main(int argc, char *argv[]) -> int
{

    displayCompilationInfo(argv[0]);

    int         numProcesses = 4;
    std::string processType  = "simul";

    cli::driver::parseArguments(argc, argv, numProcesses, processType);

    tools::LoggerManager::createLoggerType();

    std::thread readerThread(cli::driver::main);

    process::Controller::run(processType, numProcesses);

    readerThread.join(); // Ensure the reader thread is joined before exiting

    cli::driver::printpid("[INFO] Main process exiting", "");

    return 0;
}
