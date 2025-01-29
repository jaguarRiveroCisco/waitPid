#include "messenger.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <thread>
namespace concurrency
{
    Messenger::Messenger()
    {
        // Generate a unique key for the message queue
        auto key = ftok("progfile", 65);
        // Create or open a message queue
        msgid_ = msgget(key, 0666 | IPC_CREAT);
        if (msgid_ == -1)
        {
            perror("msgget");
            throw std::runtime_error("Failed to create or open message queue");
        }

        // Determine whether a new queue was created or an existing one was opened
        struct msqid_ds buf;
        if (msgctl(msgid_, IPC_STAT, &buf) == -1)
        {
            perror("msgctl");
            throw std::runtime_error("Failed to retrieve message queue status");
        }
        /*
              // Log some attributes of the message queue
        tools::LogOpt::getInstance().logInfo("Message queue created with ID " + std::to_string(msgid_));
        tools::LogOpt::getInstance().logInfo("Number of messages in queue: " + std::to_string(buf.msg_qnum));
        tools::LogOpt::getInstance().logInfo("Last message sent time: " + std::to_string(buf.msg_stime));
        tools::LogOpt::getInstance().logInfo("Last message received time: " + std::to_string(buf.msg_rtime));
        tools::LogOpt::getInstance().logInfo("Last change time: " + std::to_string(buf.msg_ctime));
        */
        // Initialize mutex and condition variable
        pthread_mutex_init(&mutex, nullptr);
    }

    Messenger::~Messenger()
    {
        // Destroy the message queue
        if (msgctl(msgid_, IPC_RMID, nullptr) == -1)
        {
            perror("msgctl");
        }
        // Destroy mutex and condition variable
        pthread_mutex_destroy(&mutex);
    }

    void Messenger::sendMessage(const std::string &text, int msgType)
    {
        pthread_mutex_lock(&mutex);
        Message message;
        message.msgType = msgType;
        std::snprintf(message.msgText, sizeof(message.msgText), "%s", text.c_str());

        if (msgsnd(msgid_, &message, sizeof(message.msgText), 0) == -1)
        {
            perror("msgsnd");
            pthread_mutex_unlock(&mutex);
            throw std::runtime_error("Failed to send message");
        }
        pthread_mutex_unlock(&mutex);
    }

    std::string Messenger::receiveMessage(int msgType)
    {
        Message message;
        auto start = std::chrono::steady_clock::now();
        auto timeout = std::chrono::seconds(5); // Set a timeout duration
        
        while (true)
        {
            pthread_mutex_lock(&mutex);
            if (msgrcv(msgid_, &message, sizeof(message.msgText), msgType, IPC_NOWAIT) != -1)
            {
                pthread_mutex_unlock(&mutex);
                return std::string(message.msgText);
            }
            pthread_mutex_unlock(&mutex);
        
            if (errno != ENOMSG)
            {
                perror("msgrcv");
                throw std::runtime_error("Failed to receive message");
            }
        
            // Check for timeout
            auto now = std::chrono::steady_clock::now();
            if (now - start > timeout)
            {
                throw std::runtime_error("Timeout while waiting for message");
            }
        
            // Sleep for a short duration before retrying
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
} // namespace concurrency
