#pragma once
#include <memory>
#include <SDL3/SDL.h>
#include "util.h"
#include "camera.h"

typedef enum
{
	BLOCK_NONE,
	BLOCK_GENERATOR,
	BLOCK_SEQUENCER,
	BLOCK_SETTINGS,
	BLOCK_EFFECT,
	BLOCK_MODULATOR
} blockType;

class Block
{
public:
	Block(Vector2f);
	SDL_FRect *getFRect();
	Vector2f getPos() { return Vector2f(rect_.x, rect_.y); }
	void setPos(Vector2f pos)
	{
		pos = floorVec(pos);
		rect_.x = pos.x;
		rect_.y = pos.y;
	}
	blockType getType() { return type_; }
	virtual Block *clone() = 0;

	virtual void drawGUI() = 0;
	void toggleGUI() { viewGUI_ = !viewGUI_; }

protected:
	bool viewGUI_ = false;
	blockType type_;
	bool bypass_ = false;
	SDL_FRect rect_{0, 0, 1.0f, 1.0f};
	SDL_FRect render_rect_;
};

typedef enum
{
	WAVE_SAMPLE,
	WAVE_SAW,
	WAVE_SINE,
	WAVE_SQUARE,
	WAVE_TRIANGLE
} WAVE_FORMS;

typedef struct generator_data_t
{
	WAVE_FORMS waveform;
	float *wave;
	float amp;
	float pan;
	float freq;
	float phase;
} generator_data_t;

class BlockGenerator : public Block
{
public:
	BlockGenerator(Vector2f, float phase = 0.0f);
	~BlockGenerator();
	BlockGenerator *clone() override;

	SDL_AudioStream *getStream() { return stream_; }
	void drawGUI() override;

	generator_data_t getData() { return data_; }
	void setData(generator_data_t data) { data_ = data; }
	void incrPhase() { data_.phase++; }

	void setWave(WAVE_FORMS waveform, float *wave = nullptr);

private:
	SDL_AudioStream *stream_;
	generator_data_t data_ = {
		.waveform = WAVE_SINE,
		.wave = new float[WAVE_SIZE],
		.amp = 1.0f,
		.pan = 0.0f,
		.freq = 440.0f,
		.phase = 0.0f};

	static void audioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount);
};