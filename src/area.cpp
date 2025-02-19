#include "area.h"
#include "util.h"
#include <algorithm>
#include <format>

bool Area::isInside(Vector2f pos)
{
	pos = floorVec(pos);
	for (auto &field : positions_)
		if (field == pos)
			return true;

	return false;
}

void Area::addPositions(std::vector<Vector2f> positions)
{
	for (auto &pos : positions)
		positions_.push_back(pos);
}

bool Area::addPosition(Vector2f pos)
{
	pos = floorVec(pos);
	if (std::find(positions_.begin(), positions_.end(), pos) != positions_.end())
		return false;
	positions_.push_back(pos);
	return true;
}

void Area::removePosition(Vector2f pos)
{
	pos = floorVec(pos);

	positions_.erase(
		std::remove_if(positions_.begin(), positions_.end(),
					   [pos](auto p)
					   { return p == pos; }),
		positions_.end());
}

Block *Area::addBlock(Block *block)
{
	if (block)
	{
		blocks_.push_back(block);
		if (block->getType() == BLOCK_GENERATOR)
		{
			BlockGenerator *block_g = (BlockGenerator *)block;
			block_g->setInArea(true);
			updateGlissando(block_g);
			updateAttack(block_g);
			(block_g)->setFrequency(0.0f);
		}
		return block;
	}
	return nullptr;
}

void Area::removeBlock(Block *block)
{
	blocks_.erase(
		std::remove_if(blocks_.begin(), blocks_.end(),
					   [block](auto b)
					   { return b == block; }),
		blocks_.end());

	if (block && block->getType())
		((BlockGenerator *)block)->setInArea(false);
}

bool Area::removeBlock(Vector2f pos)
{
	pos = floorVec(pos);

	return (blocks_.end() != blocks_.erase(
								 std::remove_if(blocks_.begin(), blocks_.end(),
												[pos](auto block)
												{ return block->getPos() == pos; }),
								 blocks_.end()));
}

Block *Area::getBlock(Vector2f pos)
{
	pos = floorVec(pos);

	for (auto &block : blocks_)
		if (block->getPos() == pos)
			return block;

	return nullptr;
}

BlockSequencer *Area::getSequencer(Vector2f pos)
{
	pos = floorVec(pos);

	for (auto &sequencer : sequence_)
		if (sequencer && sequencer->getPos() == pos)
			return sequencer;

	return nullptr;
}

Block *Area::addSequencer(BlockSequencer *sequencer)
{
	sequence_.push_back(sequencer);
	sequencer->addArea(this);

	updateSequence();
	return sequencer;
}

void Area::removeSequencer(BlockSequencer *sequencer)
{
	if (sequence_.erase(
			std::remove_if(sequence_.begin(), sequence_.end(),
						   [sequencer](auto s)
						   { return s == sequencer; }),
			sequence_.end()) != sequence_.end())
	{
		sequencer->removeArea(this);
		if (sequencer->hasNoAreas())
			delete sequencer;
		updateSequence();
	}
}

bool Area::removeSequencer(Vector2f pos)
{
	for (auto sequencer : sequence_)
		if (sequencer && sequencer->getPos() == pos)
		{
			removeSequencer(sequencer);
			return true;
		}
	return false;
}

void Area::removeDanglingSequencers()
{
	for (auto sequencer : sequence_)
	{
		if (!sequencer)
			continue;

		auto pos = sequencer->getPos();
		if (!isInside({pos.x + 1, pos.y}) &&
			!isInside({pos.x - 1, pos.y}) &&
			!isInside({pos.x, pos.y + 1}) &&
			!isInside({pos.x, pos.y - 1}))
		{
			sequence_.erase(
				std::remove_if(sequence_.begin(), sequence_.end(),
							   [sequencer](auto s)
							   { return s == sequencer; }),
				sequence_.end());
			sequencer->removeArea(this);

			if (sequencer->hasNoAreas())
				delete sequencer;
		}
	}
}

Vector2f Area::getTopLeft()
{
	Vector2f top_left = {GRID_SIZE + 1, GRID_SIZE + 1};

	for (auto &pos : positions_) // find topleft most position, top > left
	{
		if (pos.y < top_left.y)
			top_left = pos;
		else if (pos.y == top_left.y && pos.x < top_left.x)
			top_left = pos;
	}

	return top_left;
}

void Area::cleanupSequence()
{
	// remove nullptrs
	sequence_.erase(
		std::remove_if(sequence_.begin(), sequence_.end(),
					   [](BlockSequencer *sequencer)
					   { return sequencer == nullptr; }),
		sequence_.end());

	// remove duplicates
	std::sort(sequence_.begin(), sequence_.end());
	sequence_.erase(
		std::unique(sequence_.begin(), sequence_.end()),
		sequence_.end());

	removeDanglingSequencers();
}

