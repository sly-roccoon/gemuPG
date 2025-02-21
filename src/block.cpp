#include "block.h"
#include "gui.h"
#include "audio.h"

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

void BlockGenerator::copySample(Sample *sample)
{
	sample_.openPath(sample->getPath());
	sample_.setPlayType(sample->getPlayType());
	sample_.setRoot(sample->getRoot());
}

BlockGenerator *BlockGenerator::clone()
{
	// BlockGenerator *copy = new BlockGenerator(*this);
	// copy->createAudioStream(); //audiocallback crashes?

	BlockGenerator *copy = new BlockGenerator({getPos().x, getPos().y}, data_.phase);

	copy->setData(data_);
	copy->copySample(&sample_);
	copy->setRelFreq(rel_freq_);
	copy->setFreqFactor(freq_factor_);

	return copy;
}

BlockGenerator::BlockGenerator(Vector2f pos, double phase) : Block(pos)
{
	type_ = BLOCK_GENERATOR;
	data_.phase = phase;
	fs_ = AudioEngine::getInstance().getSpec()->freq;
	setWave(WAVE_SINE);

	data_.freq = SDL_pow(10, SDL_randf() - 1) * 500.0f;
	setWave((WAVE_FORMS)(SDL_rand(4) + 1));

	createAudioStream();
}

void BlockGenerator::createAudioStream()
{
	SDL_DestroyAudioStream(stream_);

	SDL_AudioSpec *spec = AudioEngine::getInstance().getSpec();
	stream_ = SDL_CreateAudioStream(spec, spec);
	SDL_SetAudioStreamGetCallback(stream_, this->audioCallback, this);
}

BlockGenerator::~BlockGenerator()
{
	SDL_DestroyAudioStream(stream_);
}

double BlockGenerator::getPhase()
{
	if (getWaveForm() != WAVE_SAMPLE)
		data_.phase = SDL_fmod(data_.phase + (getFrequency() / fs_), 1.0);
	else if (sample_.getSize() > 0)
	{
		double root = sample_.getRoot();

		if (sample_.getTrigger() && sample_.getPlayType() == ONE_SHOT)
		{
			data_.phase = 0.0; // first sample of retrigger should always be [0]
			sample_.setTrigger(false);
		}
		else
			data_.phase = data_.phase + getFrequency() / (root * sample_.getSize());

		if (data_.phase > 1.0)
		{
			sample_.setPlayed(true);
		}

		data_.phase = SDL_fmod(data_.phase, 1.0);
	}
	else
		data_.phase = 0.0;

	data_.phase = SDL_clamp(data_.phase, 0.0, 1.0);
	return data_.phase;
}

void BlockGenerator::audioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{
	BlockGenerator *block = (BlockGenerator *)userdata;

	int fs = AudioEngine::getInstance().getSpec()->freq;

	additional_amount /= sizeof(float);
	while (additional_amount > 0)
	{
		float samples[BUFFER_SIZE] = {};
		const int total = SDL_min(additional_amount, BUFFER_SIZE);
		int i;
		double freq = block->getFrequency();

		std::array<float, WAVE_SIZE> *simple_wave;
		Sample *sample = block->getSample();

		if (block->getWaveForm() != WAVE_SAMPLE)
			simple_wave = AudioEngine::getWaveTable(block->getWaveForm(), freq);

		for (i = 0; i < total; i++)
		{
			if (freq == 0.0f)
			{
				block->getPhase();
				continue;
			}
			if (block->getBypass())
			{
				samples[i] = 0.0f;
				continue;
			}

			double amp = block->getAmp();
			if (block->getWaveForm() != WAVE_SAMPLE)
			{
				double idx = block->getPhase() * (WAVE_SIZE);

				samples[i] = amp * interpTable(simple_wave->data(), WAVE_SIZE, idx);
				if (ADJUST_AMP_BY_CREST)
					samples[i] *= block->getData()->crest * ONE_DIV_SQRT_THREE; // TODO: find better way as to not go over +-1.0f
			}
			else
			{
				if (sample->getSize() != 0 && (!sample->isPlayed() || sample->getPlayType() == REPEAT))
				{
					double idx = 0.0;

					float phase = block->getPhase();
					long size = block->getSample()->getSize();
					idx = SDL_fmod(phase * size, size);

					samples[i] = amp * interpTable(sample->getWave(), sample->getSize(), idx);
				}
				else
					samples[i] = 0.0f;
			}
		}

		SDL_PutAudioStreamData(stream, samples, total * sizeof(float));
		additional_amount -= total;
	}
}

