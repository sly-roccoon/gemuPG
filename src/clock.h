#pragma once

#include "audio.h"
#include "util.h"
#include <SDL3/SDL_time.h>
#include <SDL3/SDL_timer.h>

class Clock {
public:
  static Clock &getInstance() {
    static Clock clock;
    return clock;
  }

  static Uint64 stepCallback(void *userdata, SDL_TimerID id, Uint64 interval);
  static int stepThread(void *);
  inline static SDL_Semaphore *clock_sem = SDL_CreateSemaphore(0);
  bool shouldDraw();

  static unsigned int getStepCounter() { return step_counter_; };

  float getBPM() { return bpm_; };
  void setBPM(float bpm);
  void setTimerID(SDL_TimerID id) { timer_ = id; };

  bool isRunning() { return running_; };
  void setRunning(bool running);

  size_t getFPS() { return fps_; }

private:
  Clock() { AudioEngine::getInstance().updateStepTick(bpm_); };
  Clock(const Clock &) = delete;
  Clock &operator=(const Clock &) = delete;

  bool running_ = true;
  SDL_TimerID timer_ = 0;

  Uint64 last_step_ = SDL_GetPerformanceCounter();
  Uint64 last_draw_ = SDL_GetPerformanceCounter();
  float bpm_ = DEFAULT_BPM;
  size_t fps_ = MAX_FPS;

  size_t step_overshoot_ = 0;

  inline static Uint64 step_counter_ = 0;
  inline static double delay_remainder_NS_ = 0.0;
  inline static Uint64 expected_delay_NS_ = 0;
  inline static Uint64 last_call_ = 0;
};
