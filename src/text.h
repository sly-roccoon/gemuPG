#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <string>

#include "util.h"

class Text
{
public:
    static Text &getInstance()
    {
        static Text instance;
        return instance;
    }
    static void init(SDL_Renderer *);

    static SDL_Texture *getTexture(std::string text, SDL_Color col);
    static Vector2f getTextSize(std::string text);

    static void drawTexture(SDL_Texture *, Vector2f, Vector2f, bool centered = false);

private:
    Text() {};
    Text(const Text &) = delete;
    Text &operator=(const Text &) = delete;

    static TTF_Font *default_font_;
    static TTF_TextEngine *text_engine_;
    static SDL_Renderer *renderer_;
};