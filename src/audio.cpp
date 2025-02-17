#include "audio.h"
#include "interface.h"

void LowPassFilter::init(int sample_rate)
{
	fs_ = sample_rate;
	fc_ = sample_rate / 8.0f;
	calculateCoefficient();
}

void LowPassFilter::calculateCoefficient()
{
	alpha_ = (2.0f * fc_) / (2.0f * fc_ + fs_);
}

float LowPassFilter::process(float sample)
{
	float out = alpha_ * sample + (1.0f - alpha_) * prev_;
	prev_ = out;
	return out;
}

void audioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{
	AudioEngine *audio = (AudioEngine *)userdata;

	while (additional_amount > 0)
	{
		float samples[BUFFER_SIZE] = {};
		float new_samples[BUFFER_SIZE] = {};
		const int n_samples = SDL_min(additional_amount, BUFFER_SIZE);

		Grid &grid = Interface::getInstance().getGrid();

		for (auto block : grid.getGlobalBlocks())
		{
			if (block->getType() != BLOCK_GENERATOR)
				continue;

			BlockGenerator *gen_block = (BlockGenerator *)block;
			SDL_GetAudioStreamData(gen_block->getStream(), new_samples, n_samples * sizeof(float));

			for (int i = 0; i < n_samples; i++)
				samples[i] += new_samples[i] * audio->getAmp();
		}

		for (auto area : grid.getAreas())
			for (auto block : area->getBlocks())
			{
				if (block->getType() != BLOCK_GENERATOR)
					continue;

				BlockGenerator *gen_block = (BlockGenerator *)block;
				SDL_GetAudioStreamData(gen_block->getStream(), new_samples, n_samples * sizeof(float));

				for (int i = 0; i < n_samples; i++)
					samples[i] += new_samples[i] * audio->getAmp() * area->getAmp();
			}

		for (int i = 0; i < n_samples; i++)
			samples[i] = audio->lowPass(samples[i]);

		audio->setOutput(samples, n_samples);
		SDL_PutAudioStreamData(stream, samples, n_samples * sizeof(float));
		additional_amount -= n_samples;
	}
}

void AudioEngine::setOutput(const float *output, const int n_samples)
{
	// static int written;
	// int remainder = BUFFER_SIZE - n_samples;
	// SDL_memcpy(output_ + written, output_ + written - remainder, remainder * sizeof(float));
	SDL_memset(output_, (int)0.0f, BUFFER_SIZE * sizeof(float));
	SDL_memcpy(output_, output, n_samples * sizeof(float));
	// written = n_samples;
}

AudioEngine::AudioEngine()
{
	if (!SDL_Init(SDL_INIT_AUDIO))
	{
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		exit(SDL_APP_FAILURE);
	}

	SDL_GetAudioDeviceFormat(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec_, nullptr);
	spec_.channels = 1;
	spec_.format = SDL_AUDIO_F32;
	spec_.freq = spec_.freq * 2;
	printf("SAMPLE RATE: %d\n", spec_.freq);
	filter_.init(spec_.freq);

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