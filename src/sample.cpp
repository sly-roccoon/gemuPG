#include "sample.h"
#include "audio.h"
#include <vector>

void Sample::processOversampled (float* samples, int n_samples, int oversampling_factor)
{
    // fs' = fs * K, cutoff = ~0.45 * fs (Hz)
    const int fs = AudioEngine::getInstance().getSampleRate();
    const int fs_oversampled = fs * oversampling_factor;
    const double cutoff_hz = 0.45 * fs; // anything above fs/2 will alias after decimation

    for (auto& filter : filters_)
    {
        filter.setParams (fs_oversampled, cutoff_hz);
        filter.process (samples, n_samples);
    }
}

void Sample::convertAudio()
{
    if (sample_)
        delete[] sample_;

    int channels = spec_.channels / AudioEngine::getInstance().getSpec()->channels;
    fs_ratio_ = AudioEngine::getInstance().getSpec()->freq / static_cast<double> (spec_.freq);
    double max_value = 1.0;
    std::vector<float> buffer_mono;

    switch (spec_.format)
    {
        case SDL_AUDIO_S16:
        {
            int16_t* buffer = reinterpret_cast<int16_t*> (audio_);
            size_t buffer_size_ =
                audio_len_ / static_cast<uint32_t> (SDL_AUDIO_BYTESIZE (SDL_AUDIO_S16));
            max_value = AUDIO_S16_PEAK;

            sample_size_ = buffer_size_ * fs_ratio_ / channels;

            buffer_mono.reserve (buffer_size_ / channels);
            for (int i = 0; i < buffer_size_; i += channels)
                buffer_mono.push_back (buffer[i]);
            break;
        }
        case SDL_AUDIO_S32:
        {
            int32_t* buffer = reinterpret_cast<int32_t*> (audio_);
            size_t buffer_size_ =
                audio_len_ / static_cast<uint32_t> (SDL_AUDIO_BYTESIZE (SDL_AUDIO_S32));
            max_value = AUDIO_S32_PEAK;

            sample_size_ = buffer_size_ * fs_ratio_ / channels;

            buffer_mono.reserve (buffer_size_ / channels);
            for (int i = 0; i < buffer_size_; i += channels)
                buffer_mono.push_back (buffer[i]);
            break;
        }
        case SDL_AUDIO_F32:
        {
            float* buffer = reinterpret_cast<float*> (audio_);
            size_t buffer_size_ =
                audio_len_ / static_cast<uint32_t> (SDL_AUDIO_BYTESIZE (SDL_AUDIO_F32));
            max_value = AUDIO_F32_PEAK;

            sample_size_ = buffer_size_ * fs_ratio_ / channels;

            buffer_mono.reserve (buffer_size_ / channels);
            for (int i = 0; i < buffer_size_; i += channels)
                buffer_mono.push_back (buffer[i]);
            break;
        }
        default:
        {
            break;
        }
    }

    sample_ = new float[sample_size_];
    for (int i = 0; i < sample_size_; i++)
        sample_[i] =
            interpTable (buffer_mono.data(), buffer_mono.size(), i / fs_ratio_) / max_value;

    return;
}

void Sample::copyWave (float* src, size_t size)
{
    memcpy (sample_, src, sizeof (float) * size);
    sample_size_ = size;
}

Sample::Sample() : spec_ { *AudioEngine::getInstance().getSpec() }
{
    for (auto& filter : filters_)
        filter.init (AudioEngine::getInstance().getSampleRate());
}

Sample::~Sample()
{
    SDL_free (audio_);
    if (sample_)
        delete[] sample_;
}

void callback (void* userdata, const char* const* filelist, int filter);

void Sample::updatePath()
{
    std::string currentDir (SDL_GetCurrentDirectory());
    std::string filePath (path_);
    if (path_.find (currentDir) == 0)
    {
        filePath = filePath.substr (currentDir.length());
        if (! filePath.empty())
            if (filePath[0] == '/' || filePath[0] == '\\')
                filePath.erase (0, 1);

        name_ = filePath;
        path_ = filePath;
    }
    else
    {
        size_t pos = filePath.find_last_of ("/");
        if (pos != std::string::npos)
            name_ = filePath.substr (pos + 1);
        else
        {
            pos = filePath.find_last_of ("\\");
            if (pos != std::string::npos)
                name_ = filePath.substr (pos + 1);
            else
                name_ = filePath;
        }
    }
}

bool Sample::openPath (std::string path)
{
    if (path.empty())
        return false;

    if (SDL_LoadWAV (path.c_str(), &spec_, &audio_, &audio_len_))
    {
        path_ = path;
        updatePath();
        updateWave();
        return true;
    }
    return false;
}

void Sample::open()
{
#ifndef EMSCRIPTEN
    SDL_ShowOpenFileDialog (callback,
                            this,
                            nullptr,
                            &wav_filter,
                            1,
                            SDL_GetCurrentDirectory(),
                            false);
#endif
}

void Sample::updateWave()
{
    convertAudio();

    disp_wave_ = {};

    for (int i = 0; i < WAVE_SIZE; i++)
    {
        int idx = floor (static_cast<float> (i * sample_size_) / WAVE_SIZE);

        disp_wave_.at (i) = static_cast<float> (sample_[idx]);
    }
}

void callback (void* userdata, const char* const* filelist, int filter)
{
    Sample* sample = (Sample*) userdata;
    if (filelist == NULL || ! filelist[0])
        return;

    SDL_AudioSpec spec;
    Uint8* audio;
    Uint32 audio_len;

    if (SDL_LoadWAV (filelist[0], &spec, &audio, &audio_len))
    {
        sample->setSpec (spec);
        sample->setAudio (audio);
        sample->setAudioLen (audio_len);

        sample->setPath (filelist[0]);
        sample->updateWave();
        sample->updatePath();
        sample->setNewLoad();
    }
}

bool Sample::isNewLoad()
{
    if (! new_load)
        return false;

    new_load = false;
    return true;
}
