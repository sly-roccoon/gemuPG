#include "sample.h"
#include "audio.h"
#include <semaphore>

Sample::~Sample()
{
    SDL_free(audio_);
}

void callback(void *userdata, const char *const *filelist, int filter);

bool Sample::open()
{
    SDL_ShowOpenFileDialog(callback, this, nullptr, &wav_filter, 1, NULL, false);

    SDL_WaitSemaphore(sem);
    if (empty())
        return false;

    updateWave();
    return true;
}

void Sample::updateWave()
{
    Uint8 *data;
    int size;
    SDL_ConvertAudioSamples(&spec_, audio_, audio_len_, AudioEngine::getInstance().getSpec(), &data, &size);

    sample_.reserve(size / spec_.channels);
    for (int i = 0; i < size; i += spec_.channels)
    {
        sample_.push_back((static_cast<float>(data[i]) / 255.0f) * 2.0f - 1.0f);
    }
}

std::array<float, WAVE_SIZE> Sample::getDispWave()
{
    std::array<float, WAVE_SIZE> out = {};
    if (empty())
        return out;

    Uint8 *data;
    int size;
    SDL_ConvertAudioSamples(&spec_, audio_, audio_len_, AudioEngine::getInstance().getSpec(), &data, &size);

    for (int i = 0; i < WAVE_SIZE; i++)
    {
        int idx = floor(i * size / WAVE_SIZE);
        idx -= idx % spec_.channels;

        out.at(i) = (static_cast<float>(data[idx]) / 255.0f) * 2.0f - 1.0f;
    }

    return out;
}

void callback(void *userdata, const char *const *filelist, int filter)
{
    Sample *sample = (Sample *)userdata;
    if (filelist == NULL || !filelist[0])
    {
        SDL_SignalSemaphore(sample->sem);
        return;
    }

    sample->setPath(filelist[0]);
    SDL_AudioSpec spec;
    Uint8 *audio;
    Uint32 audio_len;

    SDL_LoadWAV(sample->getPath().c_str(), &spec, &audio, &audio_len);

    sample->setSpec(spec);
    sample->setAudio(audio);
    sample->setAudioLen(audio_len);

    SDL_SignalSemaphore(sample->sem);
}