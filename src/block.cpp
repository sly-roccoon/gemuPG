#include "block.h"
#include "gui.h"
#include <SDL3/SDL.h>

Block::Block(Vector2f pos)
{
	type_ = BLOCK_GENERATOR;
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
	SDL_SetAudioStreamGetCallback(stream_, this->audioCallback, this);
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

		for (i = 0; i < total; i++)
		{
			unsigned int idx = (unsigned int)(std::floorf(block->getData().phase * block->getFrequency() * WAVE_SIZE / SAMPLE_RATE)) % WAVE_SIZE;

			if (block->getBypass())
				samples[i] = 0.0f;
			else
			{
				samples[i] = block->getAmp() * block->getData().wave[idx];
				if (ADJUST_AMP_BY_CREST)
					samples[i] *= block->getData().crest; // TODO: find better way as to not go over +-1.0f
			}

			block->incrPhase();
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

void BlockGenerator::setFrequency(pitch_t freq)
{
	data_.freq = (freq + rel_freq_) * freq_factor_;
}

void BlockGenerator::drawGUI()
{
	if (!viewGUI_)
		return;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize;
	ImGui::SetNextWindowSize({512, 256});
	ImGui::SetNextWindowPos(ImGui::GetMousePos(), ImGuiCond_Appearing);

	ImGui::Begin(std::format("generator block @ [{}, {}]", rect_.x, rect_.y).c_str(), &viewGUI_, flags);

	ImGui::DragFloat("amplitude", &data_.amp, 0.001f, 0.0f, 1.0f, "% .3f", ImGuiSliderFlags_Logarithmic);
	if (!is_in_area_)
		ImGui::SliderFloat("frequency", &data_.freq, 20.0f, 20000.0f, "% .2f", ImGuiSliderFlags_Logarithmic);
	else
	{
		ImGui::SliderFloat("relative frequency", &rel_freq_, -20.0f, 20.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("frequency multiplicator", &freq_factor_, 1.0f / 16.0f, 16.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
	}

	// ImGui::SliderFloat("Pan", &data_.pan, -1.0f, 1.0f, "% .1f");

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

	pitch_type_ = PITCH_ABS_FREQUENCY;
	pitch_ = 440.0f;

	if (SEQUENCER_RANDOMIZED)
		randomize();
}

void BlockSequencer::randomize()
{
	pitch_type_ = (pitch_type_t)SDL_rand(4);

	// just some random values in decently pleasant ranges...
	if (pitch_type_ == PITCH_ABS_FREQUENCY || pitch_type_ == PITCH_NOTE || pitch_type_ == PITCH_INTERVAL)
		do
			pitch_ = SDL_pow(1000, SDL_randf() - 1) * 20000.0f;
		while (pitch_ < 50.0f || pitch_ > 880.0f);

	if (pitch_type_ == PITCH_REL_FREQUENCY)
		pitch_ = SDL_randf() * 1000.0f;

	if (pitch_type_ == PITCH_INTERVAL)
	{
		octave_subdivision_ = SDL_rand(24) + 1;
		interval_ = SDL_rand(octave_subdivision_) + 1;
	}
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
	ImGui::SetNextWindowPos(ImGui::GetMousePos(), ImGuiCond_Appearing);

	ImGui::Begin(std::format("Sequencer Block @ [{}, {}]", rect_.x, rect_.y).c_str(), &viewGUI_, flags);

	const char *preview = pitch_type_ == PITCH_REL_FREQUENCY   ? "relative frequency"
						  : pitch_type_ == PITCH_ABS_FREQUENCY ? "absolute frequency"
						  : pitch_type_ == PITCH_INTERVAL	   ? "interval"
						  : pitch_type_ == PITCH_NOTE		   ? "note"
															   : "unknown";

	if (ImGui::BeginCombo("Type", preview))
	{
		if (ImGui::Selectable("relative frequency"))
			setPitchType(PITCH_REL_FREQUENCY);
		if (ImGui::Selectable("absolute frequency"))
			setPitchType(PITCH_ABS_FREQUENCY);
		if (ImGui::Selectable("interval"))
			setPitchType(PITCH_INTERVAL);
		if (ImGui::Selectable("note"))
			setPitchType(PITCH_NOTE);
		ImGui::EndCombo();
	}

	if (pitch_type_ == PITCH_REL_FREQUENCY)
		ImGui::SliderFloat("frequency diff.", &pitch_, -1000.0f, 1000.0f, "%.2f", ImGuiSliderFlags_Logarithmic);

	if (pitch_type_ == PITCH_ABS_FREQUENCY)
		ImGui::SliderFloat("frequency", &pitch_, 20.0f, 20000.0f, "% .2f", ImGuiSliderFlags_Logarithmic);

	if (pitch_type_ == PITCH_INTERVAL)
	{
		ImGui::SliderFloat("Interval", &interval_, -octave_subdivision_ * 2, octave_subdivision_ * 2, "%.1f", ImGuiSliderFlags_NoRoundToFormat);
		ImGui::SliderFloat("octave subdivision", &octave_subdivision_, 1.0f, 24.0f, "%.1f", ImGuiSliderFlags_NoRoundToFormat);
	}

	if (pitch_type_ == PITCH_NOTE)
	{
		auto [note, octave] = freqToNote(pitch_);
		ImGui::SetNextItemWidth(128);
		ImGui::SliderInt("octave", &octave, 0, 10);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(128);
		if (ImGui::BeginCombo("note", note.c_str()))
		{
			if (ImGui::Selectable("C"))
				note = "C";
			if (ImGui::Selectable("C#"))
				note = "C#";
			if (ImGui::Selectable("D"))
				note = "D";
			if (ImGui::Selectable("D#"))
				note = "D#";
			if (ImGui::Selectable("E"))
				note = "E";
			if (ImGui::Selectable("F"))
				note = "F";
			if (ImGui::Selectable("F#"))
				note = "F#";
			if (ImGui::Selectable("G"))
				note = "G";
			if (ImGui::Selectable("G#"))
				note = "G#";
			if (ImGui::Selectable("A"))
				note = "A";
			if (ImGui::Selectable("A#"))
				note = "A#";
			if (ImGui::Selectable("B"))
				note = "B";
			ImGui::EndCombo();
		}

		pitch_ = noteToFreq(note, octave);
	}

	ImGui::End();
}

void BlockSequencer::removeArea(Area *area)
{
	areas_.erase(std::remove(areas_.begin(), areas_.end(), area), areas_.end());
}

SDL_FRect *BlockSequencer::getFRect()
{
	if (!is_active_)
		render_rect_ = Block::smallerFRect(rect_);
	else
		render_rect_ = rect_;

	render_rect_ = Camera::resizeFRect(render_rect_);
	return &render_rect_;
}