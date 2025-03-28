
#include "api.h"
#include "console_control.h"
#include "nap_time.h"
#include <thread>
#include "console_loop.h"

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
    static tools::ConsoleLogger cl;

    cli::driver::parseArguments(argc, argv, numProcesses, processType, cl);

    cli::driver::printContext(numProcesses, processType);

    cli::driver::printCommands(); // Call to printHelp

    api::initialize(numProcesses, processType);

    cli::driver::consoleLoop();

    api::execute();

    cli::driver::consoleLoop(false);

    std::this_thread::sleep_for(std::chrono::milliseconds(tools::NapTimeMs::LONG));

    cl.logInfo("[EXITING] Main process exiting");

    return 0;
}
