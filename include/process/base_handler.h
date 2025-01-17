#ifndef PROCESS_BASE_H
#define PROCESS_BASE_H
#include "synchro.h"
namespace process
{
    class BaseHandler 
    {
    public:
        pid_t getPid() const;

    protected:
        BaseHandler() = default;
        virtual ~BaseHandler() = default;
        void          displayProcessStatus(int &status);
        bool          isProcessRunning();
        void          terminateProcess();
        void          killProcess();
        void          intProcess();
        void          sendSignal(int signal);
        void          startProcessMonitoringThread();
        void          monitorProcessStatus();
        pid_t         pid_{0};
        Synchro      *synchro_{nullptr};
    };
} // namespace process

#endif // PROCESS_BASE_H
