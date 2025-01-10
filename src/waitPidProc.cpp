#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include "process.h"
#include "process_handler.h"
#include "simul_process.h"
#include "process_control.h"

std::atomic<bool> g_display = true;

auto main(int argc, char *argv[]) -> int
{
    int         numProcesses = 4;
    std::string processType  = "simul";
    int         rndUpper     = 10; // Default value for rndUpper

    process::controller::parseArguments(argc, argv, numProcesses, processType, rndUpper);

    process::ProcessSimulator::setRndUpper(rndUpper); // Call to setRndUpper with the parsed value

    std::thread readerThread(process::controller::main);

    process::Controller::run(processType, numProcesses);

    readerThread.join(); // Ensure the reader thread is joined before exiting
    return 0;
}