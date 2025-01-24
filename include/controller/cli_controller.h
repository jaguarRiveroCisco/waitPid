#pragma once

#ifndef CONTROLLER_CLI_CONTROLLER_H
#define CONTROLLER_CLI_CONTROLLER_H
#include "thread_controller.h"
namespace cli::driver
{
    class CLIController : public tools::ThreadController 
    {
    protected:
        void runThread() override;

    private:
        bool validateInput(const std::string &input);
    };
} // namespace cli::driver
#endif // CONTROLLER_CLI_CONTROLLER_H
