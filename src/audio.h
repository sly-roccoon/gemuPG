#pragma once
#include <SDL3/SDL.h>
#include "interface.h"

class AudioEngine
{
public:
	static AudioEngine &getInstance()
	{
		static AudioEngine instance;
		return instance;
	}
	void destroy();

private:
	SDL_AudioStream *stream_;
	SDL_AudioSpec spec_;

	// static void getStreams(int n_samples, AudioEngine *audio);

	AudioEngine();
	AudioEngine(const AudioEngine &) = delete;
	AudioEngine &operator=(const AudioEngine &) = delete;
};