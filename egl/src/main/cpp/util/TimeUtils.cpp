#include "TimeUtils.h"
#include <chrono>

using namespace hiveVG;

double CTimeUtils::getCurrentTime()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration - seconds);
    
    return static_cast<double>(seconds.count()) + static_cast<double>(microseconds.count()) / 1'000'000.0;
}