
#pragma once
#include "util.h"
#include <SDL3/SDL.h>
#include <map>

constexpr int n_filters = 4;

class LowPassFilter
{
public:
    LowPassFilter() :
        fs_ (0),
        fc_ (0.0),
        d_ (0.0),
        b_ { 0.0, 0.0, 0.0 },
        a_ { 0.0, 0.0, 0.0 },
        prev_x_ { 0.0f, 0.0f },
        prev_y_ { 0.0f, 0.0f }
    {
    }
    void init (int sample_rate);
    void process (float* samples, int n_samples);

    void setParams (int sample_rate, double cutoff_hz, double Q = ONE_DIV_SQRT_TWO);

private:
    int fs_;
    double fc_;
    double d_;

    std::array<double, 3> b_;
    std::array<double, 3> a_;

    std::array<float, 2> prev_x_;
    std::array<float, 2> prev_y_;
    void calculateCoefficients();
};

class AudioEngine
{
public:
    static AudioEngine& getInstance()
    {
        static AudioEngine instance;
        return instance;
    }
    void destroy();

    void setVolume (float volume) { volume_ = volume; }
    float getAmp() { return volume_; }
    void lowPass (float* samples, int n_samples)
    {
        for (auto& filter : filters_)
            filter.process (samples, n_samples);
    }

    int getSampleRate() { return spec_.freq; }
    static void updateStepTick (float bpm);

    float* getOutput() { return output_; }
    void setOutput (const float*, const int n);

    SDL_AudioSpec* getSpec() { return &spec_; }
    static std::array<float, WAVE_SIZE>* getWaveTable (WAVE_FORMS wave, pitch_t freq);
    static void initWaveTables();
    static bool checkStepTick();

    static float getBPM() { return bpm_; }
    static void setBPM (float bpm);

    static bool isRunning() { return is_running_; }
    static void setRunning (bool running);

private:
    SDL_AudioStream* stream_;
    static SDL_Thread* clockThread;
    static SDL_AudioSpec spec_;

    inline static float bpm_ = DEFAULT_BPM;
    inline static bool is_running_ = true;

    static double n_samples_per_step_tick_;

    AudioEngine();
    AudioEngine (const AudioEngine&) = delete;
    AudioEngine& operator= (const AudioEngine&) = delete;

    float volume_ = 0.5f;
    std::array<LowPassFilter, n_filters> filters_ {};

    float output_[BUFFER_SIZE] = {};
    static std::map<std::pair<WAVE_FORMS, int>, std::array<float, WAVE_SIZE>>
        wavetables_; // waveforms_[{WAVE_FORM,n_harmonics}][idx]
};

void audioCallback (void* userdata,
                    SDL_AudioStream* stream,
                    int additional_amount,
                    int total_amount);
