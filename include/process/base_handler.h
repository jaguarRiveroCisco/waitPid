#ifndef PROCESS_BASE_H
#define PROCESS_BASE_H
#include "synchro.h"
namespace process
{
    class BaseHandler 
    {
    public:
        pid_t getPid() const;
        std::atomic<bool> &running() { return running_; }

    protected:
        BaseHandler() = default;
        virtual ~BaseHandler() = default;
        void          displayProcessStatus(int &status);
        bool          isProcessRunning();
        void          terminateProcess();
        void          killProcess();
        void          intProcess();
        void          sendSignal(int signal);
        void          initializeProcessMonitoringThread();
        void          monitorProcessStatus();
        pid_t         pid_{0};
        Synchro      *synchro_{nullptr};
        std::atomic<bool> running_{true};
    };
} // namespace process

#endif // PROCESS_BASE_H
