#include "clock.h"

bool Clock::isTime()
{
    SDL_Time now;
    SDL_GetCurrentTime(&now);
    delta_ = SDL_NS_TO_SECONDS(now - last_time_);
    if (delta_ >= 60.0f / bpm_)
    {
        last_time_ = now;
        return true;
    }

    return false;
}