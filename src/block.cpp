#include "block.h"
#include "gui.h"
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

    copy->setData({data_.wave, data_.amp, data_.freq, data_.pan, data_.phase});

    return copy;
}

BlockGenerator::BlockGenerator(Vector2f pos, float phase) : Block(pos)
{
    type_ = BLOCK_GENERATOR;
    data_.phase = phase;

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

        for (i = 0; i < total; i++)
        {
            samples[i] = block->getData().amp * sinf(TWOPI * block->getData().freq * block->getData().phase / SAMPLE_RATE);
            block->incrPhase();
        }

        SDL_PutAudioStreamData(stream, samples, total * sizeof(float));
        additional_amount -= total;
    }
}

void BlockGenerator::drawGUI()
{
    if (!viewGUI_)
        return;

    ImGui::Begin(std::format("Generator Block @ [{}, {}]", rect_.x, rect_.y).c_str(), &viewGUI_);

    ImGuiSliderFlags log = ImGuiSliderFlags_Logarithmic;
    ImGui::SliderFloat("Amplitude", &data_.amp, 0.0f, 1.0f, "% .2f");
    ImGui::SliderFloat("Frequency", &data_.freq, 20.0f, 20000.0f, "% .2f", log);
    ImGui::SliderFloat("Pan", &data_.pan, -1.0f, 1.0f, "% .1f");

    ImGui::End();
}