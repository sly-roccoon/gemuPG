#pragma once
#include "ofMain.h"
#include "ofxPDSP.h"
#include <cstdint>

#pragma region BASE BLOCK

typedef enum {
	BLOCK_NONE,
	BLOCK_GENERATOR,
	BLOCK_SEQUENCER,
	BLOCK_SETTINGS,
	BLOCK_EFFECT,
	BLOCK_MODULATOR
} blockType;

typedef glm::ivec2 Vec2;

class Block
{
public:
	Block();
	Vec2 getPos() { return pos_; }

protected:
	blockType type_;
	Vec2 pos_;
	bool bypass_;
};

#pragma endregion

#pragma region GENERATOR BLOCK

typedef enum {
	WAVE_SAMPLE,
	WAVE_SAW,
	WAVE_SINE,
	WAVE_SQUARE,
	WAVE_TRIANGLE
} WAVE_FORMS;

class BlockGenerator : public Block
{
public:
	BlockGenerator(Vec2);

private:
	pdsp::TableOscillator	osc_;
	pdsp::ADSR				adsr_;
	float					amp_;
	float					pan_;
};

#pragma endregion

#pragma region BLOCK FACTORY

class BlockFactory
{
public:
    static BlockFactory& getInstance() {
        static BlockFactory instance;
        return instance;
    }

    std::shared_ptr<Block> createBlock(const blockType type, const Vec2 pos)
    {
        switch (type)
        {
        case BLOCK_GENERATOR:
            return std::make_unique<BlockGenerator>(pos);
        default:
            return nullptr;
        }
    }

private:
    BlockFactory() = default;
    BlockFactory(const BlockFactory&) = delete;
    BlockFactory& operator=(const BlockFactory&) = delete;
};



#pragma endregion