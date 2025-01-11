#ifndef PROCESS_H
#define PROCESS_H

#include "process_interface.h"

namespace process
{
    class Process : public BaseProcess 
    {
    public:
        Process()          = default;
        virtual ~Process() = default;
        void work() override;
    };
} // namespace process

#endif // PROCESS_H