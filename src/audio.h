#pragma once
#include <SDL3/SDL.h>
#include <cmath>
#include <numbers>
#include "interface.h"

class LowPassFilter
{
public:
	LowPassFilter() {}
	void init(int sample_rate);
	float process(float sample);

private:
	int fs_;
	int fc_;
	double alpha_;
	float prev_ = {0.0f};
	void calculateCoefficient();
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
	float lowPass(float sample) { return filter_.process(sample); }

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
	LowPassFilter filter_;

	float output_[BUFFER_SIZE] = {};
};

void audioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount);