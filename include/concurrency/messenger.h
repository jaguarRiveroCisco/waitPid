#pragma once

#ifndef MESSENGER_H
#define MESSENGER_H

#include <thread>
#include <string>

namespace concurrency
{
    class Messenger 
    {
    public:
        Messenger();
        ~Messenger();
        void        sendMessage(const std::string &text, int msgType);
        std::string receiveMessage(int msgType);
        // Delete the new and delete operators to prevent dynamic allocation
        void *operator new(size_t)   = delete;
        void operator delete(void *) = delete;
        // Delete copy constructor and assignment operator
        Messenger(Messenger const &)      = delete;
        void operator=(Messenger const &) = delete;

    private:
        int msgid_{};
        std::mutex mutex{};
    };
}

#endif // MESSENGER_H
