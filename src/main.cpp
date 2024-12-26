// #include "audio.h"
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "interface.h"
#include "input.h"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
	SDL_SetAppMetadata("GemuPG", "0.1.0", "Poly* Audio Software");

	Interface &interface = Interface::getInstance();
	// InputHandler& input = InputHandler::getInstance();
	// AudioEngine& audio = AudioEngine::getInstance();

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

	InputHandler::getInstance().handleEvent(event);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	return;
}