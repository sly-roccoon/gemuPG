#pragma once
#include "raylib.h"
#include "interface.h"

class AudioEngine
{
public:
    static AudioEngine &getInstance()
    {
        static AudioEngine instance;
        return instance;
    }
    ~AudioEngine();

private:
    AudioStream stream_;

    AudioEngine();
    AudioEngine(const AudioEngine &) = delete;
    AudioEngine &operator=(const AudioEngine &) = delete;
};