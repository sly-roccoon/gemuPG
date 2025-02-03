#include "area.h"
#include <algorithm>
#include "util.h"

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

void Area::addBlock(Block *block)
{
	if (block)
		blocks_.push_back(block);
}

void Area::removeBlock(Block *block)
{
	blocks_.erase(
		std::remove_if(blocks_.begin(), blocks_.end(),
					   [block](auto b)
					   { return b == block; }),
		blocks_.end());
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

void Area::addSequencer(BlockSequencer *sequencer)
{
	sequence_.push_back(sequencer);
	sequencer->addArea(this);

	updateSequence();
}

void Area::removeSequencer(BlockSequencer *sequencer)
{
	sequence_.erase(
		std::remove_if(sequence_.begin(), sequence_.end(),
					   [sequencer](auto s)
					   { return s == sequencer; }),
		sequence_.end());
	sequencer->removeArea(this);
	if (sequencer->hasNoAreas())
		delete sequencer;

	updateSequence();
}

void Area::removeDanglingSequencers()
{
	for (auto sequencer : sequence_)
	{
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

void Area::updateSequence()
{
	removeDanglingSequencers();
	// TODO: sorting algorithm, nullptr signals pause
}

bool Area::sequencerExists(Vector2f pos)
{
	for (auto &sequencer : sequence_)
		if (sequencer->getPos() == pos)
			return true;

	return false;
}