//
// Created by Jaguar Rivero on 2025-02-05.
//

#ifndef API_H
#define API_H

#include <iostream>
#include <string>
#include "nap_time.h"
#include "main_controller.h"

namespace api
{
    void initialize(const int &numProcesses, const std::string &processType);
    void execute();
    void respawn(bool);
    void loggerType(const std::string &);
    void SleepType(tools::NapType);
    void stop();
    void terminateAll();
    void killAll();
    void killPid(pid_t pid);
    void terminatePid(pid_t pid);
    void intPid(pid_t pid);
    void displayAllPids();
    void readConfigFile(const std::string &configFilePath);
    template<typename MonitorType, typename ProcessType>
    void registerHandler(const std::string& key)
    {
        process::MainController::registerHandler<MonitorType, ProcessType>(key);
    }

}

#endif //API_H
