#pragma once

#ifndef PROCESS_INTERFACE_H
#define PROCESS_INTERFACE_H

#include <sys/types.h>
namespace process
{
    class IProcess 
    {
    public:
        virtual ~IProcess() = default;
        virtual void work() = 0;
        [[nodiscard]] virtual pid_t getPid() const = 0;
        virtual void setPid(pid_t) = 0;

    protected:
        virtual void preWork(pid_t pid) = 0;
        virtual void postWork() = 0;
    };

} // namespace process

#endif // PROCESS_INTERFACE_H
