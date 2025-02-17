#include "clock.h"

unsigned int Clock::counter_ = 0;

bool Clock::isTime()
{
    if (!running_)
        return false;

    Uint64 now = SDL_GetPerformanceCounter();
    delta_ = static_cast<double>(now - last_time_) / static_cast<double>(SDL_GetPerformanceFrequency());
    if (delta_ >= 60.0f / bpm_ / MAX_SUBDIVISION)
    {
        last_time_ = now;
        counter_++;
        return true;
    }

    return false;
}