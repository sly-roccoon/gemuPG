#include "block.h"
#include "gui.h"
#include <SDL3/SDL.h>

Block::Block(Vector2f pos)
{
	type_ = BLOCK_GENERATOR;
	bypass_ = true;
	rect_ = {pos.x, pos.y, 1.0f, 1.0f};
}

SDL_FRect Block::smallerFRect(SDL_FRect rect)
{
	rect.x += rect.w * (1 - BLOCK_SIZE_FACTOR) / 2;
	rect.y += rect.h * (1 - BLOCK_SIZE_FACTOR) / 2;
	rect.w *= BLOCK_SIZE_FACTOR;
	rect.h *= BLOCK_SIZE_FACTOR;
	return rect;
}

SDL_FRect *Block::getFRect()
{
	render_rect_ = Block::smallerFRect(rect_);
	render_rect_ = Camera::resizeFRect(render_rect_);
	return &render_rect_;
}

BlockGenerator *BlockGenerator::clone()
{
	BlockGenerator *copy = new BlockGenerator({rect_.x, rect_.y});

	copy->setData(data_);

	return copy;
}

BlockGenerator::BlockGenerator(Vector2f pos, float phase) : Block(pos)
{
	type_ = BLOCK_GENERATOR;
	data_.phase = phase;
	setWave(WAVE_SINE);

	stream_ = SDL_CreateAudioStream(&DEFAULT_SPEC, &DEFAULT_SPEC);
	SDL_SetAudioStreamGetCallback(stream_, audioCallback, this);
}

BlockGenerator::~BlockGenerator()
{
	SDL_DestroyAudioStream(stream_);
}

void BlockGenerator::audioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{
	BlockGenerator *block = (BlockGenerator *)userdata;
	float delta = block->getData().freq / SAMPLE_RATE;
	additional_amount /= sizeof(float);
	while (additional_amount > 0)
	{
		float samples[BUFFER_SIZE] = {};
		const int total = SDL_min(additional_amount, SDL_arraysize(samples));
		int i;

		generator_data_t data = block->getData();

		for (i = 0; i < total; i++)
		{
			unsigned int idx = (int)(std::floorf(block->getData().phase * data.freq * WAVE_SIZE / SAMPLE_RATE)) % WAVE_SIZE;
			samples[i] = data.amp * data.wave[idx];
			block->incrPhase();

			if (ADJUST_AMP_BY_CREST)
			{
				samples[i] *= data.crest; // TODO: find better way as to not go over +-1.0f
			}
		}

		SDL_PutAudioStreamData(stream, samples, total * sizeof(float));
		additional_amount -= total;
	}
}

void BlockGenerator::setWave(WAVE_FORMS waveform, float *wave)
{
	data_.waveform = waveform;

	if (waveform == WAVE_SAMPLE)
	{
		if (wave)
			data_.wave = wave;
	}
	else if (waveform == WAVE_SAW)
	{
		for (int i = 0; i < WAVE_SIZE; i++)
			data_.wave[i] = 1.0f - (2.0f * i / WAVE_SIZE);
	}

	else if (waveform == WAVE_SINE)
	{
		for (int i = 0; i < WAVE_SIZE; i++)
			data_.wave[i] = sinf(TWOPI * i / WAVE_SIZE);
	}

	else if (waveform == WAVE_SQUARE)
	{
		for (int i = 0; i < WAVE_SIZE; i++)
			data_.wave[i] = (i < WAVE_SIZE / 2) ? 1.0f : -1.0f;
	}

	else if (waveform == WAVE_TRIANGLE)
	{
		for (int i = 0; i < WAVE_SIZE; i++)
		{
			if (i < WAVE_SIZE / 4)
				data_.wave[i] = 4.0f * i / WAVE_SIZE;
			else if (i < 3 * WAVE_SIZE / 4)
				data_.wave[i] = 2.0f - 4.0f * i / WAVE_SIZE;
			else
				data_.wave[i] = -4.0f + 4.0f * i / WAVE_SIZE;
		}
	}

	data_.crest = calcCrest(data_.wave);
}

void BlockGenerator::drawGUI()
{
	if (!viewGUI_)
		return;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize;
	ImGui::SetNextWindowSize({512, 256});
	ImGui::Begin(std::format("Generator Block @ [{}, {}]", rect_.x, rect_.y).c_str(), &viewGUI_, flags);

	ImGuiSliderFlags log = ImGuiSliderFlags_Logarithmic;
	ImGui::SliderFloat("Amplitude", &data_.amp, 0.0f, 1.0f, "% .2f");
	ImGui::SliderFloat("Frequency", &data_.freq, 20.0f, 20000.0f, "% .2f", log);
	ImGui::SliderFloat("Pan", &data_.pan, -1.0f, 1.0f, "% .1f");

	const char *preview = data_.waveform == WAVE_SAW ? "Saw" : data_.waveform == WAVE_SINE	 ? "Sine"
														   : data_.waveform == WAVE_SQUARE	 ? "Square"
														   : data_.waveform == WAVE_TRIANGLE ? "Triangle"
																							 : "Sample";

	if (ImGui::BeginCombo("Waveform", preview))
	{
		// if (ImGui::Selectable("Sample"))
		// 	setWave(WAVE_SAMPLE); //TODO: implement sample loading
		if (ImGui::Selectable("Saw"))
			setWave(WAVE_SAW);
		if (ImGui::Selectable("Sine"))
			setWave(WAVE_SINE);
		if (ImGui::Selectable("Square"))
			setWave(WAVE_SQUARE);
		if (ImGui::Selectable("Triangle"))
			setWave(WAVE_TRIANGLE);
		ImGui::EndCombo();
	}

	ImGui::PlotLines("##Waveform", data_.wave, WAVE_SIZE, 0, "WAVEFORM", -1.0f, 1.0f, ImVec2(512, 128));

	ImGui::End();
}

//--------------------------------------------------------
BlockSequencer::BlockSequencer(Vector2f pos) : Block(pos)
{
	type_ = BLOCK_SEQUENCER;
	rect_ = {pos.x, pos.y, 1.0f, 1.0f};

	pitch_ = 440.0f;
	pitch_type_ = PITCH_ABS_FREQUENCY;
}

BlockSequencer *BlockSequencer::clone()
{
	BlockSequencer *copy = new BlockSequencer({rect_.x, rect_.y});
	return copy;
}

void BlockSequencer::drawGUI()
{
	if (!viewGUI_)
		return;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize;
	ImGui::SetNextWindowSize({512, 256});
	ImGui::Begin(std::format("Sequencer Block @ [{}, {}]", rect_.x, rect_.y).c_str(), &viewGUI_, flags);

	ImGui::End();
}

void BlockSequencer::removeArea(Area *area)
{
	areas_.erase(std::remove(areas_.begin(), areas_.end(), area), areas_.end());
}