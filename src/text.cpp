#include "text.h"

TTF_Font *Text::default_font_ = nullptr;
TTF_TextEngine *Text::text_engine_ = nullptr;
SDL_Renderer *Text::renderer_ = nullptr;

void Text::init(SDL_Renderer *renderer)
{
    renderer_ = renderer;
    std::string font_path = SDL_GetBasePath();
    font_path.append("/terminal_f4.ttf");
    default_font_ = TTF_OpenFont(font_path.c_str(), 16);
    text_engine_ = TTF_CreateRendererTextEngine(renderer_);
}

void Text::draw(std::string text, Vector2f pos, SDL_Color col, float size)
{
    TTF_SetFontSize(default_font_, size);
    TTF_Text *ttf_text = TTF_CreateText(text_engine_, default_font_, text.c_str(), 0);
    TTF_SetTextColor(ttf_text, col.r, col.g, col.b, col.a);
    TTF_DrawRendererText(ttf_text, pos.x, pos.y);

    TTF_DestroyText(ttf_text);
}

Vector2f Text::getTextSize(std::string text, float size)
{
    int width, height;
    TTF_SetFontSize(default_font_, size);
    TTF_Text *ttf_text = TTF_CreateText(text_engine_, default_font_, text.c_str(), 0);
    TTF_GetTextSize(ttf_text, &width, &height);
    TTF_DestroyText(ttf_text);

    return {(float)width, (float)height};
}