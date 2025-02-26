#include "audio.h"
#include "interface.h"

void LowPassFilter::init(int sample_rate)
{
	fs_ = sample_rate;
	fc_ = 20'000.0;
	d_ = 1.0 / std::sqrt(2.0);

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

		for (int i = 0; i < 3; i++)
		{
			if (std::isnan(prev_x_.at(i)))
				prev_x_.at(i) = 0.0;
			if (std::isnan(prev_y_.at(i)))
				prev_y_.at(i) = 0.0;
		}
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

		for (int b = 0; b < grid.getGlobalBlocks().size(); b++) // cannot use ranged for loop because of possible vector reallocation
		{
			Block *block = grid.getGlobalBlocks().at(b);
			if (block->getType() != BLOCK_GENERATOR)
				continue;

			BlockGenerator *gen_block = (BlockGenerator *)block;
			SDL_GetAudioStreamData(gen_block->getStream(), new_samples, n_samples * sizeof(float));

			for (int i = 0; i < n_samples; i++)
			{
				if (std::isnan(new_samples[i]))
					new_samples[i] = 0.0;
				samples[i] += new_samples[i] * audio->getAmp();
			}
		}

		for (int a = 0; a < grid.getAreas().size(); a++)
		{
			Area *area = grid.getAreas().at(a);
			for (int b = 0; b < area->getBlocks().size(); b++)
			{
				Block *block = area->getBlocks().at(b);
				if (block->getType() != BLOCK_GENERATOR)
					continue;

				BlockGenerator *gen_block = (BlockGenerator *)block;
				SDL_GetAudioStreamData(gen_block->getStream(), new_samples, n_samples * sizeof(float));

				for (int i = 0; i < n_samples; i++)
				{
					if (std::isnan(new_samples[i]))
						new_samples[i] = 0.0;
					samples[i] += new_samples[i] * audio->getAmp() * area->getAmp();
				}
			}
		}

		audio->lowPass(samples, n_samples);

		audio->setOutput(samples, n_samples);
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

	initWaveTables();

	SDL_ResumeAudioStreamDevice(stream_);
}

void AudioEngine::destroy()
{
	SDL_DestroyAudioStream(stream_);
	SDL_CloseAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
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

void AudioEngine::initWaveTables()
{
	for (int harmonic = 0; harmonic <= MAX_HARMONIC; harmonic++)
	{
		wavetables_[{WAVE_SINE, harmonic}] = {0.0f};
		wavetables_[{WAVE_SAW, harmonic}] = {0.0f};
		wavetables_[{WAVE_TRIANGLE, harmonic}] = {0.0f};
		wavetables_[{WAVE_SQUARE, harmonic}] = {0.0f};
	}

	constexpr double PI = std::numbers::pi;
	constexpr double TWO_PI = 2.0 * PI;
	constexpr double INV_PI = 1.0 / PI;
	constexpr double INV_PI_SQ = 1.0 / (PI * PI);

	for (int i = 0; i < WAVE_SIZE; i++)
	{
		long double phase = TWO_PI * i / WAVE_SIZE;

		for (int harmonic = 1; harmonic <= MAX_HARMONIC; harmonic++)
		{
			// SINE
			wavetables_.at({WAVE_SINE, harmonic}).at(i) = sinf(phase);

			// SAW
			float prev = wavetables_.at({WAVE_SAW, harmonic - 1}).at(i);
			wavetables_.at({WAVE_SAW, harmonic}).at(i) = prev - (2.0 * INV_PI) * ((-1) + 2 * (harmonic % 2)) * sin(harmonic * phase) / harmonic;

			// TRIANGLE
			prev = wavetables_.at({WAVE_TRIANGLE, harmonic - 1}).at(i);
			wavetables_.at({WAVE_TRIANGLE, harmonic}).at(i) = prev + -8.0 * INV_PI_SQ * ((-1) + 2 * (harmonic % 2)) * sin((2 * harmonic - 1) * phase) / std::pow(2 * harmonic - 1, 2);
		}

		for (int harmonic = 1; harmonic <= MAX_HARMONIC; harmonic++)
		{
			float prev = wavetables_.at({WAVE_SQUARE, harmonic - 1}).at(i);
			wavetables_.at({WAVE_SQUARE, harmonic}).at(i) = prev + 4.0 * INV_PI * sin((2 * harmonic - 1) * phase) / (2 * harmonic - 1);
		}
	}
}

std::array<float, WAVE_SIZE> *AudioEngine::getWaveTable(WAVE_FORMS form, pitch_t freq)
{
	int n_harmonics = floor(spec_.freq / (2.0 * freq));
	n_harmonics = std::clamp(n_harmonics, 1, MAX_HARMONIC);

	return &wavetables_.at({form, n_harmonics});
}

SDL_AudioSpec AudioEngine::spec_ = {};
std::map<std::pair<WAVE_FORMS, int>, std::array<float, WAVE_SIZE>> AudioEngine::wavetables_ = {};