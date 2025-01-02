#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <format>
#include "util.h"

class GUI
{
public:
	static bool init(SDL_Window *, SDL_Renderer *);
	static void destroy();

	static void drawToolbar();

private:
	static ImGuiContext *context_;
	static SDL_Window *window_;
	static SDL_Renderer *renderer_;
};