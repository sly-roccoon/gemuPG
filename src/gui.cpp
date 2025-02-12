#include "gui.h"
#include "command.h"
#include "audio.h"

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
	ImGui::Begin("Side", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

	if (ImGui::ColorButton("Generator Block", toImVec4(GENERATOR_COLOR), 0, {ICON_SIZE, ICON_SIZE}))
		Interface::getInstance().setSelection(BLOCK_GENERATOR);
	if (ImGui::ColorButton("Area", toImVec4(AREA_COLOR), 0, {ICON_SIZE, ICON_SIZE}))
		Interface::getInstance().setSelection(AREA);
	if (ImGui::ColorButton("Sequencer", toImVec4(SEQUENCER_COLOR), 0, {ICON_SIZE, ICON_SIZE}))
		Interface::getInstance().setSelection(BLOCK_SEQUENCER);

	ImGui::End();

	//------------------------------
	ImGui::SetNextWindowPos({ImGui::GetMainViewport()->GetCenter().x - ICON_SIZE * 2, ImGui::GetMainViewport()->Size.y - ICON_SIZE});
	ImGui::SetNextWindowSize({ICON_SIZE * 2, ICON_SIZE});
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	ImGui::Begin("Bottom Left", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

	if (Clock::getInstance().isRunning())
	{
		if (ImGui::Button("Pause", {ICON_SIZE, ICON_SIZE}))
			Interface::getInstance().togglePlayPause();
	}
	else
	{
		if (ImGui::Button("Play", {ICON_SIZE, ICON_SIZE}))
			Interface::getInstance().togglePlayPause();
	}

	ImGui::SameLine();
	if (ImGui::Button("Stop", {ICON_SIZE, ICON_SIZE}))
	{
		if (Clock::getInstance().isRunning())
			Interface::getInstance().togglePlayPause();

		Interface::getInstance().getGrid().stopSequence();
	}

	ImGui::End();
	ImGui::PopStyleVar(2);

	//-----------------------------

	ImGui::SetNextWindowPos({ImGui::GetMainViewport()->GetCenter().x, ImGui::GetMainViewport()->Size.y - ICON_SIZE});
	ImGui::SetNextWindowSize({ICON_SIZE * 4, ICON_SIZE});
	ImGui::Begin("Bottom Right", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

	float bpm = Clock::getInstance().getBPM();
	float vol = AudioEngine::getInstance().getVolume();

	ImGui::InputFloat("BPM", &bpm, 1.0f, 5.0f, "%.1f");
	ImGui::SliderFloat("Volume", &vol, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

	Clock::getInstance().setBPM(bpm);
	AudioEngine::getInstance().setVolume(vol);

	ImGui::End();
}