#include "block.h"
#include <SDL3/SDL.h>

Block::Block(Vector2f pos)
{
    type_ = BLOCK_NONE;
    bypass_ = true;
    rect_ = {pos.x, pos.y, 1.0f, 1.0f};
}

SDL_FRect *Block::getFRect()
{
    render_rect_ = Camera::resizeFRect(rect_);
    return &render_rect_;
}

Block *BlockGenerator::clone()
{
    BlockGenerator *copy = new BlockGenerator({rect_.x, rect_.y});

    copy->amp = amp;
    copy->freq = freq;
    copy->pan = pan;

    return copy;
}

BlockGenerator::BlockGenerator(Vector2f pos) : Block(pos)
{
    type_ = BLOCK_GENERATOR;
    amp = 1.0f;
    freq = 440.0f;
    pan = 0.0f;

    stream_ = SDL_CreateAudioStream(&DEFAULT_SPEC, nullptr);
    SDL_SetAudioStreamGetCallback(stream_, audioCallback, this);
}

BlockGenerator::~BlockGenerator()
{
    /*UnloadAudioStream(stream_);*/
}

void BlockGenerator::processAudio()
{
}

void BlockGenerator::audioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{
    BlockGenerator *block = (BlockGenerator *)userdata;
    float delta = block->freq / SAMPLE_RATE;
    additional_amount /= sizeof(float);
    while (additional_amount > 0)
    {
        float samples[BUFFER_SIZE];
        const int total = SDL_min(additional_amount, SDL_arraysize(samples));
        int i;

        for (i = 0; i < total; i++)
        {
            samples[i] = SDL_sinf(TWOPI * block->freq * block->phase / SAMPLE_RATE);
            block->phase++;
        }

        SDL_PutAudioStreamData(stream, samples, total * sizeof(float));
        additional_amount -= total;
    }
}