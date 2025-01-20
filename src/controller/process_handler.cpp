
#include "process_handler.h"
#include "communicator.h"
#include "network_process.h"
#include "process.h"
#include "simul_process.h"
#include <thread>

namespace process
{
    void Controller::createHandler()
    {
        auto handler = std::make_unique<ControllerBase>();
        if (processType_ == "real")
        {
            handler->init(std::make_unique<Process>());
        }
        else if (processType_ == "simul")
        {
            handler->init(std::make_unique<ProcessSimulator>());
        }
        else if (processType_ == "network")
        {
            handler->init(std::make_unique<NetworkProcess>());
        }
        else
        {
            throw std::runtime_error("Invalid process type");
        }

        Communicator::getInstance().receiveCreationMessage();
        handlers_.push_back(std::move(handler));
    }

    void Controller::createHandlers(int numHandlers)
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

        tools::LoggerManager::getInstance() << "Number of handlers: " << handlers_.size();
        tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
        }

    void Controller::startMonitoring()
    {
        for (auto &handler : handlers_)
        {
            handler->createMonitorProcessThread();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void Controller::run(const std::string &processType, int numProcesses)
    {
        setProcessType(processType);
        setNumProcesses(numProcesses);
        createHandlers(numProcesses_);
        startMonitoring();
        waitForEvents();
    }

    void Controller::removeHandler()
    {
        if (!handlers_.empty())
        {
            pid_t pid = concurrency::Synchro::getInstance().removeFrontPid();
            if (pid != -1)
            {
                // Find and remove the handler with the matching PID
                auto it = std::remove_if(
                        handlers_.begin(), handlers_.end(),
                        [pid](const std::unique_ptr<ControllerBase> &handler) { return handler->getPid() == pid; });
                if (it != handlers_.end())
                {
                    handlers_.erase(it, handlers_.end());
                }
            }
        }
    }

    void Controller::waitForEvents()
    {
        while (process::Controller::running())
        {
            concurrency::Synchro::getInstance().blockUntilPidAvailable();

            // Process all events
            while (!concurrency::Synchro::getInstance().isPidQueueEmpty())
            {
                removeHandler();
                restoreHandlerCount();
                if (handlers_.empty())
                {
                    process::Controller::running() = false;
                    tools::LoggerManager::getInstance() << "[PARENT PROCESS] All handlers removed, exiting...";
                    tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
                }
            }
        }
    }

    void Controller::restoreHandlerCount()
    {
        // Check if the number of handlers is less than numProcesses_
        if (process::Controller::respawn() && (handlers_.size() < numProcesses_))
        {
            int numHandlersToCreate = numProcesses_ - handlers_.size();
            createHandlers(numHandlersToCreate);
        }
    }

} // namespace process
