#include "audio.h"
#include <cmath>
#include "stdio.h"

float idx = 0.0f;

void audioCallback(void *buffer, unsigned int frames)
{
    float delta = 440.0f / SAMPLE_RATE;
    short *d = (short *)buffer;

    for (int i = 0; i < frames; i++)
    {
        d[i] = 32000 * sinf(2 * PI * idx);
        idx += delta;
        if (idx >= 1.0f)
            idx -= 1.0f;
    }
}

AudioEngine::AudioEngine()
{
    InitAudioDevice();
    stream_ = LoadAudioStream(SAMPLE_RATE, BIT_DEPTH, CHANNELS);
    SetAudioStreamBufferSizeDefault(BUFFER_SIZE);
    SetAudioStreamCallback(stream_, audioCallback);
    PlayAudioStream(stream_);
}

AudioEngine::~AudioEngine()
{
    UnloadAudioStream(stream_);
    CloseAudioDevice();
}
