#include "time_manager.h"
#include "nap_time.h"
#include "random_stuff.h"
#include <iomanip>
#include <sstream>

namespace tools
{
    void TimeManager::setSleepDuration()
    {
        int val = 0;
        switch (tools::SleepTime::type)
        {
            case tools::NapType::MIN:
                val              = tools::randomMin();
                sleepDurationMs_ = val * 60 * 1000;
                units_           = " minutes";
                break;
            case tools::NapType::SEC:
                val               = tools::randomSec();
                sleepDurationMs_  = val * 1000;
                units_            = " seconds";
                break;
            default:
                val               = tools::randomMs();
                sleepDurationMs_  = val;
                break;
        }

        sleepDurationStr_ = timeToStr(sleepDurationMs_);
        endTime_     = startTime_ + std::chrono::milliseconds(sleepDurationMs_);
        maxLifeTime_ = std::chrono::milliseconds(sleepDurationMs_ + tools::NapTimeMs::LONG);
    }

    std::chrono::milliseconds::rep TimeManager::getElapsedMilliseconds() const
    {
        currentTime_ = std::chrono::high_resolution_clock::now();
        auto duration    = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime_ - startTime_);
        return duration.count();
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> &TimeManager::currentTime() { return currentTime_; }

    int         TimeManager::sleepDurationMs() const { return sleepDurationMs_; }

    bool        TimeManager::endOfLife() const { return currentTime_ >= endTime_; }

    bool        TimeManager::maxLifeExceeded() const { return currentTime_ - startTime_ >= maxLifeTime_; }

    std::string TimeManager::getSleepDurationStr() const { return sleepDurationStr_; }

    std::string TimeManager::getFormattedElapsedTimeStr() const
    {
        auto elapsedMilliseconds = getElapsedMilliseconds();
        return timeToStr(elapsedMilliseconds);
    }

    std::string TimeManager::timeToStr(long long elapsedMilliseconds) const
    {
        long long minutes = elapsedMilliseconds / 60000;
        double    seconds = (elapsedMilliseconds % 60000) / 1000.0;

        std::ostringstream oss;
        oss << minutes << ":" << std::setw(2) << std::setfill('0')
            << std::fixed << std::setprecision(3) << seconds << units_;

        return oss.str();
    }
} // namespace tools
