#include "clock.h"

#include "interface.h"

void Clock::setRunning(bool running)
{
    if (running_ == false && running == true)
    {
        // Start the timer when the clock is set to running
        timer_ = SDL_AddTimerNS(0, Clock::stepCallback, nullptr);
    }
    else if (running == false)
    {
        // Stop the timer when the clock is set to not running
        SDL_RemoveTimer(timer_);
    }

    running_ = running;
}

Uint64 Clock::stepCallback(void* userdata, SDL_TimerID id, Uint64 interval)
{
    if (!Clock::getInstance().isRunning())
        return 0;

    //TODO: add drift compensation
    Uint64 next_delay_NS = 1e9 * 60.0 / Clock::getInstance().getBPM() / TICKS_PER_BAR;
    step_counter_ = ++step_counter_ % TICKS_PER_BAR;
    Interface::getInstance().getGrid().stepSequence();

    return next_delay_NS == 0 ? 1 : next_delay_NS;
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
