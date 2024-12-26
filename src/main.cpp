#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include "interface.h"
#include "input.h"
#include "audio.h"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
	SDL_SetAppMetadata("GemuPG", "0.1.0", "Poly* Audio Software");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	Interface &interface = Interface::getInstance();
	Input &input = Input::getInstance();
	AudioEngine &audio = AudioEngine::getInstance();

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
	Interface::getInstance().draw();

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	if (event->type == SDL_EVENT_QUIT)
		return SDL_APP_SUCCESS;

	Input::getInstance().handleEvent(event);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	Interface::getInstance().destroy();

	SDL_Quit();
}