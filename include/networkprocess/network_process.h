#pragma once
// network_process.h
#pragma once
#include <curl/curl.h>
#include <iostream>
#include <stdexcept>
#include "process.h"

namespace process
{

    class NetworkProcess : public Process 
    {
    public:
        ~NetworkProcess() override = default;
        void work() override;
    };

} // namespace process