void BlockGenerator::setWave(WAVE_FORMS waveform)
{
	data_.waveform = waveform;

	constexpr double PI = std::numbers::pi;
	constexpr double TWOPI = 2 * PI;

	if (waveform == WAVE_SAMPLE)
	{
		data_.disp_wave = *sample_.getDispWave();
	}

	else if (waveform == WAVE_SINE)
	{
		data_.disp_wave = {};
		for (int i = 0; i < WAVE_SIZE; i++)
			data_.disp_wave.at(i) = sinf(TWOPI * i / WAVE_SIZE);
	}

	else if (waveform == WAVE_SAW)
	{
		for (int i = 0; i < WAVE_SIZE; i++)
			data_.disp_wave[i] = 1.0f - (2.0f * i / WAVE_SIZE);
	}

	else if (waveform == WAVE_SQUARE)
	{
		for (int i = 0; i < WAVE_SIZE; i++)
			data_.disp_wave[i] = (i < WAVE_SIZE / 2) ? 1.0f : -1.0f;
	}

	else if (waveform == WAVE_TRIANGLE)
	{
		for (int i = 0; i < WAVE_SIZE; i++)
		{
			if (i < WAVE_SIZE / 4)
				data_.disp_wave[i] = 4.0f * i / WAVE_SIZE;
			else if (i < 3 * WAVE_SIZE / 4)
				data_.disp_wave[i] = 2.0f - 4.0f * i / WAVE_SIZE;
			else
				data_.disp_wave[i] = -4.0f + 4.0f * i / WAVE_SIZE;
		}
	}

	data_.crest = calcCrest(data_.disp_wave);
}

void BlockGenerator::setFrequency(pitch_t freq)
{
	last_freq_ = data_.freq;
	if (freq == 0.0f)
	{
		data_.freq = 0.0f;
		return;
	}

	if (last_freq_ == 0.0f)
	{
		sample_.setPlayed(false);
		sample_.setTrigger(true);
	}

	gliss_freq_ = last_freq_;
	last_note_change_ = SDL_GetPerformanceCounter();
	last_freq_ = data_.freq;
	data_.freq = SDL_clamp((freq + rel_freq_) * freq_factor_, 0.0, fs_ / 2);
}

double BlockGenerator::getAmp()
{
	if (!is_in_area_)
		return data_.amp;

	// Uint64 last = now;
	now = SDL_GetPerformanceCounter();

	// double dt = static_cast<double>(now - last) / static_cast<double>(PERFORMANCE_FREQUENCY);
	double dt = static_cast<double>(now - last_note_change_) / static_cast<double>(PERFORMANCE_FREQUENCY);
	if (now - last_note_change_ < attack_time_ * PERFORMANCE_FREQUENCY && attack_time_ != 0.0)
	{
		// double factor = SDL_exp(-ENV_TIME_CONST * static_cast<double>(dt) / static_cast<double>(attack_time_));
		// env_amp_ = SDL_clamp(data_.amp * (1.0 - factor), 0.0, data_.amp);
		double factor = static_cast<double>(dt) / static_cast<double>(attack_time_);
		env_amp_ = SDL_clamp(data_.amp * factor, 0.0, data_.amp);
	}
	else if (now - last_note_change_ > release_time_ * PERFORMANCE_FREQUENCY)
	{
		dt -= release_time_;
		double factor = SDL_exp(-ENV_TIME_CONST * static_cast<double>(dt) / static_cast<double>(note_length_ - release_time_));
		env_amp_ = SDL_clamp(data_.amp * factor, 0.0, data_.amp);
	}
	else
	{
		env_amp_ = data_.amp;
	}
	return SDL_clamp(env_amp_, 0.0, 1.0);
}

double BlockGenerator::getFrequency()
{
	constexpr float e = 10e-6;
	if (!is_in_area_ || last_freq_ == 0.0f || gliss_time_ == 0 || std::abs(data_.freq - gliss_freq_) < e)
		return data_.freq;

	Uint64 now = SDL_GetPerformanceCounter();
	double dt = static_cast<double>(now - last_note_change_) / static_cast<double>(SDL_GetPerformanceFrequency());

	double factor = static_cast<double>(dt) / static_cast<double>(gliss_time_);
	factor = SDL_clamp(factor, 0.0, 1.0);
	double ratio = data_.freq / last_freq_;
	gliss_freq_ = last_freq_ * pow(ratio, factor);

	return gliss_freq_;
}

