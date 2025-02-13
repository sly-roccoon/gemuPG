#include "clock.h"

unsigned int Clock::counter_ = 0;

bool Clock::isTime()
{
    if (!running_)
        return false;

    Uint64 now = SDL_GetTicksNS();
    delta_ = SDL_NS_TO_MS(now - last_time_) / 1000.0f;
    if (delta_ >= 60.0f / bpm_ / MAX_SUBDIVISION)
    {
        last_time_ = now;
        counter_++;
        return true;
    }

    return false;
}