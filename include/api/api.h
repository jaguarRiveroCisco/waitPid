//
// Created by Jaguar Rivero on 2025-02-05.
//

#ifndef API_H
#define API_H

#include <iostream>
#include <string>
#include "nap_time.h"

namespace api
{
    void execute(const int &numProcesses, const std::string &processType);
    void respawn(bool);
    void loggerType(const std::string &);
    void SleepType(tools::NapType);
}

#endif //API_H
