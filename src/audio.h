
#pragma once
#include <SDL3/SDL.h>
#include <cmath>
#include <map>
#include <numbers>
#include "interface.h"
#include "util.h"

constexpr int n_filters = 4;

class LowPassFilter
{
public:
	LowPassFilter()
		: fs_(0),
		fc_(0.0),
		d_(0.0),
		b_{0.0, 0.0, 0.0},
		a_{0.0, 0.0, 0.0},
		prev_x_{0.0f, 0.0f, 0.0f},
		prev_y_{0.0f, 0.0f, 0.0f}
	{}
	void init(int sample_rate);
	void process(float *samples, int n_samples);

private:
	int fs_;
	double fc_;
	double d_;

	std::array<double, 3> b_;
	std::array<double, 3> a_;

	std::array<float, 3> prev_x_;
	std::array<float, 3> prev_y_;
	void calculateCoefficients();
};

class AudioEngine
{
public:
	static AudioEngine &getInstance()
	{
		static AudioEngine instance;
		return instance;
	}
	void destroy();

	void setVolume(float volume) { volume_ = volume; }
	float getAmp() { return volume_; }
	void lowPass(float *samples, int n_samples)
	{
		for (auto &filter : filters_)
			filter.process(samples, n_samples);
	}

	int getSampleRate() { return spec_.freq; }

	float *getOutput() { return output_; }
	void setOutput(const float *, const int n);

	SDL_AudioSpec *getSpec() { return &spec_; }
	static std::array<float, WAVE_SIZE> *getWaveTable(WAVE_FORMS wave, pitch_t freq);
	static void initWaveTables();

private:
	SDL_AudioStream *stream_;
	static SDL_Thread *clockThread;
	static SDL_AudioSpec spec_;

	AudioEngine();
	AudioEngine(const AudioEngine &) = delete;
	AudioEngine &operator=(const AudioEngine &) = delete;

	float volume_ = 0.5f;
	std::array<LowPassFilter, n_filters> filters_;

	float output_[BUFFER_SIZE] = {};
	static std::map<std::pair<WAVE_FORMS, int>, std::array<float, WAVE_SIZE>> wavetables_; // waveforms_[{WAVE_FORM,n_harmonics}][idx]
};

void audioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount);