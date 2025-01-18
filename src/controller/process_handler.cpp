
#include "process_handler.h"
#include "communicator.h"
#include "network_process.h"
#include "process.h"
#include "simul_process.h"

namespace process
{
    void Controller::createHandler()
    {
        auto handler = std::make_unique<ControllerBase>();
        if (processType_ == "real")
        {
            handler->init(synchro(), std::make_unique<Process>());
        }
        else if (processType_ == "simul")
        {
            handler->init(synchro(), std::make_unique<ProcessSimulator>());
        }
        else if (processType_ == "network")
        {
            handler->init(synchro(), std::make_unique<NetworkProcess>());
        }
        else
        {
            throw std::runtime_error("Invalid process type");
        }

        Communicator::getInstance().receiveCreationMessage();
        handler->start();
        handlers_.push_back(std::move(handler));
    }

    void Controller::createHandlers_(int numHandlers)
    {
        tools::LoggerManager::getInstance() << "Creating " << numHandlers << " process handlers.";
        tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
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
        tools::LoggerManager::getInstance() << "Created " << numHandlers << " process handlers.";
        tools::LoggerManager::getInstance().flush(tools::LogLevel::INFO);
    }

    void Controller::createHandlers() { createHandlers_(numProcesses_); }

    void Controller::run(const std::string &processType, int numProcesses)
    {
        setProcessType(processType);
        setNumProcesses(numProcesses);
        createHandlers();
        waitForEvents();
    }

    void Controller::removeHandler()
    {
        if (!handlers_.empty())
        {
            pid_t pid = synchro()->getAndPopFront();
            if (pid != -1)
            {
                // Find and remove the handler with the matching PID
                auto it = std::remove_if(
                        handlers_.begin(), handlers_.end(),
                        [pid](const std::unique_ptr<ControllerBase> &handler) { return handler->getPid() == pid; });
                handlers_.erase(it, handlers_.end());
            }
        }
    }

    void Controller::waitForEvents()
    {
        while (process::Controller::running())
        {
            std::unique_lock<std::mutex> lock(synchro()->mtx);
            synchro()->cv.wait(lock, [&] { return !synchro()->eventQueue.empty(); });

            // Process all events
            while (!synchro()->eventQueue.empty())
            {
                removeHandler();
                restoreHandlerCount();
                if (handlers_.empty())
                {
                    process::Controller::running() = false;
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
            createHandlers_(numHandlersToCreate);
        }
    }

} // namespace process
