#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "interface.h"
#include "input.h"
#include "audio.h"
#include "gui.h"
#include "clock.h"
#include "util.h"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
	SDL_SetAppMetadata("GemuPG", "0.1.0", "Poly* Audio Software");

	Interface &interface = Interface::getInstance();
	Input &input = Input::getInstance();
	AudioEngine &audio = AudioEngine::getInstance();
	GUI::init(interface.getWindow(), interface.getRenderer());

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
	Interface::getInstance().draw();

	if (Clock::getInstance().isTime())
	{
		Interface::getInstance().getGrid().stepSequence();
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	if (event->type == SDL_EVENT_QUIT)
		return SDL_APP_SUCCESS;

	if (event->type == SDL_EVENT_WINDOW_DISPLAY_CHANGED || event->type == SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED)
	{
		Interface::updateScaling();
	}

	Input::getInstance().handleEvent(event);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	GUI::destroy();
	AudioEngine::getInstance().destroy();
	Interface::getInstance().destroy();

	SDL_Quit();
}