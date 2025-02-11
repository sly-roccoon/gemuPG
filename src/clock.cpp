#include "clock.h"

bool Clock::isTime()
{
    SDL_Time now;
    SDL_GetCurrentTime(&now);
    delta_ = SDL_NS_TO_MS(now - last_time_) / 1000.0f;
    if (delta_ >= 60.0f / bpm_)
    {
        last_time_ = now;
        return true;
    }

    return false;
}