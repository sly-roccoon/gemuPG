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

static Uint64 last = 0;

Uint64 Clock::stepCallback(void *userdata, SDL_TimerID id, Uint64 interval)
{
    if (!Clock::getInstance().isRunning())
        return 0;

    Interface::getInstance().getGrid().stepSequence();

    double next_delay_NS = 1e9 * 60.0 / Clock::getInstance().getBPM() / TICKS_PER_BAR;
    Uint64 next_delay_NS_int = SDL_trunc(next_delay_NS);

    delay_remainder_NS_ += next_delay_NS - next_delay_NS_int;
    if (delay_remainder_NS_ >= 1.0)
    {
        delay_remainder_NS_ = 0.0;
        next_delay_NS_int += 1;
    }

    step_counter_ = ++step_counter_ % TICKS_PER_BAR;

    printf("end of clock callback \n");

    return next_delay_NS_int == 0 ? 1 : next_delay_NS_int;
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
