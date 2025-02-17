#include "audio.h"
#include "interface.h"

void LowPassFilter::init(int sample_rate)
{
	fs_ = sample_rate;
	fc_ = 10'000.0;
	d_ = 1.0;

	calculateCoefficients();
}

void LowPassFilter::calculateCoefficients()
{
	double F = 1.0 / std::tan(std::numbers::pi * fc_ / fs_);

	b_.at(0) = 1.0f / (1 + 2 * d_ * F + F * F);
	b_.at(1) = 2 * b_.at(0);
	b_.at(2) = b_.at(0);

	a_.at(0) = 0;
	a_.at(1) = 2 * b_.at(0) * (1.0 - F * F);
	a_.at(2) = b_.at(0) * (1.0 - 2.0 * d_ * F + F * F);
}

void LowPassFilter::process(float *samples, int n_samples)
{
	for (int sample = 0; sample < n_samples; sample++)
	{
		prev_x_.at(2) = prev_x_.at(1);
		prev_x_.at(1) = prev_x_.at(0);
		prev_x_.at(0) = samples[sample];

		samples[sample] = 0.0f;
		for (int i = 0; i < b_.size(); i++)
		{
			samples[sample] += b_.at(i) * prev_x_.at(i) - a_.at(i) * prev_y_.at(i);
		}

		prev_y_.at(2) = prev_y_.at(1);
		prev_y_.at(1) = samples[sample];
	}
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

		audio->lowPass(samples, n_samples);

		audio->setOutput(samples, n_samples);
		SDL_PutAudioStreamData(stream, samples, n_samples * sizeof(float));
		additional_amount -= n_samples;
	}
}

void AudioEngine::setOutput(const float *output, const int n_samples)
{
	static int written = 0;
	int remainder = BUFFER_SIZE - n_samples;

	// Shift the previous entries
	SDL_memmove(output_, output_ + n_samples, remainder * sizeof(float));

	// Copy the new output
	SDL_memcpy(output_ + remainder, output, n_samples * sizeof(float));
	written = n_samples;
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
	printf("SAMPLE RATE: %d\n", spec_.freq);
	for (auto &filter : filters_)
		filter.init(spec_.freq);

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