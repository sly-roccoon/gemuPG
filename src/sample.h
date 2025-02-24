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
    void setRoot(pitch_t root) { root_ = root <= 0.0f ? 0.1f : root; };
    bool open();
    bool openPath(std::string path = "");
    void updatePath();

    void setPath(std::string path) { path_ = path; }
    std::string getPath() { return path_; }
    std::string getName() { return name_; }

    SDL_AudioSpec getSpec() { return spec_; }
    void setSpec(const SDL_AudioSpec &spec) { spec_ = spec; }
    double getFsRatio() { return fs_ratio_; }

    Uint32 getAudioLen() { return audio_len_; }
    void setAudioLen(Uint32 len) { audio_len_ = len; }

    Uint8 *getAudio() { return audio_; }
    void setAudio(Uint8 *audio) { audio_ = audio; }

    void updateWave();
    std::array<float, WAVE_SIZE> *getDispWave() { return &disp_wave_; };
    float *getWave() { return sample_; };
    void copyWave(float *, size_t);

    int getSize() { return sample_size_; }

    void setPlayed(bool played) { played_ = played; }
    bool isPlayed() { return played_; }

    bool getTrigger() { return trigger_; }
    void setTrigger(bool trigger) { trigger_ = trigger; }

    sample_play_type_t getPlayType() { return play_type_; };
    void setPlayType(sample_play_type_t play_type) { play_type_ = play_type; }

private:
    std::string path_ = "";
    std::string name_ = "";
    SDL_AudioSpec spec_ = {};
    Uint8 *audio_ = nullptr;
    Uint32 audio_len_ = -1;

    double fs_ratio_ = 1.0f;

    float *sample_ = nullptr;
    int sample_size_ = 0;

    bool played_ = false;
    bool trigger_ = true;

    std::array<float, WAVE_SIZE> disp_wave_ = {};

    pitch_t root_ = 440.0f;
    sample_play_type_t play_type_ = REPEAT;

    static const inline SDL_DialogFileFilter wav_filter = {"WAV (*.wav)", "wav"};

    void convertAudio();
};
;