#include "audio.h"
#include "stdio.h"

static int total_samples_generated = 0;

float *getStreams(int n_samples, AudioEngine *audio);

void audioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{
	AudioEngine *audio = (AudioEngine *)userdata;

	while (additional_amount > 0)
	{
		float samples[BUFFER_SIZE] = {};
		float new_samples[BUFFER_SIZE];
		const int n_samples = SDL_min(additional_amount, BUFFER_SIZE);

		Grid &grid = Interface::getInstance().getGrid();

		auto blocks = grid.getBlocks();
		for (auto block : blocks)
		{
			if (block->getType() != BLOCK_GENERATOR)
				continue;

			BlockGenerator *gen_block = (BlockGenerator *)block;
			SDL_GetAudioStreamData(gen_block->getStream(), new_samples, n_samples * sizeof(float));

			for (int i = 0; i < n_samples; i++)
				samples[i] += new_samples[i];
		}

		SDL_PutAudioStreamData(stream, samples, n_samples * sizeof(float));
		additional_amount -= n_samples;
	}
}

AudioEngine::AudioEngine()
{
	if (!SDL_Init(SDL_INIT_AUDIO))
	{
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		exit(SDL_APP_FAILURE);
	}

	spec_ = DEFAULT_SPEC;
	stream_ = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec_, audioCallback, this);

	if (!stream_)
	{
		SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
		exit(SDL_APP_FAILURE);
	}

	SDL_ResumeAudioStreamDevice(stream_);
}

void AudioEngine::destroy()
{
	SDL_DestroyAudioStream(stream_);
	SDL_CloseAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
}