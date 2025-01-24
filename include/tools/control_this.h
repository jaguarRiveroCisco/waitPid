#pragma once

#ifndef TOOLS_CONTROL_THIS_H
#define TOOLS_CONTROL_THIS_H
#include <functional>
namespace cli::driver
{
    template<typename T> 
    void controlThis(bool run, std::function<void(const std::string &)> commandFunc)
    {
        static std::unique_ptr<T> cc{nullptr};
        if (run)
        {
            if (!cc)
            {
                cc = std::make_unique<T>();
                cc->run(commandFunc);
            }
        }
        else
        {
            if (cc)
            {
                cc->stop();
                cc.reset();
            }
        }
    }
}

#endif // TOOLS_CONTROL_THIS_H