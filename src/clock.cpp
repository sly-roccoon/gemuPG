#include "clock.h"

unsigned int Clock::counter_ = 0;

bool Clock::isTime()
{
    if (!running_)
        return false;

    SDL_Time now;
    SDL_GetCurrentTime(&now);
    delta_ = SDL_NS_TO_MS(now - last_time_) / 1000.0f;
    if (delta_ >= 60.0f / bpm_ / MAX_SUBDIVISION)
    {
        last_time_ = now;
        counter_++;
        return true;
    }

    return false;
}