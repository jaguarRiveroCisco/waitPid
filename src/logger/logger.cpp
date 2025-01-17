#include "logger.h"
#include <atomic>
#include <sstream>
#include <string>
#include "timestamp.h"
namespace tools
{

    void Logger::log(LogLevel level, const std::string &message)
    {
        std::string logEntry = TimeStamp::get() + logLevelToString(level) + ": " + message;
        outputLog(logEntry);
    }

    void Logger::flush(LogLevel level)
    {
        std::string                 message = currentMessageStream.str();
        if (!message.empty())
        {
            log(level, message);
            currentMessageStream.str(""); // Clear the stream
        }
    }
    void Logger::logInfo(const std::string &message)
    {
        log(LogLevel::INFO, message);
    }
    void Logger::logWarning(const std::string &message)
    {
        log(LogLevel::WARNING, message);
    }
    void Logger::logError(const std::string &message)
    {
        log(LogLevel::ERROR, message);
    }
    void Logger::logException(const std::string &message)
    {
        log(LogLevel::EXCEPTION, message);
    }
    std::string Logger::logLevelToString(LogLevel level) const
    {
        switch (level)
        {
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::WARNING:
                return "WARNING";
            case LogLevel::ERROR:
                return "ERROR";
            case LogLevel::EXCEPTION:
                return "EXCEPTION";
            default:
                return "UNKNOWN";
        }
    }
} // namespace tools