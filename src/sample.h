#pragma once

#include <SDL3/SDL.h>
#include <vector>
#include <array>
#include "util.h"

typedef enum
{
    ONE_SHOT,
    REPEAT
} sample_play_type_t;

class Sample
{
public:
    ~Sample();
    Sample();

    bool empty() { return audio_ == nullptr; };
    pitch_t getRoot() { return root_; };
    void setRoot(pitch_t root) { root_ = root; };
    bool open();

    void setPath(std::string path) { path_ = path; }
    std::string getPath() { return path_; }

    SDL_AudioSpec getSpec() { return spec_; }
    void setSpec(const SDL_AudioSpec &spec) { spec_ = spec; }

    Uint32 getAudioLen() { return audio_len_; }
    void setAudioLen(Uint32 len) { audio_len_ = len; }

    Uint8 *getAudio() { return audio_; }
    void setAudio(Uint8 *audio) { audio_ = audio; }

    void updateWave();
    std::array<float, WAVE_SIZE> *getDispWave() { return &disp_wave_; };
    float *getWave() { return sample_; };

    size_t getSize() { return sample_size_; }

    SDL_Semaphore *sem = SDL_CreateSemaphore(0);

private:
    std::string path_ = "";
    SDL_AudioSpec spec_ = {};
    Uint8 *audio_ = nullptr;
    Uint32 audio_len_ = -1;

    float fs_ratio_ = 1.0f;

    float *sample_ = nullptr;
    size_t sample_size_ = 0;

    std::array<float, WAVE_SIZE> disp_wave_ = {};

    pitch_t root_ = 440.0f;
    sample_play_type_t play_type_ = ONE_SHOT;

    void convertAudio(const SDL_AudioSpec *src_spec, Uint8 *data, Uint32 audio_len);

    static const inline SDL_DialogFileFilter wav_filter = {"WAV (*.wav)", "wav"};
};
;