#pragma once

#ifndef CONTROLLER_COMMAND_CONTROLLER_H
#define CONTROLLER_COMMAND_CONTROLLER_H

#include <atomic>
#include <functional>
#include <iostream>
#include <thread>

namespace cli::driver
{
    class ThreadController 
    {
    public:
        virtual ~ThreadController() = default;

        void run(std::function<void(const std::string &)> commandFunc);

        void stop();

    protected:
        virtual void runThread() = 0;

        std::atomic<bool>                        stopFlag{false};
        std::function<void(const std::string &)> commandFunc;

    private:
        std::thread readerThread;
    };
} // namespace cli::driver

#endif // CONTROLLER_COMMAND_CONTROLLER_H
