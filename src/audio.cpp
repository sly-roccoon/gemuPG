//#include "audio.h"
//#include "stdio.h"
//
//
//static int total_samples_generated = 0;
//
//void audioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
//{ //TODO: replace
//	additional_amount /= sizeof(float);  /* convert from bytes to samples */
//	while (additional_amount > 0) {
//		float samples[128];  /* this will feed 128 samples each iteration until we have enough. */
//		const int total = SDL_min(additional_amount, SDL_arraysize(samples));
//		int i;
//
//		for (i = 0; i < total; i++) {
//			/* You don't have to care about this math; we're just generating a simple sine wave as we go.
//			   https://en.wikipedia.org/wiki/Sine_wave */
//			const float time = total_samples_generated / 8000.0f;
//			const int sine_freq = 500;   /* run the wave at 500Hz */
//			samples[i] = SDL_sinf(6.283185f * sine_freq * time);
//			total_samples_generated++;
//		}
//
//		/* feed the new data to the stream. It will queue at the end, and trickle out as the hardware needs more data. */
//		SDL_PutAudioStreamData(stream, samples, total * sizeof(float));
//		additional_amount -= total;  /* subtract what we've just fed the stream. */
//	}
//}
//
//AudioEngine::AudioEngine()
//{
//	if (!SDL_Init(SDL_INIT_AUDIO)) {
//		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
//		exit(SDL_APP_FAILURE);
//	}
//
//	spec_.channels = CHANNELS;
//	spec_.format = SDL_AUDIO_F32;
//	spec_.freq = SAMPLE_RATE;
//	stream_ = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec_, audioCallback, this);
//
//	if (!stream_) {
//		SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
//		exit(SDL_APP_FAILURE);
//	}
//	
//	SDL_ResumeAudioStreamDevice(stream_);
//}