void Area::updateSequence()
{
	std::vector<BlockSequencer *> new_sequence;

	cleanupSequence();

	int dir = 0;
	auto cur_pos = getAdjacentPositions(getTopLeft()).at(dir);
	auto start_pos = cur_pos;
	auto next_dir = [](int dir, int n_times = 1)
	{ return (dir + n_times) % 4; };

	do // directions: 0 = up, 1 = left, 2 = down, 3 = right | normal direction from side of area
	{  // i know this looks convoluted, but i swear this makes sense...
		auto adj_pos = getAdjacentPositions(cur_pos);
		new_sequence.push_back(getSequencer(cur_pos));

		if (!isInside(getAdjacentPositions(adj_pos.at(next_dir(dir))).at(next_dir(dir, 2)))) // outer corner / left rotation
		{
			cur_pos = getAdjacentPositions(adj_pos.at(next_dir(dir))).at(next_dir(dir, 2));
			dir = next_dir(dir);
			continue;
		}

		if (!isInside(adj_pos.at(next_dir(dir)))) // continue straight?
		{
			cur_pos = adj_pos.at(next_dir(dir));
			continue;
		}

		// around the inner corner / right rotation
		// cur_pos = cur_pos;  //position stays the same because in inner corner two sides of an area block touch sequencer block
		dir = next_dir(dir, 3); // rotate 3 times effectively

	} while (cur_pos != start_pos);

	sequence_ = new_sequence;
}

void Area::setNotes(pitch_t freq)
{
	for (auto block : blocks_)
	{
		if (block->getType() == BLOCK_GENERATOR)
		{
			auto generator = (BlockGenerator *)block;
			generator->setFrequency(freq);
		}
	}
};

void Area::stepSequence()
{
	unsigned int counter = Clock::getCounter();
	if (counter % (MAX_SUBDIVISION / bpm_subdivision_) != 0)
		return;

	last_note_idx_ = cur_note_idx_;
	cur_note_idx_ = cur_note_idx_ >= sequence_.size() - 1 ? 0 : cur_note_idx_ + 1;

	if (sequence_.at(last_note_idx_))
		sequence_.at(last_note_idx_)->setActive(false);

	if (!sequence_.at(cur_note_idx_))
		setNotes(0.0f);
	else
	{
		sequence_.at(cur_note_idx_)->setActive(true);

		switch (sequence_.at(cur_note_idx_)->getPitchType())
		{
		case PITCH_ABS_FREQUENCY:
		case PITCH_NOTE:
			last_freq_ = sequence_.at(cur_note_idx_)->getPitch();
			break;
		case PITCH_REL_FREQUENCY:
			last_freq_ += sequence_.at(cur_note_idx_)->getPitch();
			break;
		case PITCH_INTERVAL:
			auto [interval, oct_sub] = sequence_.at(cur_note_idx_)->getInterval();
			last_freq_ = last_freq_ * intervalToRatio(interval, oct_sub);
			break;
		}

		if (last_freq_ < 20.0f)
			last_freq_ += 20000.0f;
		else if (last_freq_ > 20000.0f)
			last_freq_ -= 20000.0f;

		setNotes(last_freq_);
	}
};

void Area::stopSequence()
{
	cur_note_idx_ = 0;
	last_note_idx_ = sequence_.size() - 1;
}

void Area::updateGlissando(BlockGenerator *block)
{
	double time = (gliss_percent_ / 100.0f) * (60.0f / (bpm_subdivision_ * Clock::getInstance().getBPM()));

	if (block)
	{
		((BlockGenerator *)block)->setGlissTime(time);
		return;
	}

	for (auto block : blocks_)
		if (block->getType() == BLOCK_GENERATOR)
			((BlockGenerator *)block)->setGlissTime(time);
}

void Area::updateAttack(BlockGenerator *block)
{
	double time = (attack_percent_ / 100.0f) * (60.0f / (bpm_subdivision_ * Clock::getInstance().getBPM()));

	if (block)
	{
		((BlockGenerator *)block)->setAttackTime(time);
		return;
	}

	for (auto block : blocks_)
		if (block->getType() == BLOCK_GENERATOR)
			((BlockGenerator *)block)->setAttackTime(time);
}

void Area::drawGUI()
{
	if (!viewGUI_)
		return;
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize;
	ImGui::SetNextWindowSize({IMGUI_WIN_SIZE.x * RENDER_SCALE, IMGUI_WIN_SIZE.y * RENDER_SCALE});
	ImGui::SetNextWindowPos(ImGui::GetMousePos(), ImGuiCond_Appearing);
	ImGui::Begin(std::format("Area @ [{}, {}]", getTopLeft().x, getTopLeft().y).c_str(), &viewGUI_, flags);

	if (ImGui::DragInt("bpm subdivision", &bpm_subdivision_, 1, 1, 32, "1/%d", ImGuiSliderFlags_Logarithmic))
	{
		updateGlissando();
		updateAttack();
	}

	ImGui::DragFloat("amplitude", &amp_, 0.001f, 0.0f, 1.0f, "% .3f", ImGuiSliderFlags_Logarithmic);

	if (ImGui::SliderFloat("glissando", &gliss_percent_, 0.0f, 100.0f, "%.1f %%", ImGuiSliderFlags_AlwaysClamp))
		updateGlissando();
	if (ImGui::SliderFloat("attack", &attack_percent_, 0.0f, 100.0f, "%.1f %%", ImGuiSliderFlags_AlwaysClamp))
		updateAttack();

	ImGui::End();
}