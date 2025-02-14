#include "text.h"

TTF_Font *Text::default_font_ = nullptr;
TTF_TextEngine *Text::text_engine_ = nullptr;
SDL_Renderer *Text::renderer_ = nullptr;

void Text::init(SDL_Renderer *renderer)
{
    renderer_ = renderer;
    std::string font_path = SDL_GetBasePath();
    font_path.append("/");
    font_path.append(DEFAULT_FONT);
    default_font_ = TTF_OpenFont(font_path.c_str(), 128);
    text_engine_ = TTF_CreateRendererTextEngine(renderer_);
}

SDL_Texture *Text::getTexture(std::string text, SDL_Color col)
{
    SDL_Surface *surface = TTF_RenderText_Blended(default_font_, text.c_str(), 0, col);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_DestroySurface(surface);

    return texture;
}

void Text::drawTexture(SDL_Texture *texture, Vector2f pos, Vector2f constraints, bool centered)
{
    Vector2f texture_size;
    SDL_GetTextureSize(texture, &texture_size.x, &texture_size.y);
    if (texture_size.y > constraints.y)
    {
        texture_size.x = constraints.y * texture_size.x / texture_size.y;
        texture_size.y = constraints.y;
    }
    if (texture_size.x > constraints.x)
    {
        texture_size.y = constraints.x * texture_size.y / texture_size.x;
        texture_size.x = constraints.x;
    }

    if (centered)
    {
        pos = {pos.x - texture_size.x / 2, pos.y - texture_size.y / 2};
    }

    SDL_FRect dest = {pos.x, pos.y, texture_size.x, texture_size.y};

    SDL_RenderTexture(renderer_, texture, NULL, &dest);
}

Vector2f Text::getTextSize(std::string text)
{
    int width, height;
    TTF_Text *ttf_text = TTF_CreateText(text_engine_, default_font_, text.c_str(), 0);
    TTF_GetTextSize(ttf_text, &width, &height);
    TTF_DestroyText(ttf_text);

    float font_size = TTF_GetFontSize(default_font_);

    return {(float)width / font_size, (float)height / font_size};
}