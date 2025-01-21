#ifndef PROCESS_BASE_H
#define PROCESS_BASE_H
#include "synchro.h"
namespace process
{
    class BaseHandler 
    {
    public:
        pid_t getPid() const;
        const std::atomic<bool>& monitoring() const { return monitoring_; }
        void                     stopMonitoring();
        void                     createMonitorProcessThread();

    protected:
        BaseHandler() = default;
        virtual ~BaseHandler() = default;
        void          displayProcessStatus(int &status, const std::string&);
        bool          isProcessRunning();
        void          terminateProcess();
        void          killProcess();
        void          intProcess();
        void          sendSignal(int signal);
        void          monitorProcessThread();
        pid_t         pid_{0};
        std::atomic<bool> monitoring_ {false};
        std::atomic<bool> threadRunning_ {false};
    private:
        enum Nap
        {
            TIME = 100
        };
    };
} // namespace process

#endif // PROCESS_BASE_H
