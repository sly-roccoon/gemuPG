#pragma once
#include <memory>
#include <vector>
#include <SDL3/SDL.h>
#include "util.h"
#include "camera.h"

class Area;

class Block
{
public:
	Block(Vector2f);
	SDL_FRect *getFRect();
	SDL_FRect smallerFRect(SDL_FRect rect);
	Vector2f getPos() { return Vector2f(rect_.x, rect_.y); }
	void setPos(Vector2f pos)
	{
		pos = floorVec(pos);
		rect_.x = pos.x;
		rect_.y = pos.y;
	}
	block_type_t getType() { return type_; }
	virtual Block *clone() = 0; // TODO: implement properly with operator overloading

	virtual void drawGUI() = 0;
	void toggleGUI() { viewGUI_ = !viewGUI_; }
	void setGUI(bool view) { viewGUI_ = view; }

	bool getInArea() { return is_in_area_; }
	void setInArea(bool in_area) { is_in_area_ = in_area; }

	std::string getText(int i) { return text_.at(i); }
	void setText(std::string text, int i) { text_.at(i) = text; }

	SDL_Texture *getTextTexture(int i) { return text_texture_.at(i); }
	void setTextTexture(SDL_Texture *text_texture, int i) { text_texture_.at(i) = text_texture; }

	Vector2f getValueTextSize() { return value_text_size_; }
	void setValueTextSize(Vector2f size) { value_text_size_ = size; }

protected:
	bool viewGUI_ = false;
	block_type_t type_;
	bool is_in_area_ = false;
	SDL_FRect rect_{0, 0, 1.0f, 1.0f};
	SDL_FRect render_rect_;

	std::array<std::string, 2> text_ = {"", ""};
	std::array<SDL_Texture *, 2> text_texture_ = {nullptr, nullptr};
	Vector2f value_text_size_ = {0.0f, 0.0f};
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
	float crest;
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
	void setData(generator_data_t data)
	{
		float tmp_phase = data_.phase;
		data_ = data;
		data_.phase = tmp_phase;
	}

	void setFrequency(pitch_t freq);
	float getFrequency() { return data_.freq; }

	void incrPhase() { data_.phase++; }

	void setWave(WAVE_FORMS waveform, float *wave = nullptr);
	WAVE_FORMS getWave() { return data_.waveform; }

	float getAmp() { return data_.amp; }
	void setAmp(float amp) { data_.amp = amp; }

	pitch_t getRelFreq() { return rel_freq_; }
	void setRelFreq(pitch_t rel_freq) { rel_freq_ = rel_freq; }
	float getFreqFactor() { return freq_factor_; }
	void setFreqFactor(float freq_factor) { freq_factor_ = freq_factor; }

	void setBypass(bool bypass) { bypass_ = bypass; }
	bool getBypass() { return bypass_; }

private:
	bool bypass_ = false;
	SDL_AudioStream *stream_;
	generator_data_t data_ = {
		.waveform = WAVE_SINE,
		.wave = new float[WAVE_SIZE],
		.crest = 1.0f,
		.amp = 1.0f,
		.pan = 0.0f,
		.freq = 440.0f,
		.phase = 0.0f};

	pitch_t rel_freq_ = 0.0f;
	float freq_factor_ = 1.0f;

	static void audioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount);
};

class BlockSequencer : public Block
{
public:
	BlockSequencer(Vector2f);
	~BlockSequencer() = default;

	BlockSequencer *clone() override;
	void drawGUI() override;
	SDL_FRect *getFRect();

	void setActive(bool active) { is_active_ = active; }

	void setPitch(pitch_t pitch) { pitch_ = pitch; }
	pitch_t getPitch() { return pitch_; }

	void setPitchType(pitch_type_t pitch_type) { pitch_type_ = pitch_type; }
	pitch_type_t getPitchType() { return pitch_type_; }

	void addArea(Area *area) { areas_.push_back(area); }
	void removeArea(Area *area);

	std::pair<float, float> getInterval() { return {interval_, octave_subdivision_}; }
	void setInterval(float interval, float oct_sub)
	{
		interval_ = interval;
		octave_subdivision_ = oct_sub;
	}

	bool hasNoAreas() { return areas_.empty(); }

	void randomize();

private:
	bool is_active_;
	pitch_t pitch_;
	pitch_type_t pitch_type_;

	float interval_ = 7;
	float octave_subdivision_ = 12;

	std::vector<Area *> areas_;
};