#pragma once

#ifndef PROCESS_H
#define PROCESS_H

#include "base_process.h"

namespace process
{
    class Process : public BaseProcess 
    {
    public:
        virtual ~Process() = default;
        void work() override;
    };
} // namespace process

#endif // PROCESS_H