void BlockGenerator::drawGUI()
{
	if (!viewGUI_)
		return;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize;
	ImVec2 margins = ImGui::GetStyle().WindowPadding;

	ImGui::SetNextWindowSize({IMGUI_WIN_SIZE.x * RENDER_SCALE, IMGUI_WIN_SIZE.y * RENDER_SCALE});
	ImGui::SetNextWindowPos(ImGui::GetMousePos(), ImGuiCond_Appearing);

	ImGui::Begin(std::format("generator block @ [{}, {}]", rect_.x, rect_.y).c_str(), &viewGUI_, flags);

	ImGui::DragFloat("amplitude", &data_.amp, 0.001f, 0.0f, 1.0f, "% .3f", ImGuiSliderFlags_Logarithmic);
	if (!is_in_area_)
		ImGui::SliderFloat("frequency", &data_.freq, 20.0f, 20000.0f, "% .2fHz", ImGuiSliderFlags_Logarithmic);
	else
	{
		ImGui::SliderFloat("relative frequency", &rel_freq_, -20.0f, 20.0f, "%.2fHz", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("frequency factor", &freq_factor_, 1.0f / 16.0f, 16.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
	}

	// ImGui::SliderFloat("Pan", &data_.pan, -1.0f, 1.0f, "% .1f");

	const char *preview = data_.waveform == WAVE_SAW ? "saw" : data_.waveform == WAVE_SINE	 ? "sine"
														   : data_.waveform == WAVE_SQUARE	 ? "square"
														   : data_.waveform == WAVE_TRIANGLE ? "triangle"
																							 : "sample";

	if (ImGui::BeginCombo("waveform", preview))
	{
		if (ImGui::Selectable("sample"))
			setWave(WAVE_SAMPLE); // TODO: implement sample loading
		if (ImGui::Selectable("saw"))
			setWave(WAVE_SAW);
		if (ImGui::Selectable("sine"))
			setWave(WAVE_SINE);
		if (ImGui::Selectable("square"))
			setWave(WAVE_SQUARE);
		if (ImGui::Selectable("triangle"))
			setWave(WAVE_TRIANGLE);
		ImGui::EndCombo();
	}

	if (data_.waveform == WAVE_SAMPLE)
	{
		if (ImGui::Button("load sample", {ICON_SIZE * RENDER_SCALE * 2, 0}))
		{
			sample_.open();

			data_.disp_wave = *sample_.getDispWave();
		}
		ImGui::SameLine();
		ImGui::Text(sample_.getName().c_str());

		if (is_in_area_)
		{
			const char *preview = sample_.getPlayType() == REPEAT ? "repeat" : "oneshot";
			if (ImGui::BeginCombo("play type", preview))
			{
				if (ImGui::Selectable("repeat"))
					sample_.setPlayType(REPEAT);
				if (ImGui::Selectable("oneshot"))
					sample_.setPlayType(ONE_SHOT);
				ImGui::EndCombo();
			}
		}

		pitch_t root = sample_.getRoot();
		if (ImGui::SliderFloat("sample root frequency", &root, 1.0f, 10'000.0f, "%.2fHz", ImGuiSliderFlags_Logarithmic))
			sample_.setRoot(root);
	}

	ImGui::PlotLines("##waveform", data_.disp_wave.data(), data_.disp_wave.size(), 0, "WAVEFORM", -1.0f, 1.0f, {ICON_SIZE * RENDER_SCALE * 8 - margins.x * 2, ICON_SIZE * RENDER_SCALE * 2 - margins.y * 2});

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
		pitch_ = SDL_randf() * 500.0f - 250.0f;

	if (pitch_type_ == PITCH_INTERVAL)
	{
		octave_subdivision_ = SDL_rand(24) + 1;
		interval_ = SDL_rand(octave_subdivision_) + 1;
	}
}

BlockSequencer *BlockSequencer::clone()
{
	BlockSequencer *copy = new BlockSequencer(*this);

	for (auto area : copy->getAreas())
		removeArea(area);
	copy->setActive(false);

	return copy;
}

void BlockSequencer::drawGUI()
{
	if (!viewGUI_)
		return;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize;
	ImGui::SetNextWindowSize({IMGUI_WIN_SIZE.x * RENDER_SCALE, IMGUI_WIN_SIZE.y * RENDER_SCALE});
	ImGui::SetNextWindowPos(ImGui::GetMousePos(), ImGuiCond_Appearing);

	ImGui::Begin(std::format("sequencer block @ [{}, {}]", rect_.x, rect_.y).c_str(), &viewGUI_, flags);

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
		ImGui::SliderFloat("interval", &interval_, -octave_subdivision_ * 2, octave_subdivision_ * 2, "%.1f", ImGuiSliderFlags_NoRoundToFormat);
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

void BlockSequencer::addArea(Area *area)
{
	if (std::find(areas_.begin(), areas_.end(), area) == areas_.end())
		areas_.push_back(area);
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