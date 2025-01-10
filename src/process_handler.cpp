
#include <unistd.h>
#include "process.h"

#include "simul_process.h"
#include "process_handler.h"
#include "process_helper.h"

extern std::atomic<bool> g_display;
namespace process
{
    void Controller::createHandlers()
    {
        createHandlers_(numProcesses_);
    }

    void Controller::createHandlers_(int numHandlers)
    {
        std::cout << "Creating " << numHandlers << " child processes of type " << processType_ << ".\n";

        for (int i = 0; i < numHandlers; ++i)
        {
            try
            {
                createHandler();
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error creating process handler: " << e.what() << std::endl;
            }
        }
    }

    void Controller::run(const std::string &processType, int numProcesses)
    {
        setProcessType(processType);
        setNumProcesses(numProcesses);
        createHandlers();
        waitForEvents();
    }

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

        std::string messageText = Communicator::getInstance().receiveCreationMessage();
        if (g_display)
            std::cout << messageText << std::endl;
        handler->start();
        handlers_.push_back(std::move(handler));
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
                pid_t pid = synchro()->getAndPopFront();
                if (pid != -1)
                {
                    // Find and remove the handler with the matching PID
                    auto it = std::remove_if(
                            handlers_.begin(), handlers_.end(),
                            [pid](const std::unique_ptr<ControllerBase> &handler) { return handler->getPid() == pid; });
                    handlers_.erase(it, handlers_.end());
                    if (process::Controller::running())
                    {
                        if(process::Controller::respawn())
                        {
                            //if(g_display)
                            //    std::cout << "\trespawn!  ";
                            createHandler();
                        }
                    }
                }
                // Check if the number of handlers is less than numProcesses_
                if (handlers_.size() < numProcesses_)
                {
                    if (process::Controller::respawn())
                    {
                        int numHandlersToCreate = numProcesses_ - handlers_.size();
                        createHandlers_(numHandlersToCreate);
                    }
                }
                if(handlers_.empty())
                {
                    process::Controller::running() = false;
                }
            }
        }
    }


    



} // namespace process
