#include "clock.h"
#include "audio.h"
#include "interface.h"

void Clock::setRunning (bool running)
{
    if (running_ == false && running == true)
    {
        // Start the timer when the clock is set to running
        // timer_ = SDL_AddTimerNS(0, Clock::stepCallback, nullptr);
        // SDL_SignalSemaphore(clock_sem);
    }
    else if (running == false)
    {
        // Stop the timer when the clock is set to not running
        // SDL_RemoveTimer(timer_);
        last_call_ = 0;
    }

    running_ = running;
}

void Clock::setBPM (float bpm)
{
    bpm_ = bpm;
    AudioEngine::getInstance().updateStepTick (bpm_);
};

Uint64 Clock::stepCallback (void* userdata, SDL_TimerID id, Uint64 interval)
{
    if (! Clock::getInstance().isRunning())
        return 0;

    double next_delay_NS_ideal =
        1000000000ULL * 60.0 / Clock::getInstance().getBPM() / TICKS_PER_BAR;
    Uint64 next_delay_NS_int = SDL_trunc (next_delay_NS_ideal);

    // fractional nanosaconds handling
    delay_remainder_NS_ += next_delay_NS_ideal - next_delay_NS_int;
    if (delay_remainder_NS_ >= 1.0)
    {
        delay_remainder_NS_ = 0.0;
        next_delay_NS_int += 1;
    }

    Uint64 timer_delay_comp_NS = 0;

    if (last_call_)
        timer_delay_comp_NS = (SDL_GetTicksNS() - last_call_) - next_delay_NS_int;

    step_counter_ = ++step_counter_ % TICKS_PER_BAR;

    Uint64 before_step = SDL_GetTicksNS();
    Interface::getInstance().getGrid().stepSequence();
    Uint64 step_time_NS = SDL_GetTicksNS() - before_step;

    next_delay_NS_int = next_delay_NS_int - timer_delay_comp_NS - step_time_NS;

    last_call_ = SDL_GetTicksNS();
    return next_delay_NS_int == 0 ? 1 : next_delay_NS_int;
}

int Clock::stepThread (void* userdata)
{
    while (true)
    {
        if (! Clock::getInstance().isRunning())
            SDL_WaitSemaphore (clock_sem);

        Interface::getInstance().getGrid().stepSequence();

        double next_delay_NS = 1e9 * 60.0 / Clock::getInstance().getBPM() / TICKS_PER_BAR;
        Uint64 next_delay_NS_int = SDL_trunc (next_delay_NS);

        delay_remainder_NS_ += next_delay_NS - next_delay_NS_int;
        if (delay_remainder_NS_ >= 1.0)
        {
            delay_remainder_NS_ = 0.0;
            next_delay_NS_int += 1;
        }

        step_counter_ = ++step_counter_ % TICKS_PER_BAR;

        next_delay_NS_int = 0 ? 1 : next_delay_NS_int;
        SDL_DelayNS (next_delay_NS_int);
    }
}

bool Clock::shouldDraw()
{
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 dt = now - last_draw_;

    if (dt >= PERFORMANCE_FREQUENCY / fps_)
    {
        if (step_overshoot_ > (1.0 + MAX_BPM_DEVIATION) * PERFORMANCE_FREQUENCY * 60.0f / bpm_)
            fps_ = SDL_clamp (--fps_, MIN_FPS, MAX_FPS);
        else
            fps_ = SDL_clamp (++fps_, MIN_FPS, MAX_FPS);

        last_draw_ += dt;
        return true;
    }

    return false;
}
