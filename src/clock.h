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

    static unsigned int getCounter() { return counter_; };

    float getBPM() { return bpm_; };
    void setBPM(float bpm) { bpm_ = bpm; };

    bool isRunning() { return running_; };
    void setRunning(bool running) { running_ = running; };

private:
    Clock() { last_time_ = SDL_GetPerformanceCounter(); }
    Clock(const Clock &) = delete;
    Clock &operator=(const Clock &) = delete;

    bool running_ = true;

    Uint64 last_time_;
    double delta_;
    float bpm_ = DEFAULT_BPM;

    static unsigned int counter_;
};