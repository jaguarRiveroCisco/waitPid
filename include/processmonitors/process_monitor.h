#pragma once

#ifndef PROCESS_MONITOR_H
#define PROCESS_MONITOR_H

#include "base_process.h"
#include "synchro.h"

namespace process
{
    class ProcessMonitor 
    {
    public:
        ProcessMonitor()          = default;
        virtual ~ProcessMonitor();
        std::atomic<bool>   &monitoring();
        [[nodiscard]] pid_t getPid() const;
        void                createMonitorProcessThread();
        void                collectAndLaunch(std::unique_ptr<IProcess> process);
        virtual void        launchChildProcess();
        void                terminateProcess() const;
        void                killProcess() const;
        void                intProcess() const;
        void                monitorProcessThread();

    protected:

        pid_t                     pid_{0};
        std::unique_ptr<IProcess> process_;

    private:
        std::atomic<bool> monitor_{false};
#ifdef __linux__
        void* stack_ = nullptr;
        void* args_ = nullptr;
#endif
    };

} // namespace process

#endif // PROCESS_MONITOR_H
