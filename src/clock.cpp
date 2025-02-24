#include "clock.h"

bool Clock::shouldStep()
{
    if (!running_)
        return false;

    size_t min_possible_step_ = PERFORMANCE_FREQUENCY * 60.0f / bpm_ / MAX_SUBDIVISION;
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 dt = now - last_step_;
    if (dt >= min_possible_step_)
    {
        step_overshoot_ = min_possible_step_ - dt;

        last_step_ += dt;
        step_counter_ = ++step_counter_ % MAX_SUBDIVISION;
        return true;
    }

    return false;
}

bool Clock::shouldDraw()
{
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 dt = now - last_draw_;

    if (dt >= PERFORMANCE_FREQUENCY / fps_)
    {
        if (step_overshoot_ > (1.0 + MAX_BPM_DEVIATION) * PERFORMANCE_FREQUENCY * 60.0f / bpm_)
            fps_ = SDL_clamp(--fps_, MIN_FPS, MAX_FPS);
        else
            fps_ = SDL_clamp(++fps_, MIN_FPS, MAX_FPS);

        last_draw_ += dt;
        return true;
    }

    return false;
}