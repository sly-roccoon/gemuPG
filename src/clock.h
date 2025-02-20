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

    bool shouldStep();
    bool shouldDraw();

    static unsigned int getStepCounter()
    {
        step_counter_ = step_counter_ % MAX_SUBDIVISION;
        return step_counter_;
    };

    float getBPM() { return bpm_; };
    void setBPM(float bpm) { bpm_ = bpm; };

    bool isRunning() { return running_; };
    void setRunning(bool running) { running_ = running; };

    size_t getFPS() { return fps_; }

private:
    Clock() {};
    Clock(const Clock &) = delete;
    Clock &operator=(const Clock &) = delete;

    bool running_ = true;

    Uint64 last_step_ = SDL_GetPerformanceCounter();
    Uint64 last_draw_ = SDL_GetPerformanceCounter();
    float bpm_ = DEFAULT_BPM;
    size_t fps_ = MAX_FPS;

    size_t step_overshoot_ = 0;
    size_t min_possible_step_ = PERFORMANCE_FREQUENCY * 60.0f / bpm_ / MAX_SUBDIVISION;

    inline static unsigned int step_counter_ = 0;
};