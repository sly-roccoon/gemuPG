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

protected:
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

class BlockGenerator : public Block
{
public:
    BlockGenerator(Vector2f);
    ~BlockGenerator();
    Block *clone() override;

    void processAudio();
    float amp;
    float pan;
    float freq;
    float phase;

private:
    SDL_AudioStream *stream_;

    static void audioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount);
};