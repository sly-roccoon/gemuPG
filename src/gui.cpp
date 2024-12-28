#include "gui.h"
#include "command.h"

ImGuiContext *GUI::context_ = nullptr;
SDL_Window *GUI::window_ = nullptr;
SDL_Renderer *GUI::renderer_ = nullptr;

bool GUI::init(SDL_Window *window, SDL_Renderer *renderer)
{
	IMGUI_CHECKVERSION();
	context_ = ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	// ImGui::StyleColorsDark();

	window_ = window;
	renderer_ = renderer;

	if (!ImGui_ImplSDL3_InitForSDLRenderer(window_, renderer_))
		return false;

	if (!ImGui_ImplSDLRenderer3_Init(renderer_))
		return false;

	return true;
}

void GUI::destroy()
{
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}