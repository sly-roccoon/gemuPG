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

private:
    Clock() { SDL_GetCurrentTime(&last_time_); };
    Clock(const Clock &) = delete;
    Clock &operator=(const Clock &) = delete;

    SDL_Time last_time_;
    float delta_;
    float bpm_ = 120.0f;
};