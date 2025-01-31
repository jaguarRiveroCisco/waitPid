
#include "main_controller.h"
#include <mutex>
#include <thread>
#include "communicator.h"
#include "console_control.h"
#include "logger_instance.h"
#include "nap_time.h"
#include "network_process.h"
#include "process.h"
#include "process_simulator.h"
#include "random_stuff.h"
#include "string_tools.h"
#include "system_monitor.h"
#include "system_process.h"

namespace process
{
    std::mutex MainController::handlersMutex_;
    // Define the handler factory map
    std::unordered_map<std::string, MainController::HandlerFactory> MainController::handlerFactoryMap_;
    // Initialize the factory map
    void MainController::initializeFactory()
    {
        handlerFactoryMap_["real"]    = []() { return createHandler<ProcessMonitor, Process>(); };
        handlerFactoryMap_["simul"]   = []() { return createHandler<ProcessMonitor, ProcessSimulator>(); };
        handlerFactoryMap_["network"] = []() { return createHandler<ProcessMonitor, NetworkProcess>(); };
        handlerFactoryMap_["system"]  = []() { return createHandler<SystemMonitor, SystemProcess>(); };
    }

    void MainController::createHandler()
    {
        const std::string &processType = ProcessController::processType();
        auto it          = handlerFactoryMap_.find(processType);
        if (it != handlerFactoryMap_.end())
        {
            // second() is the templatized function that creates the relevant handler. 
            // See the header for the templatized function
            std::unique_ptr<ProcessMonitor> handler = it->second(); 
            ProcessController::handlers().push_back(std::move(handler));
        }
        else
        {
            throw std::runtime_error("Invalid process type");
        }
    }

    void MainController::createHandlers(int numHandlers)
    {
        for (int i = 0; i < numHandlers; ++i)
        {
            try
            {
                createHandler();
            }
            catch (const std::exception &e)
            {

                tools::LoggerManager::getInstance() << "Error creating process handler: " << e.what();
                tools::LoggerManager::getInstance().flush(tools::LogLevel::ERROR);
            }
        }
    }

    void MainController::run(const std::string &processType, int numProcesses)
    {
        ProcessController::setProcessType(processType);
        ProcessController::setNumProcesses(numProcesses);
        initializeFactory();
        createHandlers(ProcessController::numProcesses());
        std::thread monitoringThread(&MainController::CreateMonitoringThreads);
        monitoringThread.detach();
        std::thread terminationThread(&MainController::MonitorProcessTermination);
        terminationThread.detach();
        cli::driver::consoleLoop();
        processLifecycleLoop();
        cli::driver::consoleLoop(false);
    }
    
    void MainController::processLifecycleLoop()
    {
        while (ProcessController::running())
        {
            if (concurrency::Synchro::getInstance().pauseMonitoring())
            {
                // Sleep for a random short duration to simulate pause
                tools::sleepRandomMs();
                continue; // Check again if monitoring has been resumed
            }

            std::lock_guard<std::mutex> lock(handlersMutex_);
            restoreHandlerCount();

            if (ProcessController::handlers().empty())
            {
                ProcessController::running() = false;
                tools::LoggerManager::getInstance() << "[PARENT PROCESS] | All handlers removed, exiting...";
                tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
            }
        }
    }

    void MainController::restoreHandlerCount()
    {
        // Check if the number of handlers is less than numProcesses_
        if (ProcessController::running() && ProcessController::respawn() &&
            (ProcessController::handlers().size() < ProcessController::numProcesses()))
        {
            int numHandlersToCreate = ProcessController::numProcesses() - ProcessController::handlers().size();
            createHandlers(numHandlersToCreate);
        }
    }

} // namespace process
