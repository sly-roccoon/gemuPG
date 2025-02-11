#pragma once

#include "util.h"
#include <SDL3/SDL_time.h>
#include <SDL3/SDL_timer.h>

class Clock
{
public:
    static Clock &getInstance()
    {
        static Clock clock;
        return clock;
    }

    float getDelta() { return delta_; };
    bool isTime();

    float getBPM() { return bpm_; };
    void setBPM(float bpm) { bpm_ = bpm; };

    bool isRunning() { return running_; };
    void setRunning(bool running) { running_ = running; };

private:
    Clock() { SDL_GetCurrentTime(&last_time_); };
    Clock(const Clock &) = delete;
    Clock &operator=(const Clock &) = delete;

    bool running_ = true;

    SDL_Time last_time_;
    float delta_;
    float bpm_ = DEFAULT_BPM;
};