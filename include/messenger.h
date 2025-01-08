#ifndef MESSENGER_H
#define MESSENGER_H

#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstdio>
#include <stdexcept>
#include <string>
#include "message.h"

class Messenger 
{
public:
    Messenger();
    ~Messenger();
    void sendMessage(const std::string &text, int msgType);
    std::string receiveMessage(int msgType);    
    // Delete the new and delete operators to prevent dynamic allocation
    void *operator new(size_t)   = delete;
    void operator delete(void *) = delete;
private:
    int msgid_;
};

#endif // MESSENGER_H
