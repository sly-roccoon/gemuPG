#pragma once
#include <SDL3/SDL.h>
#include <cmath>
#include <numbers>
#include "interface.h"

constexpr int n_filters = 4;

class LowPassFilter
{
public:
	LowPassFilter() {}
	void init(int sample_rate);
	void process(float *samples, int n_samples);

private:
	int fs_;
	double fc_;
	double d_;

	std::array<double, 3> b_;
	std::array<double, 3> a_;

	std::array<float, 3> prev_x_ = {0.0f, 0.0f};
	std::array<float, 3> prev_y_ = {0.0f, 0.0f};
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

	float *getOutput() { return output_; }
	void setOutput(const float *, const int n);

	SDL_AudioSpec *getSpec() { return &spec_; }

private:
	SDL_AudioStream *stream_;
	SDL_AudioSpec spec_;

	AudioEngine();
	AudioEngine(const AudioEngine &) = delete;
	AudioEngine &operator=(const AudioEngine &) = delete;
	float volume_ = 0.5f;
	std::array<LowPassFilter, n_filters> filters_;

	float output_[BUFFER_SIZE] = {};
};

void audioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount);