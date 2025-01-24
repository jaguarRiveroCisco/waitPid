#include "console_control.h"
#include <atomic>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include "logger_instance.h"
#include "nap_time.h"
#include "process_handler.h"
#include "simul_process.h"
#include "console_logger.h"
#include "cli_controller.h"
#include "control_this.h"
namespace cli::driver
{
    void killPid(pid_t pid);
    void terminatePid(pid_t pid);
    void intPid(pid_t pid);
    void doCommand(const std::string &input);
    void printContext(int numProcesses = -1, const std::string &processType = "");
    void printCommands();
    static tools::ConsoleLogger cl;

    template<typename T> void printpid(const std::string &str, const T &x)
    {
        cl << str << " " << x;
        cl.flush(tools::LogLevel::INFO);
    }

    template<typename T> void printpidE(const std::string &str, const T &x)
    {

        cl << str << " " << x;
        cl.flush(tools::LogLevel::ERROR);
    }

    template<typename T> void printpidW(const std::string &str, const T &x)
    {
        cl << str << " " << x;
        cl.flush(tools::LogLevel::WARNING);
    }

    void parseArguments(int argc, char *argv[], int &numProcesses, std::string &processType)
    {
        int opt;
        while ((opt = getopt(argc, argv, "n:t:s:l:T:h")) != -1) // Removed colon after 'h'
        {
            switch (opt)
            {
                case 'n':
                    // Set the number of processes from the argument
                    numProcesses = std::atoi(optarg);
                    if (numProcesses <= 0)
                    {
                        printpidW("Number of processes must be greater than 0. Defaulting to ", 4);
                        numProcesses = 4;
                    }
                    break;
                case 't':
                    // Set the process type from the argument
                    processType = optarg;
                    if (processType != "real" && processType != "simul" && processType != "network")
                    {
                        printpidW("Invalid process type defaulting to ", "simul");
                        processType = "simul";
                    }
                    break;
                case 's':
                    process::ControllerBase::respawn(std::atoi(optarg) != 0);
                    break;
                case 'l':
                    // Set the logging type from the argument
                    if (std::string(optarg) == "file")
                    {
                        tools::LoggerManager::loggerType() = "file";
                    }
                    else
                    {
                        printpidW("Invalid logging type defaulting to ", "console");
                        tools::LoggerManager::loggerType() = "console";
                    }
                    break;
                case 'T':
                    // Set the nap time type from the argument
                    if (std::string(optarg) == "MS")
                    {
                        tools::SleepTime::type = tools::NapType::MS;
                    }
                    else if (std::string(optarg) == "SEC")
                    {
                        tools::SleepTime::type = tools::NapType::SEC;
                    }
                    else if (std::string(optarg) == "MIN")
                    {
                        tools::SleepTime::type = tools::NapType::MIN;
                    }
                    else
                    {
                        printpidW("Invalid nap time type defaulting to ", "MS");
                        tools::SleepTime::type = tools::NapType::MS;
                    }
                    break;
                case 'h':
                default:
                    // Display usage information and exit
                    printpid(argv[0], "Usage:\n"
                                      "-n <number of processes>\n"
                                      "-t <process type 'real' or 'simul' (default)>\n"
                                      "-s <respawn (0 or 1)>\n"
                                      "-l <logging type 'console' or 'file'>\n"
                                      "-T <nap time type 'MS', 'SEC', 'MIN'>\n"
                                      "-h -> help");
                    std::exit(EXIT_SUCCESS);
            }
        }
        printContext(numProcesses, processType);
        printCommands(); // Call to printHelp
    }

    void printContext(int numProcesses, const std::string &processType)
    {
        // Static variables to store the latest values
        static int         lastNumProcesses = 0;
        static std::string lastProcessType  = "unknown";

        // Update static variables only if new values are provided
        if (numProcesses != -1)
            lastNumProcesses = numProcesses;
        if (!processType.empty())
            lastProcessType = processType;

        // Print the stored context
        std::cout << "\n========================= Context =========================\n"
                  << " PID                 : " << getpid() << "\n"
                  << " Number of Processes : " << lastNumProcesses << "\n"
                  << " Process Type        : " << lastProcessType << "\n"
                  << " Respawn             : " << (process::ControllerBase::respawn() ? "Enabled" : "Disabled") << "\n"
                  << " Logging Type        : " << process::ControllerBase::loggingTypeToString() << "\n"
                  << " Nap Time Type       : " << tools::SleepTime::NapTypeToString() << "\n"
                  << "==========================================================\n\n"
                  << std::flush;
    }

    void printCommands()
    {
        std::cout
                << "\n==========================================================\n"
                << "Process Commands Help Menu (" << "PID: " << getpid() << ")\n"
                << "==========================================================\n"
                << "Available commands:\n"
                << "  context         - Display the current context\n"
                << "  quit            - Signals the program to gracefully quit at the next loop\n"
                << "  term all        - Terminate (SIGTERM) all processes and exit the program\n"
                << "  term <pid>      - Terminate (SIGTERM) a specific process by PID\n"
                << "  int all         - Interrupt (SIGINT) all processes and exit the program\n"
                << "  int <pid>       - Interrupt (SIGINT) a specific process by PID\n"
                << "  kill all        - Kill all (SIGKILL) processes and exit the program\n"
                << "  kill <pid>      - Kill a (SIGKILL) specific process by PID\n"
                << "  display pids    - Display all current PIDs\n"
                << "  respawn on      - Turn on respawn\n"
                << "  respawn off     - Turn off respawn\n"
                << "  monitor on      - Turn on monitoring: spawn monitoring threads\n"
                << "  monitor off     - Turn off monitoring: end monitoring threads\n"
                << "  help            - Display this help message\n\n"
                << "==========================================================\n\n"
                << std::flush;
    }

    void consoleLoop(bool run)
    {
        controlThis<cli::driver::CLIController>(run, doCommand);
    }

    std::vector<std::string> splitString(const std::string &input, char delimiter)
    {
        std::vector<std::string> tokens;
        std::istringstream       stream(input);
        std::string              token;

        while (std::getline(stream, token, delimiter))
        {
            tokens.push_back(token);
        }

        return tokens;
    }


    void doCommand(const std::string &input)
    {
        if (input.empty())
        {
            return;
        }

        if (input == "context")
        {
            printContext();
        }
        else if (input == "quit")
        {
            process::ControllerBase::running(false);
            printpid("[QUIT] Signalling the program to gracefully quit at the next loop.", "");
        }
        else if (input == "term all")
        {
            printpid("[TERMINATE ALL] Terminating all processes and exiting.", "");
            process::Controller::terminateAll();
        }
        else if (input.rfind("term ", 0) == 0)
        {
            auto vals = splitString(input, ' ');
            printpid("[TERMINATE ONE] Terminating process with PID:", vals[1]);
            terminatePid(std::stoi(vals[1]));
        }
        else if (input == "int all")
        {
            printpid("[INTERRUPT ALL] Interrupting all processes and exiting.", "");
            process::Controller::intAll();
        }
        else if (input.rfind("int ", 0) == 0)
        {
            auto vals = splitString(input, ' ');

            printpid("[INTERRUPT ONE] Interrupting process with PID:", vals[1]);
            intPid( std::stoi(vals[1]) );
        }
        else if (input == "kill all")
        {
            printpid("[KILL ALL] Killing all processes and exiting.", "");
            process::Controller::killAll();
        }
        else if (input == "monitor on")
        {
            process::ControllerBase::continueMonitoring();
            process::Controller::CreateMonitoringThreads();
        }
        else if (input == "monitor off")
        {
            process::ControllerBase::pauseMonitoring();
        }
        else if (input.rfind("kill ", 0) == 0)
        {
            auto vals = splitString(input, ' ');
            printpid("[KILL ONE] Killing process with PID:", vals[1]);
            killPid(std::stoi(vals[1]));
        }
        else if (input == "display pids")
        {
            process::Controller::displayAllPids();
        }
        else if (input == "help")
        {
            printCommands();
        }
        else if (input == "respawn on")
        {
            process::ControllerBase::respawn(true);
            printpid("[RESPAWN] Respawn feature is now", "ON");
        }
        else if (input == "respawn off")
        {
            process::ControllerBase::respawn(false);
            printpid("[RESPAWN] Respawn feature is now", "OFF");
        }
        else
        {
            printpidE("[UNK] Unknown command [" + input + " ] Type 'help' for a list of available commands.", "");
        }
    }

    void killPid(pid_t pid)
    {
        try
        {
            process::Controller::killProcessByPid(pid);
        }
        catch (const std::invalid_argument &)
        {
            printpidE("Invalid PID format.", "");
        }
        catch (const std::out_of_range &)
        {
            printpidE("PID out of range.", "");
        }
    }

    void terminatePid(pid_t pid)
    {
        try
        {
            process::Controller::terminateProcessByPid(pid);
        }
        catch (const std::invalid_argument &)
        {
            printpidE("Invalid PID format.", "");
        }
        catch (const std::out_of_range &)
        {
            printpidE("PID out of range.", "");
        }
    }

    void intPid(pid_t pid)
    {
        try
        {
            process::Controller::intProcessByPid(pid);
        }
        catch (const std::invalid_argument &)
        {
            printpidE("Invalid PID format.", "");
        }
        catch (const std::out_of_range &)
        {
            printpidE("PID out of range.", "");
        }
    }

    int LoggerExample()
    {
        //
        cl.log(tools::LogLevel::INFO, "This is an info message.");
        cl.log(tools::LogLevel::WARNING, "This is a warning message.");
        cl.log(tools::LogLevel::ERROR, "This is an error message.");
        cl.logInfo("This is an info message.");
        cl.logWarning("This is a warning message.");
        cl.logError("This is an error message.");
        cl << "Starting a new log entry with operator<< ";
        cl << "and adding more details.";
        cl.flush(tools::LogLevel::INFO);
        //
        return 0;
    }
} // namespace cli::driver
