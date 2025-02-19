#include "sample.h"
#include "audio.h"
#include <semaphore>

void Sample::convertAudio(const SDL_AudioSpec *src_spec, Uint8 *data, Uint32 audio_len)
{
    if (sample_)
        delete[] sample_;

    size_t sample_size = 1;
    size_t channel_incr = src_spec->channels / AudioEngine::getInstance().getSpec()->channels;
    double freq_factor = static_cast<double>(src_spec->freq) / AudioEngine::getInstance().getSpec()->freq;
    double pos = 0;

    switch (src_spec->format)
    {
    case SDL_AUDIO_S16:
    {
        int16_t *buffer = reinterpret_cast<int16_t *>(data);
        sample_ = new float[audio_len / static_cast<uint32_t>(SDL_AUDIO_BYTESIZE(SDL_AUDIO_S16))];
        sample_size_ = audio_len / static_cast<uint32_t>(SDL_AUDIO_BYTESIZE(SDL_AUDIO_S16));
        for (size_t i = 0; i < sample_size_; i++)
        {
            pos += channel_incr * freq_factor;
            size_t idx = static_cast<size_t>(pos);
            idx -= idx % channel_incr;
            sample_[i] = static_cast<float>(buffer[idx]) / AUDIO_S16_PEAK;
        }
        return;
    }
    case SDL_AUDIO_S32: // FIXME: 24 bit audio is seen as 32 bit audio, causing glitches!
    {
        sample_size_ = audio_len / static_cast<uint32_t>(SDL_AUDIO_BYTESIZE(SDL_AUDIO_S32));

        int32_t *buffer = reinterpret_cast<int32_t *>(data);
        sample_ = new float[sample_size_];

        for (size_t i = 0; i < sample_size_; i++)
        {
            pos += channel_incr * freq_factor;
            size_t idx = static_cast<size_t>(pos);
            idx -= idx % channel_incr;
            sample_[i++] = static_cast<float>(buffer[idx]) / AUDIO_S32_PEAK;
        }
        return;
    }
    case SDL_AUDIO_F32:
    {
        sample_size_ = audio_len / static_cast<uint32_t>(SDL_AUDIO_BYTESIZE(SDL_AUDIO_F32));
        float_t *buffer = reinterpret_cast<float_t *>(data);
        sample_ = new float[sample_size_];

        for (size_t i = 0; i < sample_size_; i++)
        {
            pos += channel_incr * freq_factor;
            size_t idx = static_cast<size_t>(pos);
            idx -= idx % channel_incr;
            sample_[i++] = static_cast<float>(buffer[idx]);
        }
        return;
    }
    }

    return;
}

Sample::Sample() : spec_{*AudioEngine::getInstance().getSpec()} {}

Sample::~Sample()
{
    SDL_free(audio_);
}

void callback(void *userdata, const char *const *filelist, int filter);

void Sample::updatePath()
{
    std::string currentDir(SDL_GetCurrentDirectory());
    if (path_.find(currentDir) == 0)
    {
        std::string filePath(path_);
        filePath = filePath.substr(currentDir.length());
        if (!filePath.empty())
            if (filePath[0] == '/' || filePath[0] == '\\')
                filePath.erase(0, 1);

        path_ = filePath;
    }
}

bool Sample::openPath(std::string path)
{
    if (path.empty())
        return false;

    if (SDL_LoadWAV(path.c_str(), &spec_, &audio_, &audio_len_))
    {
        updatePath();
        updateWave();
        return true;
    }
    return false;
}

bool Sample::open()
{
    SDL_ShowOpenFileDialog(callback, this, nullptr, &wav_filter, 1, SDL_GetCurrentDirectory(), false);

    SDL_WaitSemaphore(sem);
    if (empty())
        return false;

    updatePath();
    updateWave();
    return true;
}

void Sample::updateWave()
{
    fs_ratio_ = AudioEngine::getInstance().getSpec()->freq / spec_.freq;
    convertAudio(&spec_, audio_, audio_len_);

    disp_wave_ = {};

    convertAudio(&spec_, audio_, audio_len_);

    for (int i = 0; i < WAVE_SIZE; i++)
    {
        int idx = floor(i * sample_size_ / WAVE_SIZE);

        disp_wave_.at(i) = static_cast<float>(sample_[idx]);
    }
}

void callback(void *userdata, const char *const *filelist, int filter)
{
    Sample *sample = (Sample *)userdata;
    if (filelist == NULL || !filelist[0])
    {
        SDL_SignalSemaphore(sample->sem);
        return;
    }

    SDL_AudioSpec spec;
    Uint8 *audio;
    Uint32 audio_len;

    SDL_LoadWAV(filelist[0], &spec, &audio, &audio_len);

    sample->setSpec(spec);
    sample->setAudio(audio);
    sample->setAudioLen(audio_len);

    sample->setPath(filelist[0]);

    SDL_SignalSemaphore(sample->sem);
}