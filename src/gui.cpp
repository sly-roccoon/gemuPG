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
	ImGui::StyleColorsDark();

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

void GUI::drawToolbar()
{
	ImGui::SetNextWindowPos({0, ImGui::GetMainViewport()->GetCenter().y - (ICON_SIZE + 4) * 2});
	ImGui::SetNextWindowSize({ICON_SIZE + ICON_SIZE / 4, (ICON_SIZE + 4) * 4}); // TODO: find better way to adjust margins of button border
	ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

	if (ImGui::ColorButton("Generator Block", toImVec4(GENERATOR_COLOR), 0, {ICON_SIZE, ICON_SIZE}))
		Interface::getInstance().setSelection(SELECT_GENERATOR);
	if (ImGui::ColorButton("Area", toImVec4(AREA_COLOR), 0, {ICON_SIZE, ICON_SIZE}))
		Interface::getInstance().setSelection(SELECT_AREA);
	if (ImGui::ColorButton("Sequencer", toImVec4(SEQUENCER_COLOR), 0, {ICON_SIZE, ICON_SIZE}))
		Interface::getInstance().setSelection(SELECT_SEQUENCER);

	ImGui::End();
}